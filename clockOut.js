'use strict'

var isr = require('./build/Release/isr.node');

function init() {
  // pinNum: wiringPi pin Num
  // feq: range from [4.7k, 19.2M] (Hz)
  function createClockOut(pinNum, feq) {
    isr.clockOutSetup(pinNum, feq);

    return {}; // place-holder
  }

  return createClockOut;
}

module.exports = init;