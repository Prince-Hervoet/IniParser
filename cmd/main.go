package main

import lightpool "LightSqlPool/internal/light_pool"

type MyConnectionCreator struct {
}

func (this *MyConnectionCreator) Create() (interface{}, error) {
	return nil, nil
}

func (this *MyConnectionCreator) Close(connection interface{}) {

}

func main() {
	cr := &MyConnectionCreator{}
	pc := &lightpool.PoolConfig{
		CoreConnections: 32,
		MaxConnections:  256,
		WaitTimeout:     10000,
		Creator:         cr,
	}
	lp, err := lightpool.NewLightPool(pc)
	if err != nil {
		return
	}
	_, err2 := lp.Get()
	if err2 != nil {
		return
	}
}
