"use strict"

// https://pinout.xyz/pinout/wiringpi#
var wiringpi2GPIOMap = {
    0: 17,
    1: 18,
    2: 27,
    3: 22,
    4: 23,
    5: 24,
    6: 25,
    7: 4,
    8: 2,
    9: 3,
    10: 8,
    11: 7,
    12: 10,
    13: 9,
    14: 11,
    15: 14,
    16: 15,
    21: 5,
    22: 6,
    23: 13,
    24: 19,
    25: 26,
    26: 12,
    27: 16,
    28: 20,
    29: 21,
    30: 0,
    31: 1,
};

function wiringpi2Gpio(wiringPiPin) {
    if(wiringPiPin in wiringpi2GPIOMap) {
        return wiringpi2GPIOMap[wiringPiPin];
    } else {
        throw "unknown wiringPiPin:" + wiringPiPin;
    }
}

module.exports.toGpio = wiringpi2Gpio;