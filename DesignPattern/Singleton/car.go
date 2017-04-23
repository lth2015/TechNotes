package main
import (
    "fmt"
    "sync"
    "time"
)


type MyCar struct {
    Brand string
    Type string
    Price string
}


var carInstance *MyCar
var once sync.Once


func MyCarInstance() *MyCar{
    return carInstance
}

func NewCar() *MyCar {
    once.Do(func() {
        carInstance = &MyCar{Brand: "LandRover", Type: "RangeRover", Price: "$4,0000"}
    })
    return carInstance 
}

func (m *MyCar)Run() {
    fmt.Println(m.Brand)
    delay := 100
    for {
        for _, r := range `-\|/` {
            fmt.Printf("\r%c", r)
            time.Sleep(time.Duration(delay) * time.Millisecond) 
        }
    }
}

func (m *MyCar)Info() {
    fmt.Println(m.Brand)
}


func main() {
/*
    benz := new(MyCar)
    benz.Brand = "Benz"
    benz.Type = "ML550"
    benz.Price = "$25,000"         
//    benz.Run()
    benz.Info()

    toyota := new(MyCar)
    toyota.Brand = "Toyota"
    toyota.Type = "Prado"
    toyota.Price = "$5,000"
//    toyota.Run()
    toyota.Info()
*/
//    landrover := MyCarInstance() 
//    landrover.Info()


    //aaa := MyCarInstance() 
    //aaa.Info()


    which := NewCar()
    which.Info()

    what := NewCar()
    what.Info()
    
    aaa := MyCarInstance() 
    aaa.Info()
}
