package types

type ConnectionCreator interface {
	Create() (interface{}, error)
	Close(interface{})
}
