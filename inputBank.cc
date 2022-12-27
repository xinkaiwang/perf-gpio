#include "isr.h"

#include <errno.h>
#include <wiringPi.h>

// 2 parameters: portMask, PULL
// return: none
void inputBankSetup(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 2) {
    Nan::ThrowTypeError("Wrong number of arguments, expected 2");
    return;
  }

  if (!info[0]->IsNumber() || !info[1]->IsString()) {
    Nan::ThrowTypeError("Wrong arguments");
    return;
  }

  Nan::Maybe<double> arg0 = Nan::To<double>(info[0]);
  int portMap = (int)arg0.FromJust();
  Nan::Maybe<double> arg1 = Nan::To<double>(info[1]);
  int pull = (int)arg1.FromJust(); // 0=off, 1=pull_up, 2=pull_down

  int pudInt = PUD_OFF;
  if (pull == 1) {
    pudInt = PUD_UP;
  } else if (pull == 2) {
    pudInt = PUD_DOWN;
  }

  uint32_t mask = 0x01;
  for (int i=0;i<32;i++, mask<<=1) {
    if (portMap && mask) {
      pinMode (i, INPUT);
      pullUpDnControl(i, pudInt);
    }
  }
}

// 1 parameters: portMask
// return: value (bit mask)
void inputBankGet(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Wrong number of arguments, expected 1");
    return;
  }

  if (!info[0]->IsNumber()) {
    Nan::ThrowTypeError("Wrong arguments");
    return;
  }

  Nan::Maybe<double> arg0 = Nan::To<double>(info[0]);
  int portMap = (int)arg0.FromJust();

  uint32_t valueMask = 0;
  uint32_t mask = 0x01;
  for (int i=0;i<32;i++,mask<<=1) {
    if (portMap && mask) {
      if (digitalRead (i)) {
        valueMask |= mask;
      }
    }
  }

  info.GetReturnValue().Set(valueMask);
}