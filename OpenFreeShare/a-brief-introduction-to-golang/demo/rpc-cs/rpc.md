```golang
    // main
    ...
    client, err := rpc.DialHTTP("tcp", "localhost:8080")
    err = client.Call("Arith.Multiply", args, &reply)
    ...
```


```golang
    // main
    ...
    rpc.Register(arith)
    rpc.HandleHTTP()
    err := http.ListenAndServe(":8080", nil)
    ...

    // Multiply
    func (t *Arith) Multiply(args *Args, reply *int) error {
    ...
    *reply = args.A * args.B
    return nil
    ...
}
```
