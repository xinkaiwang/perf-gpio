'use strict'

var isr = require('./build/Release/isr.node');

function init() {
  function createInputBank(portList, pud = 'PUD_OFF') {
    var portMask = 0;
    for (var i = 0;i<portList.length;i++) {
      portMask |= (1<<portList[i]);
    }

    isr.inputBankSetup(portMask, pud);

    function getVal() {
      var valueMask = isr.inputBankGet(portMask);

      var result = 0;
      for (var i = portList.length-1 ;i>=0;i--) {
        result <<= 1;
        if (valueMask & (1<<portList[i])) {
          result |= 1;
        }
      }
      return result;
    }

    return getVal;
  }

  return createInputBank;
}

module.exports = init;