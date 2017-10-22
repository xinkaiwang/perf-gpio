# perf-gpio
High performance GPIO tool kit for raspberry pi, including quadrature_decoder, DC motor control, soft PWM, buttons, etc. Based on c wiringpi.

# install

```
npm install perf-gpio --save
```

# API Example 
https://pinout.xyz/pinout/wiringpi# is the best doc I can find for wiringpi Pin number mappings.
## onoff -- simple
``` js
var onoff = require('perf-gpio').onoff;

// wiring-pi 2 = GPIO27 = PIN13
var led = onoff(2);
led.set(1);
```

## onoff -- flash
``` js
var onoff = require('perf-gpio').onoff;

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
```

## button -- simple read
``` js
var button = require('perf-gpio').button;

// wiring-pi 3 = GPIO22 = PIN15
var b = button(3, button.PUD_UP); // use raspberry pi build-in 50k pull-up resistor

setTimeout(timeout, 1000);
function timeout() {
    setTimeout(timeout, 1000);
    var current = b.get();
    console.log(current);
}
```

## button -- callback
``` js
var button = require('perf-gpio').button;

// wiring-pi 3 = GPIO22 = PIN15
var b = button(3, button.PUD_UP);

b.watch(function(err, status) {
    console.log(status? 'UP' : 'DOWN');
});
```

## quadrature_decoder
``` js
var qd = require('perf-gpio').quadrature_decoder;

// quadrature_decoder you need 2 pins, phase A and B
// wiringpi PIN 0 and 1, maps to BCM GPIO 17 and 18
var counter = qd(0,1);

timeout();

function timeout() {
    setTimeout(timeout, 1000);
    console.log(counter.getCounter());
}
```

## led (basic)
``` js
var pin = require('perf-gpio').led(25); // wiringPi 25, GPIO 26
pin(0.5); // 50% on
```

## led (more complete)
``` js
var led = require('perf-gpio').led;
var pin = led(25); // wiringPi 25, GPIO 26
pin(1);

setTimeout(function() {
    pin(0.5);
    setTimeout(function() {
        pin.close(); // turn off led before quit
        led.shutdown(); // shutdown DMA device before quit
        process.exit(0);
    }, 2000);
}, 2000);
```

## DC motor control
``` js
var motor = require('./index').motor;
// Need 2 pin to control a DC motor (forward/backword)
// you need a H-bridge module to drive a DC motor
var pin = motor(23, 25); 

pin(-0.5); // anywhere range from [-1.0, +1.0]

function timeout() {
    pin.close(); // turn off motor before quit
    motor.shutdown(); // shutdown DMA device before quit
    process.exit(0);
}

setTimeout(timeout, 2000);
```

# How it works
Performance is the main reason I write this lib. perf-gpio is based on c wiringPi, most of the interrupt handling is done in c code. This is especially important for quadrature_decoder, so far perf-gpio is probably the best quadrature decoder available on raspberry-pi. And also perf-gpio is probably the only node.js gpio lib which supports pull-up/down resistors (which is a must-have for my project).  DMA based Soft-PWM is based on rpio-pwm.


# license

MIT
