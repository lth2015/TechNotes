interface vs nil

interface{} has two member, it equals to nil only when both members is nil.

unsafe.Pointer()

[](https://groups.google.com/forum/#!topic/golang-nuts/wnH302gBa4I/discussion)
>This is a common source of confusion.  The basic answer is to never store something in an interface if you don't expect the methods to be called on it.  The language may allow it, but that violates the semantics of the interface.  To expound, a nil value should usually not be stored in an interface unless it is of a type that has explicitly handled that case in its pointer-valued methods and has no value-receiver methods.
