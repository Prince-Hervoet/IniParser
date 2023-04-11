package lightpool

import (
	"errors"
	"sync"
)

const DEFAULT_LIMIT = 1024

type ReadList struct {
	mu    *sync.Mutex
	data  []interface{}
	limit int32
	size  int32
}

func NewReadList(limit int32) (*ReadList, error) {
	if limit < 0 {
		limit = DEFAULT_LIMIT
	} else if limit == 0 {
		return nil, errors.New("Limit is 0")
	}
	return &ReadList{
		mu:    &sync.Mutex{},
		data:  make([]interface{}, limit),
		limit: limit,
		size:  0,
	}, nil
}

func (this *ReadList) Get(index int32) interface{} {
	return this.data[index]
}

func (this *ReadList) GetSize() int32 {
	return this.size
}
