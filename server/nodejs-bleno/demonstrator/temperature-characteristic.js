var util = require('util');
var os = require('os');
var exec = require('child_process').exec;

var bleno = require('..');

var Descriptor = bleno.Descriptor;
var Characteristic = bleno.Characteristic;

var temperature = 50;

var TemperatureCharacteristic = function() {
  TemperatureCharacteristic.super_.call(this, {
    uuid: 'FFE1',
      properties: ['read','notify'],
    descriptors: [
      new Descriptor({
        uuid: '2901',
        value: 'Temperature in Celsius. If enabled,  notifications will be sent when the temperature change'
      })
    ]
  });
};

util.inherits(TemperatureCharacteristic, Characteristic);

TemperatureCharacteristic.prototype.onReadRequest = function(offset, callback) {
    console.log('TemperatureCharacteristic read :', temperature);
    
    // return value
    callback(this.RESULT_SUCCESS, new Buffer([temperature]));
};

TemperatureCharacteristic.prototype.onSubscribe = function(maxValueSize, updateValueCallback) {
    console.log('TemperatureCharacteristic subscribe');

    this.changeInterval = setInterval(function() {

        if (temperature != 0) {
            temperature = temperature - 1;
        }
        else {
            temperature = 50;
        }

        console.log('TemperatureCharacteristic update value: ' + temperature);
        
        var data = new Buffer([temperature]);
        updateValueCallback(data);
        
    }.bind(this), 60000);
};

TemperatureCharacteristic.prototype.onUnsubscribe = function() {
    console.log('TemperatureCharacteristic unsubscribe');

    if (this.changeInterval) {
        clearInterval(this.changeInterval);
        this.changeInterval = null;
    }
};

TemperatureCharacteristic.prototype.onNotify = function() {
    console.log('TemperatureCharacteristic on notify');
};

module.exports = TemperatureCharacteristic;
