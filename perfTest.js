"use strict"

var output = require('./index').onoff(2);
var input = require('./index').button(3, 'PUD_UP');

/*
 * In this test, GPIO #7 is wired to one end of a 1kΩ current limiting
 * resistor and GPIO #8 is wired to the other end of the resistor. GPIO #7 is
 * an interrupt generating input and GPIO #8 is an output.
 * By toggling the state of the output an interrupt is generated.
 * The output is toggled as often as possible to determine the maximum
 * rate at which interrupts can be handled.
 */
var irqCount = 0;
var iv;

// Interrupt handler
input.watch(function (err, value) {
  irqCount += 1;

  // Trigger next interrupt by toggling output.
  output.set(value === 0 ? 1 : 0);
});

// Print number of interrupts once a second.
iv = setInterval(function () {
  console.log(irqCount);
  irqCount = 0;
}, 1000);

// Trigger first interrupt by toggling output.
output.set(input.get() === 0 ? 1 : 0);
