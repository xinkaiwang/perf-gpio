//index.js

var isr = require('./build/Release/isr.node');

var isQdSetup = 0;

function quadrature_decoder(port1, port2) {
    if (isQdSetup) {
        throw new Error('currently we only support 1 quadrature_decoder instance');
    } else {
        isQdSetup = 1;
        isr.qdSetup(port1, port2);
        return {
            getCounter: function() { return isr.qdGetCounter(); },
            getIgnoredCount: function() { return isr.qdGetIgnoredCount(); },
            getConflictCount: function() { return isr.qdGetConflictCount(); }
        };
    }
}

function onoff(port) {
    isr.onoffSetup(port);
    return {
        set: function(bit) { isr.onoffSet(port, bit ? 1 : 0); }
    }
}

function button(port, pud) {
    pud = pud || 'PUD_OFF';
    var cb;
    isr.buttonSetup(port, pud, function(err, val) {
        // console.log(val);
        if(cb) {
            cb(err, val);
        }
    });
    return {
        get: function() { return isr.buttonGet(port); },
        watch: function(callback) { cb = callback; }
    };
}

button.PUD_OFF = 'PUD_OFF';
button.PUD_UP = 'PUD_UP';
button.PUD_DOWN = 'PUD_DOWN';

module.exports.quadrature_decoder = quadrature_decoder;
module.exports.onoff = onoff;
module.exports.button = button;
