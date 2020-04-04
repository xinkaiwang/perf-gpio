'use strict'

var isr = require('./build/Release/isr.node');

function init() {
  // pinNum: wiringPi pin Num
  // feq: range from [4.7k, 19.2M] (Hz)
  function createClockOut(pinNum) {
    isr.clockOutSetup(pinNum);

    var pin = {};
    function setFeq(newFeq) {
      isr.clockOutSetFeq(pinNum, newFeq);
      return pin;
    }
    function shutdown() {
      isr.resetPinMode(pinNum);
      return pin;
    }
    pin.setFeq = setFeq;
    pin.shutdown = shutdown;
    return pin;
  }

  return createClockOut;
}

module.exports = init;