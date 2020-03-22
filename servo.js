'use strict'

var gs = require('global-singleton');
var toGpio = require('./wiringpiPins').toGpio;

function init() {
  var ch = gs('rpio-pwm.servo', function () {
    var pwm = require('rpio-pwm');

    var chNum = pwm.host_is_model_pi4() ? 6 : 13;
    var cfg = {
      cycle_time_us: 20000, // cycle time 20000us (20ms)
      step_time_us: 10, // per step 10us
      delay_hw: 1, // 1=PCM (since LED/motor default use PWM, so servo use PCM to avoid conflict)
      invert: 0, // 0=no invert
    };

    var ch = pwm.create_dma_channel(chNum, cfg);
    cfg.step_count = Math.round(cfg.cycle_time_us/cfg.step_time_us);

    ch.cfg = cfg;
    return ch;
  });

  function createServo(wiringPiPin) {
    var gpio = toGpio(wiringPiPin);
    var pin = ch.create_pwm(gpio);

    function set_value(time_us) {
      if (time_us < 500) {
        time_us = 500;
      } else if (time_us > 2500) {
        time_us = 2500;
      }
      var width = Math.round(time_us/ch.cfg.step_time_us);
      pin.set_width(width);
    }
    set_value.shutdown = function() {
      pin.release();
    }
    return set_value;
  }

  createServo.shutdown = ch.shutdown;
  return createServo;
}

module.exports = init;