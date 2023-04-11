package lightpool

import (
	"errors"
	"sync"
)

const DEFAULT_LIMIT = 1024

type readList struct {
	mu    *sync.Mutex
	data  []*LightConnection
	limit int32
	size  int32
}

func newReadList(limit int32) (*readList, error) {
	if limit < 0 {
		limit = DEFAULT_LIMIT
	} else if limit == 0 {
		return nil, errors.New("Limit is 0")
	}
	return &readList{
		mu:    &sync.Mutex{},
		data:  make([]*LightConnection, limit),
		limit: limit,
		size:  0,
	}, nil
}

func (this *readList) get(index int32) interface{} {
	return this.data[index]
}

func (this *readList) getSize() int32 {
	return this.size
}

func (this *readList) update() {

}
