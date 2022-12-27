#include "isr.h"

#include <errno.h>
#include <wiringPi.h>

NAN_METHOD(wiringPiSetup) {
  wiringPiSetup();
}

NAN_METHOD(clockOutSetup) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Wrong number of arguments, expected 1");
    return;
  }

  if (!info[0]->IsNumber()) {
    Nan::ThrowTypeError("Wrong arguments");
    return;
  }

  Nan::Maybe<double> arg0 = Nan::To<double>(info[0]);
  int wpPin = (int)arg0.FromJust();

  pinMode(wpPin, GPIO_CLOCK);
}

NAN_METHOD(clockOutSetFeq) {
  if (info.Length() < 2) {
    Nan::ThrowTypeError("Wrong number of arguments, expected 2");
    return;
  }

  if (!info[0]->IsNumber() || !info[1]->IsNumber()) {
    Nan::ThrowTypeError("Wrong arguments");
    return;
  }

  Nan::Maybe<double> arg0 = Nan::To<double>(info[0]);
  Nan::Maybe<double> arg1 = Nan::To<double>(info[1]);
  int wpPin = (int)arg0.FromJust();
  int feq = (int)arg1.FromJust();

  printf("wppin=%d, feq=%d\n", wpPin, feq);
  // range from [4.7k, 19.2M] (Hz)
  gpioClockSet(wpPin, feq);
}

NAN_METHOD(resetPinMode) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Wrong number of arguments, expected 1");
    return;
  }

  if (!info[0]->IsNumber()) {
    Nan::ThrowTypeError("Wrong arguments");
    return;
  }

  Nan::Maybe<double> arg0 = Nan::To<double>(info[0]);
  int wpPin = (int)arg0.FromJust();

  pinMode(wpPin, INPUT); // pinMode=INPUT
}
