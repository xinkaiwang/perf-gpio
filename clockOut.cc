#include "isr.h"

#include <errno.h>
#include <wiringPi.h>

void wiringPiSetupSys(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  wiringPiSetupSys();
}

void clockOutSetup(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 2) {
    Nan::ThrowTypeError("Wrong number of arguments, expected 2");
    return;
  }

  if (!info[0]->IsNumber() || !info[1]->IsNumber()) {
    Nan::ThrowTypeError("Wrong arguments");
    return;
  }

  double arg0 = info[0]->NumberValue();
  int wpPin = (int)arg0;
  double arg1 = info[1]->NumberValue();
  int feq = (int)arg1;

  pinMode(wpPin, GPIO_CLOCK);
  // range from [4.7k, 19.2M] (Hz)
  gpioClockSet(wpPin, feq);
}
