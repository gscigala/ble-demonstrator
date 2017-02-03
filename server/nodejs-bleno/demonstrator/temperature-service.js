var util = require('util');

var bleno = require('..');

var BlenoPrimaryService = bleno.PrimaryService;

var TemperatureCharacteristic = require('./temperature-characteristic');

function TemperatureService() {
  TemperatureService.super_.call(this, {
      uuid: 'FFE0',
      characteristics: [
          new TemperatureCharacteristic()
      ]
  });
}

util.inherits(TemperatureService, BlenoPrimaryService);

module.exports = TemperatureService;
