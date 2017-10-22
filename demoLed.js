"use strict"

var led = require('./index').led;
var pin = led(25);

pin(1);

setTimeout(function() {
    pin(0.5);
    setTimeout(function() {
        pin(0);
        led.shutdown();
        process.exit(0);
    }, 2000);
}, 2000);
