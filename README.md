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

## inputBank (setup multiple pins, read all at once, as bit mask)
``` js
var inputBank = require('perf-gpio').inputBank();
// wiring-pi 25/24/23 = GPIO26/19/13 = PIN37/35/33
var bank = inputBank([25, 24, 23], "PUD_DOWN");

setTimeout(timeout, 1000);
function timeout() {
    setTimeout(timeout, 1000);
    var val = bank();
    // bit mask: bit 0 <= wp25, bit 1 <= wp24, bit 2 <= wp23
    console.log(val);
}
```
* Note: `inputBank` support batch read, but don't support edge-triggered callback. If you need callback, consider using `button`.
* Note: `inputBank` and `button` can share the same pin. Under the hood they are both "input" mode, thus no conflict.


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

## led (DMA-PWM basic)
``` js
var led = require('perf-gpio').led();
var pin = led(25); // wiringPi 25, GPIO 26
pin(0.5); // 50% on
```

## led (DMA-PWM cnt.)
``` js
var led = require('perf-gpio').led();
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
var motor = require('perf-gpio').motor();
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
Note: DC motor and LED shares the same DMA channel (14 for pi2/3/zero and 7 for pi4), 5ms cycle with 2us step size.

## Servo control
``` js
var servo = require('perf-gpio').servo();
var pin = servo(29); // wiringpi_25=gpio_21=Phys_40

pin(1500); // 1.5ms (mid)

setTimeout(function () {
  pin(500); // 0.5ms (min)
  setTimeout(function () {
    pin(2500); // 2.5ms (max)
    setTimeout(function () {
      pin(1500); // 1.5ms (mid)
      setTimeout(function () {
        servo.shutdown();
        process.exit(0);
      }, 500);
    }, 1000);
  }, 1000);
}, 1000);
```
Note: Servo runs on it's own DMA channel (13 for pi2/3/zero and 6 for pi4), 20ms cycle with 10us step size.

## Clock Output
``` js
var clockOut = require('./index').clockOut();

// wiring-pi 22 = GPIO6 = PIN31
var pinNum = 22;
var pin = clockOut(pinNum);
pin.setFeq(10000); // 10kHz
.....
pin.shutdown(); 
```
* Note: This hardware clock, only available from a few pin.
* Note: If don't shutdown, pi continue output clock even after your programe exits!
* Note: valid range from [4.7k, 19.2M] (Hz)

# Why perf-gpio?
* Performance is the main reason I write this library.
* perf-gpio is based on c wiringPi, most of the interrupt handling is done in c code. This is especially important for quadrature_decoder, so far perf-gpio is probably the best quadrature decoder available on raspberry-pi.
* The DMA-PWM is based on rpio-pwm, which is the best (if not the only) DMA-PWM solution on node.js (that's the reason I spend my time created it).
* perf-gpio also supports pull-up/down resistors (which is a must-have for my project).


# license

MIT
