"use strict"

var onoff = require('./index').onoff;

// https://pinout.xyz/pinout/wiringpi#
// wiring-pi 15 = GPIO14 = PIN8
var led = onoff(15);

var current = false;

setTimeout(timeout, 1000);
function timeout() {
    setTimeout(timeout, 1000);
    current = !current;
    led.set(current);
    console.log(current);
}
