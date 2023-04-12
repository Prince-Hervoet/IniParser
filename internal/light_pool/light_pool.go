package lightpool

import (
	"LightSqlPool/pkg/types"
	"errors"
	"sync"
	"sync/atomic"
	"time"
)

const NOT_IN_USE = 0
const IN_USE = 1

type LightPool struct {
	firstList          *readList
	secondList         *waitList
	maxConnections     int32
	coreConnections    int32
	currentConnections int32
	idleConnections    int32
	waitTimeout        int64
	creator            types.ConnectionCreator
	mu                 *sync.Mutex
}

type LightConnection struct {
	pool       *LightPool
	updateAt   int64
	isInUse    int32
	isRepay    bool
	isCore     bool
	connection interface{}
}

type PoolConfig struct {
	CoreConnections int32
	MaxConnections  int32
	WaitTimeout     int64
	creator         types.ConnectionCreator
}

func NewLightPool(c *PoolConfig) (*LightPool, error) {
	if c.MaxConnections <= 0 || c.CoreConnections <= 0 || c.creator == nil {
		return nil, errors.New("invalid number")
	}
	if c.WaitTimeout <= 0 {
		c.WaitTimeout = -1
	}
	pool := &LightPool{
		firstList:       newReadList(c.MaxConnections),
		secondList:      newWaitList(c.MaxConnections),
		maxConnections:  c.MaxConnections,
		coreConnections: c.CoreConnections,
		waitTimeout:     c.WaitTimeout,
		idleConnections: 0,
		creator:         c.creator,
	}
	now := time.Now().UnixMilli()
	for i := int32(0); i < pool.coreConnections; i++ {
		conn, err := pool.creator.Create()
		if err != nil {
			pool.creator.Close(conn)
			return nil, errors.New("open connection error")
		}
		lc := &LightConnection{
			pool:       pool,
			updateAt:   now,
			isInUse:    NOT_IN_USE,
			isRepay:    true,
			isCore:     true,
			connection: conn,
		}
		pool.firstList.data[i] = lc
	}
	pool.firstList.size += pool.coreConnections
	return pool, nil
}

func (this *LightPool) Get() (*LightConnection, error) {
	for c := 0; c < 2; c++ {
		for i := int32(0); i < this.firstList.size; i++ {
			lc := this.firstList.data[i]
			ok := atomic.CompareAndSwapInt32(&lc.isInUse, NOT_IN_USE, IN_USE)
			if ok {
				lc.isRepay = false
				return lc, nil
			}
		}
	}
	if this.currentConnections < this.maxConnections {
		now := time.Now().UnixMilli()
		this.mu.Lock()
		if this.currentConnections < this.maxConnections {
			defer this.mu.Unlock()
			conn, err := this.creator.Create()
			if err != nil {
				return nil, errors.New("open connection error")
			}
			lc := &LightConnection{
				pool:       this,
				updateAt:   now,
				isInUse:    NOT_IN_USE,
				isRepay:    true,
				isCore:     false,
				connection: conn,
			}
			this.currentConnections += 1
			return lc, nil
		} else {
			this.mu.Unlock()
		}
	}
	lc := this.secondList.get()
	return lc, nil
}

func (this *LightPool) repay(lc *LightConnection) {
	if lc == nil {
		return
	}
	if lc.isCore {
		lc.isInUse = NOT_IN_USE
	} else {
		if !this.secondList.put(lc) {
			this.creator.Close(lc)
			this.currentConnections -= 1
		}
	}
}

func (this *LightConnection) Get() (interface{}, error) {
	if this.isRepay {
		return nil, errors.New("this connecion has been repaid")
	}
	return this.connection, nil
}

func (this *LightConnection) Repay() {
	this.isRepay = true
	this.pool.repay(this)
}
