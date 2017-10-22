"use strict"

// https://pinout.xyz/pinout/wiringpi#
var wiringpi2GPIOMap = {
    0: 17,
    1: 18,
    23: 13,
    24: 19,
    25: 26,
};

function wiringpi2Gpio(wiringPiPin) {
    if(wiringPiPin in wiringpi2GPIOMap) {
        return wiringpi2GPIOMap[wiringPiPin];
    } else {
        throw "unknown wiringPiPin:" + wiringPiPin;
    }
}

module.exports.toGpio = wiringpi2Gpio;