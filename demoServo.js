"use strict"

var servo = require('./index').servo();
var pin = servo(15); // wiringpi_15=gpio_14=Phys_8

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
