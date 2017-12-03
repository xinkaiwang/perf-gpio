"use strict"

var qd = require('./index').quadrature_decoder_high_res;

// wiringpi PIN 0 and 1, maps to BCM GPIO 17 and 18
// https://pinout.xyz/pinout/wiringpi#
var counter = qd(28,29);

timeout();

function timeout() {
    setTimeout(timeout, 1000);
    console.log('' + counter.getCounter() + ',' + counter.getConflictCount());
}
