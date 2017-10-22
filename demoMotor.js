"use strict"

var motor = require('./index').motor;
var pin = motor(23, 25);

var val = 1;
pin(val);

function timeout() {
    setTimeout(timeout, 1000);
    val = val - 0.2;
    if (val < -1.1) {
        pin.close();
        motor.shutdown();
        process.exit(0);
    }
    pin(val);
}

setTimeout(timeout, 1000);
