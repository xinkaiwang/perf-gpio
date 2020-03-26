#include "isr.h"

#include <errno.h>
#include <wiringPi.h>

// 2 parameters: portMask, "PUD_UP"
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

  double arg0 = info[0]->NumberValue();
  int portMap = (int)arg0;
  v8::Local<v8::String> arg1 = info[1]->ToString();
  Nan::Utf8String pud(arg1);

  int pudInt = PUD_OFF;
  if (strcmp(*pud, "PUD_UP") == 0) {
    pudInt = PUD_UP;
  } else if (strcmp(*pud, "PUD_DOWN") == 0) {
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

  double arg0 = info[0]->NumberValue();
  int portMap = (int)arg0;

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