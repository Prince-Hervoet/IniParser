package lightpool

import (
	"sync"
)

const DEFAULT_LIMIT = 1024

type readList struct {
	mu    *sync.Mutex
	data  []*LightConnection
	limit int32
	size  int32
}

func newReadList(limit int32) *readList {
	if limit < 0 {
		limit = DEFAULT_LIMIT
	} else if limit == 0 {
		return nil
	}
	return &readList{
		mu:    &sync.Mutex{},
		data:  make([]*LightConnection, limit),
		limit: limit,
		size:  0,
	}
}
