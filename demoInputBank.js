"use strict"

var inputBank = require('./index').inputBank();

// Input bank allow you to read array of pin value and return 
// as a uint (first pin at bit 0, etc.)

// https://pinout.xyz/pinout/wiringpi#
// wiring-pi 25/24/23 = GPIO26/19/13 = PIN37/35/33
var bank = inputBank([25, 24, 23], "PUD_DOWN");

setTimeout(timeout, 1000);
function timeout() {
    setTimeout(timeout, 1000);
    var val = bank();
    console.log(val);
}
