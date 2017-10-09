//index.js

var isr = require('./build/Release/isr.node');

var isSetup = 0;

function init(port1, port2) {
    if (isSetup) {
        throw new Error('currently we only support 1 quadrature_decoder instance');
    } else {
        isSetup = 1;
        isr.setup(port1, port2);
        return {
            getCounter: function() { return isr.getCounter(); },
            getIgnoredCount: function() { return isr.getIgnoredCount(); },
            getConflictCount: function() { return isr.getConflictCount(); }
        };
    }
}

module.exports.quadrature_decoder = init;