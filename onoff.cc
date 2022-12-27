#include "isr.h"

#include <wiringPi.h>

NAN_METHOD(onoffSetup) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Wrong number of arguments, expected 2");
    return;
  }

  if (!info[0]->IsNumber()) {
    Nan::ThrowTypeError("Wrong arguments");
    return;
  }

  Nan::Maybe<double> arg0 = Nan::To<double>(info[0]);
  int port = (int)arg0.FromJust();

  pinMode (port, OUTPUT);
}

NAN_METHOD(onoffSet) {
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
  int port = (int)arg0.FromJust();
  int val = (int)arg1.FromJust();

  digitalWrite (port, val ? HIGH : LOW);  // On/off
}

NAN_METHOD(onoffRelease) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Wrong number of arguments, expected 2");
    return;
  }

  if (!info[0]->IsNumber()) {
    Nan::ThrowTypeError("Wrong arguments");
    return;
  }

  Nan::Maybe<double> arg0 = Nan::To<double>(info[0]);
  int port = (int)arg0.FromJust();

  pinMode (port, INPUT);
}
