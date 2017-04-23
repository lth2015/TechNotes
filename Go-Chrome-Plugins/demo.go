package main

import (
	"github.com/fabioberger/chrome"
	"honnef.co/go/js/dom"
	"strconv"
)

func main() {
	c := chrome.NewChrome()

	tabDetails := chrome.Object{
		"active": false,
	}
	/*
	c.Tabs.Create(tabDetails, func(tab chrome.Tab) {
		notification := "Tab with id: " + strconv.Itoa(tab.Id) + " created!"
		dom.GetWindow().Document().GetElementByID("notification").SetInnerHTML(notification)
	})
	*/

	c.Tabs.Create(tabDetails, func(tab chrome.Tab) {
		notification := "Tab: " + strconv.Itoa(tab.Id) + " Hello World!"
		dom.GetWindow().Document().GetElementByID("notification").SetInnerHTML(notification)
	})

}
