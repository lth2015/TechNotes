```golang
    resp, err := http.Get("http://localhost:8081")
	if err != nil {
		log.Println(err)
	}
	defer resp.Body.Close()

	body, err1 := ioutil.ReadAll(resp.Body)
	if err1 != nil {
		log.Println(err)
	}

	fmt.Printf("%s", string(body))

```
```golang
    remote, err := url.Parse("http://" + h.host + ":" + h.port)
	if err != nil {
		log.Println(err)
		return
	}

	proxy := httputil.NewSingleHostReverseProxy(remote)
	proxy.ServeHTTP(w, r)

    ...
    log.Println("proxy listening :8081")
	http.ListenAndServe(":8081", h)
```
```golang
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		fmt.Fprintf(w, "Hello World!\n")
	})

	log.Println("Server Listening :8080")
	http.ListenAndServe(":8080", nil)

```
