'use strict'

var gs = require('global-singleton');
var toGpio = require('./wiringpiPins').toGpio;

function init() {
  var ch = gs('rpio-pwm.5ms', function () {
    var pwm = require('rpio-pwm');

    var chNum = pwm.host_is_model_pi4() ? 7 : 14;
    var cfg = {
      cycle_time_us: 5000, // cycle time 5000us (5ms)
      step_time_us: 2, // per step 2us
      delay_hw: 0, // 0=PWM
      invert: 0, // 0=no invert
    };

    var ch = pwm.create_dma_channel(chNum, cfg);
    cfg.step_count = Math.round(cfg.cycle_time_us/cfg.step_time_us);

    ch.cfg = cfg;
    return ch;
  });

  function createPin(wiringPiPin) {
    var gpio = toGpio(wiringPiPin);
    var pin = ch.create_pwm(gpio);
    function set_value(pct) {
      if (pct < 0) {
        pct = 0;
      } else if (pct > 1) {
        pct = 1;
      }
      var width = Math.round(ch.cfg.step_count * pct);
      pin.set_width(width);
    }
    set_value.shutdown = function() {
      pin.release();
    }
    return set_value;
  }

  createPin.shutdown = ch.shutdown;
  return createPin;
}

module.exports = init;