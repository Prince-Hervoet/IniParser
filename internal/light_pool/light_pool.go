package lightpool

import (
	"LightSqlPool/pkg/types"
	"errors"
)

type LightPool struct {
	firstList          []*LightConnection
	secondList         chan (*LightConnection)
	maxConnections     int32
	minConnections     int32
	currentConnections int32
	idleConnections    int32
	waitTimeout        int64
	creator            types.ConnectionCreator
}

type LightConnection struct {
	pool       *LightPool
	updateAt   int64
	isInUse    bool
	isRepay    bool
	connection interface{}
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

	pool := &LightPool{
		firstList:       make([]*LightConnection, 0),
		secondList:      make(chan *LightConnection, 128),
		maxConnections:  c.MaxConnections,
		minConnections:  c.MinConnections,
		waitTimeout:     c.WaitTimeout,
		idleConnections: 0,
		creator:         c.creator,
	}

	for i := int32(0); i < pool.minConnections; i++ {
		conn, err := pool.creator.Create()
		if err != nil {
			pool.creator.Close(conn)
			return nil, errors.New("Open connection error")
		}

	}
	return pool, nil

}

func (this *LightPool) Get() {

}

func (this *LightPool) Repay() {

}

func (this *LightPool) Close() {

}
