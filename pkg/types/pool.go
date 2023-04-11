package types

type Pool interface {
	// get connection
	Get()
	// repay connection
	Repay()
	// close connection
	Close()
	GetCurrent() int32
	GetMax() int32
	Stop()
}
