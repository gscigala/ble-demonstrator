// +build

package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"strings"
	"time"

	"github.com/paypal/gatt"
	"github.com/gscigala/ble-demonstrator/client/golang-gatt/option"
)

var done = make(chan struct{})

func onStateChanged(d gatt.Device, s gatt.State) {
	fmt.Println("State:", s)
	switch s {
	case gatt.StatePoweredOn:
		fmt.Println("Scanning...")
		d.Scan([]gatt.UUID{}, false)
		return
	default:
		d.StopScanning()
	}
}

func onPeriphDiscovered(p gatt.Peripheral, a *gatt.Advertisement, rssi int) {
	id := strings.ToUpper(flag.Args()[0])
	if strings.ToUpper(p.ID()) != id {
		return
	}

	// Stop scanning once we've got the peripheral we're looking for.
	p.Device().StopScanning()

	fmt.Printf("\nPeripheral ID:%s, NAME:(%s)\n", p.ID(), p.Name())
	fmt.Println("  Local Name        =", a.LocalName)
	fmt.Println("  TX Power Level    =", a.TxPowerLevel)
	fmt.Println("  Manufacturer Data =", a.ManufacturerData)
	fmt.Println("  Service Data      =", a.ServiceData)
	fmt.Println("")

	p.Device().Connect(p)
}

func onPeriphConnected(p gatt.Peripheral, err error) {
	fmt.Println("Connected")
	defer p.Device().CancelConnection(p)

	if err := p.SetMTU(500); err != nil {
		fmt.Printf("Failed to set MTU, err: %s\n", err)
	}

  // Discovery services
	ss, err := p.DiscoverServices(nil)
	if err != nil {
		fmt.Printf("Failed to discover services, err: %s\n", err)
		return
	}

	for _, s := range ss {
// Get the temperature characteristic which is identified by the UUID: \xFFE1
		cs, err := p.DiscoverCharacteristics(nil, s)
		if err != nil {
			log.Printf("ERROR: Failed to discover characteristics - %s\n", err)
			continue
		}

		for _, c := range cs {
			if (c.UUID().Equal(gatt.UUID16(0xFFE1))) {
				// Read the characteristic
				if (c.Properties() & gatt.CharRead) != 0 {
					b, err := p.ReadCharacteristic(c)
					if err != nil {
						fmt.Printf("Failed to read characteristic, err: %s\n", err)
						continue
					}
					fmt.Printf("Read characteristic: temperature value %d | 0x%x | %q\n", b, b, b)
				}


				// Discover the characteristic descriptors.
				_, err := p.DiscoverDescriptors(nil, c)
				if err != nil {
					log.Printf("ERROR: Failed to discover descriptors - %s\n", err)
					continue
				}

				// Subscribe the characteristic, if possible.
				if (c.Properties() & (gatt.CharNotify | gatt.CharIndicate)) != 0 {
					f := func(c *gatt.Characteristic, b []byte, err error) {
						fmt.Printf("notified: %d | 0x%x | %q\n", b, b, b)
					}
					if err := p.SetNotifyValue(c, f); err != nil {
						fmt.Printf("Failed to subscribe characteristic, err: %s\n", err)
						continue
					}
				}
			}
		}
		log.Println()
	}
	
	fmt.Printf("Waiting for 5 minutes to get some notifiations.\n")
	time.Sleep(5 * time.Minute)
	fmt.Printf("End of notification time.\n");
}

func onPeriphDisconnected(p gatt.Peripheral, err error) {
	fmt.Println("Disconnected")
	close(done)
}

func main() {
	flag.Parse()
	if len(flag.Args()) != 1 {
		log.Fatalf("usage: %s [options] peripheral-id\n", os.Args[0])
	}

	d, err := gatt.NewDevice(option.DefaultClientOptions...)
	if err != nil {
		log.Fatalf("Failed to open device, err: %s\n", err)
		return
	}

	// Register handlers.
	d.Handle(
		gatt.PeripheralDiscovered(onPeriphDiscovered),
		gatt.PeripheralConnected(onPeriphConnected),
		gatt.PeripheralDisconnected(onPeriphDisconnected),
	)

	d.Init(onStateChanged)
	<-done
	fmt.Println("Done")
}
