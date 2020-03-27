'use strict'

var isr = require('./build/Release/isr.node');

function init() {
  // pinNum: wiringPi pin Num
  // feq: range from [4.7k, 19.2M] (Hz)
  function createClockOut(pinNum) {
    isr.clockOutSetup(pinNum);

    function setFeq(newFeq) {
      isr.clockOutSetFeq(pinNum, newFeq);
    }
    function shutdown() {
      isr.resetPinMode(pinNum);
    }
    return {
      setFeq: setFeq,
      shutdown: shutdown
    };
  }

  return createClockOut;
}

module.exports = init;