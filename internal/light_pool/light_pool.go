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
	secondList         chan (*LightConnection)
	maxConnections     int32
	minConnections     int32
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
	Connection interface{}
}

type PoolConfig struct {
	MinConnections int32
	MaxConnections int32
	WaitTimeout    int64
	creator        types.ConnectionCreator
}

func NewLightPool(c *PoolConfig) (*LightPool, error) {
	if c.MaxConnections <= 0 || c.MinConnections <= 0 || c.creator == nil {
		return nil, errors.New("Invalid number")
	}
	if c.WaitTimeout <= 0 {
		c.WaitTimeout = -1
	}
	rl, err := newReadList(c.MaxConnections)
	if err != nil {
		return nil, errors.New("ReadList init error")
	}
	pool := &LightPool{
		firstList:       rl,
		secondList:      make(chan *LightConnection, 128),
		maxConnections:  c.MaxConnections,
		minConnections:  c.MinConnections,
		waitTimeout:     c.WaitTimeout,
		idleConnections: 0,
		creator:         c.creator,
	}

	now := time.Now().UnixMilli()
	for i := int32(0); i < pool.minConnections; i++ {
		conn, err := pool.creator.Create()
		if err != nil {
			pool.creator.Close(conn)
			return nil, errors.New("Open connection error")
		}
		lc := &LightConnection{
			pool:       pool,
			updateAt:   now,
			isInUse:    NOT_IN_USE,
			isRepay:    true,
			Connection: conn,
		}
		pool.firstList.data[i] = lc
	}
	pool.firstList.size += pool.minConnections
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
		this.mu.Lock()
		if this.currentConnections < this.maxConnections {
			defer this.mu.Unlock()
			conn, err := this.creator.Create()
			if err != nil {
				return nil, errors.New("Open connection error")
			}
			now := time.Now().UnixMilli()
			lc := &LightConnection{
				pool:       this,
				updateAt:   now,
				isInUse:    NOT_IN_USE,
				isRepay:    true,
				Connection: conn,
			}
			this.currentConnections += 1
			return lc, nil
		} else {
			this.mu.Unlock()
		}
	}
	return nil, nil
}

func (this *LightPool) Repay() {

}

func (this *LightPool) Close() {

}
