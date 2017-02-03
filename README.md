README
======

BLE communication demonstrator, using nodejs-bleno/golang-gatt for the server and bluez/bluez-dbus for the client.

Summary
-------

Server

- Run bleno simulation
- Rune gatt simulation

Client

- BlueZ C program

Server
------

The server has two different implementation, one using bleno and another gatt.

Bleno is a Node.js module implementing BLE peripherals. For more informations about noble, please visit https://github.com/sandeepmistry/bleno

Gatt is a go program implementing BLE client or server. For more informations about gatt, please visit https://github.com/paypal/gatt

### Run bleno simulation

After bleno installation, copy server/nodejs-bleno/demonstrator directory in the bleno directory:

`cp -r server/nodejs-bleno/demonstrator node_modules/bleno`

Go to the demonstrator directory:

`cd node_modules/bleno/demonstrator`

Run the peripheral simulation:

`sudo node main.js`

### Run gatt simulation

After golang and gatt installation, run the peripheral simulation program:

`sudo -E go run $GOPATH/src/github.com/gscigala/ble-demonstrator/server/golang-gatt/server.go`

Client
------

NOTE : The current C BlueZ program is just the BlueZ gatt example !!

### BlueZ C program

To compile BlueZ C program, go in the `client/bluez` directory and execute:

`clang *.c -o client -lbluetooth`
