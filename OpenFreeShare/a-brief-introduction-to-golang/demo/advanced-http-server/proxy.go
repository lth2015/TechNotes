package main

import (
	"fmt"
	"io/ioutil"
	"log"
	"net/http"
)

var host string
var port string

func Get(w http.ResponseWriter, r *http.Request) {
	url := "http://" + host + ":" + port
	resp, err := http.Get(url)
	if err != nil {
		log.Println(err)
		return
	}

	body, _ := ioutil.ReadAll(resp.Body)
	defer resp.Body.Close()

	fmt.Fprintf(w, "%s", string(body))

}

func Post(w http.ResponseWriter, r *http.Request) {

}

func Unknown(w http.ResponseWriter, r *http.Request) {

}

func main() {

	host = "localhost"
	port = "8080"

	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		switch r.Method {
		case "GET":
			Get(w, r)
		case "POST":
			Post(w, r)
		default:
			Unknown(w, r)
		}
	})
	log.Println("proxy listening :8081")
	http.ListenAndServe(":8081", nil)

}
