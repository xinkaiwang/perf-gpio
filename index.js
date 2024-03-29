//index.js

// https://pinout.xyz/pinout/wiringpi#

var isr = require('./build/Release/isr.node');

isr.wiringPiSetup();

var isQdLowResSetup = 0;
function quadrature_decoder(port1, port2) {
    if (isQdLowResSetup) {
        throw new Error('currently we only support 1 quadrature_decoder instance');
    } else {
        isQdLowResSetup = 1;
        isr.qdLowResSetup(port1, port2);
        return {
            getCounter: function() { return isr.qdLowResGetCounter(); },
            getConflictCount: function() { return isr.qdLowResGetConflictCount(); }
        };
    }
}

var isQdHighResSetup = 0;
function quadrature_decoder_high_res(port1, port2) {
    if (isQdHighResSetup) {
        throw new Error('currently we only support 1 quadrature_decoder_high instance');
    } else {
        isQdHighResSetup = 1;
        isr.qdHighResSetup(port1, port2);
        return {
            getCounter: function() { return isr.qdHighResGetCounter(); },
            getIgnoredCount: function() { return isr.qdHighResGetIgnoredCount(); },
            getConflictCount: function() { return isr.qdHighResGetConflictCount(); }
        };
    }
}

function onoff(port) {
    isr.onoffSetup(port);
    return {
        set: function(bit) { isr.onoffSet(port, bit ? 1 : 0); },
        release: function() { isr.onoffRelease(port); }
    }
}

function button(port, pud) {
    var pull = 0;
    if (pud === "PUD_UP") {
      pull = 1;
    } else if (pud === "PUD_DOWN") {
      pull = 2;
    }

    var cb;
    var slot = isr.buttonSetup(port, pull, function(err, val) {
        // console.log(val);
        if(cb) {
            cb(err, val);
        }
    });
    return {
        get: function() { return isr.buttonGet(slot); },
        watch: function(callback) { cb = callback; },
        release: function() { isr.release(slot); }
    };
}

button.PUD_OFF = 'PUD_OFF';
button.PUD_UP = 'PUD_UP';
button.PUD_DOWN = 'PUD_DOWN';

module.exports.quadrature_decoder = quadrature_decoder;
module.exports.quadrature_decoder_high_res = quadrature_decoder_high_res;
module.exports.onoff = onoff;
module.exports.button = button;
module.exports.led = require('./led');
module.exports.motor = require('./motor');
module.exports.servo = require('./servo');
module.exports.inputBank = require('./inputBank');
module.exports.clockOut = require('./clockOut');
