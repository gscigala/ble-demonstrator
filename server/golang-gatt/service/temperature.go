package service

import (
	"fmt"
	"time"

	"github.com/paypal/gatt"
)

func NewTemperatureService() *gatt.Service {
	temperature := byte(50)
	s := gatt.NewService(gatt.UUID16(0xFFE0))
	c := s.AddCharacteristic(gatt.UUID16(0xFFE1))
	c.HandleReadFunc(
		func(rsp gatt.ResponseWriter, req *gatt.ReadRequest) {
			fmt.Printf("Read temperature : %d\n", temperature)
			rsp.Write([]byte{temperature})
			temperature--
		})
	c.HandleNotifyFunc(
		func(r gatt.Request, n gatt.Notifier) {
			fmt.Printf("Subscribe to temperature characteristic\n")
			for !n.Done() {
				fmt.Printf("Read temperature : %d\n", temperature)
				n.Write([]byte{temperature})
				temperature--
				time.Sleep(time.Minute)
			}
		})

	return s
}
