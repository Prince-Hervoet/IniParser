package types

type LightConnection struct {
	pool       Pool
	updateAt   int64
	isInUse    bool
	isRepay    bool
	connection interface{}
}

type ConnectionCreator interface {
	Create() (interface{}, error)
	Close(interface{})
}
