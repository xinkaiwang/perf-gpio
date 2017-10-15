//quadratureLowRes.cc
#include "isr.h"

#include <errno.h>
#include <wiringPi.h>

// TODO: Currently this code can only support 1 quadrature-decoder (2 GPIO ports).
//       Adding support for multiple quadrature-decoders beyond the scope of my current project, I'm lazy.

// Use GPIO Pin 17, which is Pin 0 for wiringPi library

namespace {

int BUTTON1_PIN = 0;
int BUTTON2_PIN = 1;


// the event counter
volatile int eventCounter = 0;
volatile int conflictCount = 0;

volatile int processing = 0;

// 00 => 0
// 10 => 1
// 11 => 2
// 01 => 3

void onInputBFall(void) {
    processing++;
    if(processing > 1) {
        conflictCount++;
    }
    if (digitalRead(BUTTON1_PIN) == 0) {
        // 01 -> 00 = 3 -> 0
        eventCounter += 4;
    } else {
        // 11 -> 10 = 2 -> 1
        eventCounter -= 4;
    }
    processing--;
}

} // anonymous namespace

void qdLowResGetCounter(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  info.GetReturnValue().Set(eventCounter);
}

void qdLowResGetConflictCount(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  info.GetReturnValue().Set(conflictCount);
}

void qdLowResSetup(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 2) {
    Nan::ThrowTypeError("Wrong number of arguments, expected 2");
    return;
  }

  if (!info[0]->IsNumber() || !info[1]->IsNumber()) {
    Nan::ThrowTypeError("Wrong arguments");
    return;
  }

  double arg0 = info[0]->NumberValue();
  double arg1 = info[1]->NumberValue();
  BUTTON1_PIN = (int)arg0;
  BUTTON2_PIN = (int)arg1;

  // set Pin 17/0 generate an interrupt on high-to-low transitions
  // and attach myInterrupt() to the interrupt
  if ( wiringPiISR (BUTTON2_PIN, INT_EDGE_FALLING, &onInputBFall) < 0 ) {
      fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
      Nan::ThrowTypeError("Unable to setup ISR 2");
  }
}
