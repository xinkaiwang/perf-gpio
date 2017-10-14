"use strict"

var button = require('./index').button;

// https://pinout.xyz/pinout/wiringpi#
// wiring-pi 3 = GPIO22 = PIN15
var b = button(3, button.PUD_UP);

// b.watch(function(err, status) {
//     console.log(status? 'UP' : 'DOWN');
// });

setTimeout(timeout, 1000);
function timeout() {
    setTimeout(timeout, 1000);
    var current = b.get();
    console.log(current);
}
