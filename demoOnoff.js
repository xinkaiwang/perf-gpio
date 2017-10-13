var onoff = require('./index').onoff;

// wiring-pi 2 = GPIO27 = PIN13
var led = onoff(2);

var current = false;

setTimeout(timeout, 1000);
function timeout() {
    setTimeout(timeout, 1000);
    current = !current;
    led.set(current);
    console.log(current);
}
