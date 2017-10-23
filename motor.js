'use strict'

function init() {
    var led = require('./index').led();

    function motor(port1, port2) {
        var pin1 = led(port1);
        var pin2 = led(port2);

        var currentVal = 0;
        pin1(0);
        pin2(0);

        function setVal(newVal) {
            if(newVal >= 0) {
                pin2(0);
                pin1(newVal);
            } else if (newVal < 0) {
                pin1(0);
                pin2(-newVal);
            }
            currentVal = newVal;
        }

        setVal.close = function() {
            pin1(0);
            pin2(0);
        };
        return setVal;
    }

    motor.shutdown = led.shutdown;
    return motor;
}

module.exports = init;