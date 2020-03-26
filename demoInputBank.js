"use strict"

var inputBank = require('./index').inputBank();

// https://pinout.xyz/pinout/wiringpi#
// wiring-pi 2 = GPIO27 = PIN13
var bank = inputBank([25, 24], "PUD_DOWN");

setTimeout(timeout, 1000);
function timeout() {
    setTimeout(timeout, 1000);
    var val = bank();
    console.log(val);
}
