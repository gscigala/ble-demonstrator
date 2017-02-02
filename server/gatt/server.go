// +build

package main

import (
	"fmt"
	"log"

	"github.com/paypal/gatt"
	"github.com/gscigala/ble-demonstrator/server/gatt/option"
	"github.com/gscigala/ble-demonstrator/server/gatt/service"
)

func main() {
	d, err := gatt.NewDevice(option.DefaultServerOptions...)
	if err != nil {
		log.Fatalf("Failed to open device, err: %s", err)
	}

	// Register optional handlers.
	d.Handle(
		gatt.CentralConnected(func(c gatt.Central) { fmt.Println("Connect: ", c.ID()) }),
		gatt.CentralDisconnected(func(c gatt.Central) { fmt.Println("Disconnect: ", c.ID()) }),
	)

	// A mandatory handler for monitoring device state.
	onStateChanged := func(d gatt.Device, s gatt.State) {
		fmt.Printf("State: %s\n", s)
		switch s {
		case gatt.StatePoweredOn:
			// Setup GAP and GATT services for Linux implementation.
			// OS X doesn't export the access of these services.
			d.AddService(service.NewGapService("Temperature")) // no effect on OS X
			d.AddService(service.NewGattService())        // no effect on OS X

			// A simple temperauture service for demo.
			s1 := service.NewTemperatureService()
			d.AddService(s1)

			s2 := service.NewBatteryService()
			d.AddService(s2)
			
			// Advertise device name and service's UUIDs.
			d.AdvertiseNameAndServices("Temperature", []gatt.UUID{s1.UUID(), s2.UUID()})

			// Advertise as an OpenBeacon iBeacon
			d.AdvertiseIBeacon(gatt.MustParseUUID("AA6062F098CA42118EC4193EB73CCEB6"), 1, 2, -59)

		default:
		}
	}

	d.Init(onStateChanged)
	select {}
}
