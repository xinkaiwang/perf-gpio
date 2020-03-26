"use strict"

var clockOut = require('./index').clockOut();

// Input bank allow you to read array of pin value and return 
// as a uint (first pin at bit 0, etc.)

// https://pinout.xyz/pinout/wiringpi#
// wiring-pi 22 = GPIO6 = PIN31
var pinNum = 22;
var pin = clockOut(pinNum, 100000); // 100kHz

setTimeout(timeout, 1000);
function timeout() {
    setTimeout(timeout, 1000);
}
