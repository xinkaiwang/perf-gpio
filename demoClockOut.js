"use strict"

var clockOut = require('./index').clockOut();

// Input bank allow you to read array of pin value and return 
// as a uint (first pin at bit 0, etc.)

// https://pinout.xyz/pinout/wiringpi#
// wiring-pi 22 = GPIO6 = PIN31
var pinNum = 22;
var pin = clockOut(pinNum);
pin.setFeq(10000); // 10kHz

setTimeout(timeout, 5000);
function timeout() {
    pin.shutdown(); // if don't shutdown, pi will continue output clock even after your programe exits!
}
