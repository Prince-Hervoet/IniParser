package lightpool

import "sync"

type waitList struct {
	mu    *sync.Mutex
	cond  *sync.Cond
	data  []*LightConnection
	size  int32
	limit int32
}

func newWaitList(limit int32) *waitList {
	mu := &sync.Mutex{}
	cond := sync.NewCond(mu)
	return &waitList{
		mu:    mu,
		cond:  cond,
		data:  make([]*LightConnection, limit),
		limit: limit,
		size:  0,
	}
}

func (this *waitList) get() *LightConnection {
	this.mu.Lock()
	defer this.mu.Unlock()
	for this.size == 0 {
		this.cond.Wait()
	}
	lc := this.data[this.size-1]
	this.size -= 1
	return lc
}

func (this *waitList) put(lc *LightConnection) bool {
	this.mu.Lock()
	defer this.mu.Unlock()
	if this.size == this.limit {
		return false
	}
	this.data[this.size] = lc
	this.cond.Signal()
	return true
}
