var qd = require('./index').quadrature_decoder;

// wiringpi PIN 0 and 1, maps to BCM GPIO 17 and 18
// https://pinout.xyz/pinout/wiringpi#
var counter = qd(0,1);

timeout();

function timeout() {
    setTimeout(timeout, 1000);
    console.log('' + counter.getCounter() + ',' + counter.getIgnoredCount() + ',' + counter.getConflictCount());
}
