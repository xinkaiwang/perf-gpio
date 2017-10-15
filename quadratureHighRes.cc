//quadratureHighRes.cc
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
volatile int ignoredCount = 0;
volatile int inputA = 0;
volatile int inputB = 0;

volatile int processing = 0;

// 00 => 0
// 10 => 1
// 11 => 2
// 01 => 3

void onInputABoth(void) {
    processing++;
    if(processing > 1) {
        conflictCount++;
    }
    if (digitalRead(BUTTON1_PIN) == 1) {
        if (inputA == 1) {
            ignoredCount++;
        } else {
            if(inputB == 0) {
                // 00 -> 10 = 0 -> 1
                eventCounter++;
            } else {
                // 01 -> 11 = 3 -> 2
                eventCounter--;
            }
            inputA = 1;
        }
    } else {
        if (inputA == 0) {
            ignoredCount++;
        } else {
            if(inputB == 0) {
                // 10 -> 00 = 1 -> 0
                eventCounter--;
            } else {
                // 11 -> 01 = 2 -> 3
                eventCounter++;
            }
            inputA = 0;
        }
    }
    processing--;
}

void onInputBBoth(void) {
    processing++;
    if(processing > 1) {
        conflictCount++;
    }
    if (digitalRead(BUTTON2_PIN) == 1) {
        if (inputB == 1) {
            ignoredCount++;
        } else {
            if(inputA == 0) {
                // 00 -> 01 = 0 -> 3
                eventCounter--;
            } else {
                // 10 -> 11 = 1 -> 2
                eventCounter++;
            }
            inputB = 1;
        }
    } else {
        if (inputB == 0) {
            ignoredCount++;
        } else {
            if(inputA == 0) {
                // 01 -> 00 = 3 -> 0
                eventCounter++;
            } else {
                // 11 -> 10 = 2 -> 1
                eventCounter--;
            }
            inputB = 0;
        }
    }
    processing--;
}
} // anonymous namespace

void qdHighResGetCounter(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  info.GetReturnValue().Set(eventCounter);
}

void qdHighResGetConflictCount(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  info.GetReturnValue().Set(conflictCount);
}

void qdHighResGetIgnoredCount(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  info.GetReturnValue().Set(ignoredCount);
}

void qdHighResSetup(const Nan::FunctionCallbackInfo<v8::Value>& info) {
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
  if ( wiringPiISR (BUTTON1_PIN, INT_EDGE_BOTH, &onInputABoth) < 0 ) {
      fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
      Nan::ThrowTypeError("Unable to setup ISR 1");
  }
  if ( wiringPiISR (BUTTON2_PIN, INT_EDGE_BOTH, &onInputBBoth) < 0 ) {
      fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
      Nan::ThrowTypeError("Unable to setup ISR 2");
  }

  inputA = digitalRead(BUTTON1_PIN);
  inputB = digitalRead(BUTTON2_PIN);
}
