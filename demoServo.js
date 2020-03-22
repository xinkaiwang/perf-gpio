"use strict"

var servo = require('./index').servo();
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
