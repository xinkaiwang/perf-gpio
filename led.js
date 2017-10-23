'use strict'

var gs = require('global-singleton');
var toGpio = require('./wiringpiPins').toGpio;

var resolution = 1; // 1us
var loopTimeUs = 5000; // 5ms
var loopSize = loopTimeUs/resolution;

function init() {
    var ch = gs('rpio-pwm.ch14', function() {
        var pwm = gs('rpio-pwm', function() {
            var pwm = require('rpio-pwm');
            pwm.setup(1); // 1us resolution
            return pwm;
        });

        var channel = 14;
        pwm.init_channel(channel, loopTimeUs); // DMA channel 14; loop interval 5ms

        function add_pulse(gpio, start, width) {
            pwm.add_channel_pulse(channel, gpio, start, width);
        }
        function clear_gpio(gpio) {
            pwm.clear_channel_gpio(channel, gpio);
        }
        function clear_channel() {
            pwm.clear_channel(channel);
        }
        function shutdown() {
            pwm.cleanup();
        }
        return {
            add_pulse: add_pulse,
            clear_gpio: clear_gpio,
            clear_channel: clear_channel,
            shutdown: shutdown,
        };
    });

    function createPin(wiringPiPin) {
        var currentVal = 0;
        var currentIntVal = 0;
        var gpio = toGpio(wiringPiPin);

        function setVal(val) {
            if (val < 0) {
                val = 0;
            } else if (val > 1) {
                val = 1;
            }
            if (currentVal != val) {
                var newIntVal = Math.round(val * loopSize);
                ch.add_pulse(gpio, 0, newIntVal);
                currentIntVal = newIntVal;
                currentVal = val;
                if (val == 0) {
                    ch.clear_gpio(gpio);
                }
            }
        }
        return setVal;
    }

    createPin.shutdown = ch.shutdown;
    return createPin;
}

module.exports = init;