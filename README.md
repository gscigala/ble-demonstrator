README
======

BLE communication demonstrator, using bleno for the server and bluez for the client.

Summary
-------

Server installation

- Copy sources

Client installation

- Compile BlueZ C program

Server installation
-------------------


The device simulation uses bleno, a Node.js module for implementing BLE peripherals. For more informations about noble, please visit https://github.com/sandeepmistry/bleno

### Copy sources

Copy server/demonstrator directory in the bleno directory:

`node_modules/bleno`

Client installation
-------------------

NOTE : The current C BlueZ program is just the BlueZ gatt example !!

### Compile BlueZ C program

To compile BlueZ C program, go in the `client/bluez` directory and execute:

`clang *.c -o client -lbluetooth`
