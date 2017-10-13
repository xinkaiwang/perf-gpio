#include <nan.h>
#include <wiringPi.h>

using v8::Value;
using Nan::AsyncQueueWorker;
using Nan::AsyncWorker;
using Nan::Callback;
using Nan::HandleScope;
using Nan::New;
using Nan::Null;

// TODO: Currently this code can only support 1 quadrature-decoder (2 GPIO ports).
//       Adding support for multiple quadrature-decoders beyond the scope of my current project, I'm lazy.

// Use GPIO Pin 17, which is Pin 0 for wiringPi library

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

Callback * buttonCallback;
AsyncWorker * worker;
class InvokeCallbackWorker : public AsyncWorker {
 public:
  InvokeCallbackWorker(Callback *callback)
    : AsyncWorker(callback) {}
  ~InvokeCallbackWorker() {}

  void Execute () {
    // Executed inside the worker-thread.
    // nothing to do
  }

  // this function will be run inside the main event loop
  // so it is safe to use V8 again
  void HandleOKCallback () {
    HandleScope scope;
    worker = new InvokeCallbackWorker(new Callback(buttonCallback->GetFunction()));

    int currentVal = digitalRead(3);
    v8::Local<Value> argv[] = {
        Null()
      , New<v8::Number>(currentVal)
    };

    callback->Call(2, argv);
  }

 private:
  int val_;
};

void onInputButton1Both(void) {
  // This is in interrupt routine, so you don't want to call the node callback directly from current thread.
  // We need to let event loop handle it instead.
  if (worker) {
    AsyncQueueWorker(worker);
    worker = static_cast<AsyncWorker *>(0);
  }
}

void qdGetCounter(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  info.GetReturnValue().Set(eventCounter);
}

void qdGetConflictCount(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  info.GetReturnValue().Set(conflictCount);
}

void qdGetIgnoredCount(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  info.GetReturnValue().Set(ignoredCount);
}

void qdSetup(const Nan::FunctionCallbackInfo<v8::Value>& info) {
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

void onoffSetup(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Wrong number of arguments, expected 2");
    return;
  }

  if (!info[0]->IsNumber()) {
    Nan::ThrowTypeError("Wrong arguments");
    return;
  }

  double arg0 = info[0]->NumberValue();
  int port = (int)arg0;

  pinMode (port, OUTPUT);
}

void onoffSet(const Nan::FunctionCallbackInfo<v8::Value>& info) {
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
  int port = (int)arg0;
  int val = (int)arg1;

  digitalWrite (port, val ? HIGH : LOW);  // On/off
}

void buttonSetup(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 3) {
    Nan::ThrowTypeError("Wrong number of arguments, expected 3");
    return;
  }

  if (!info[0]->IsNumber() || !info[1]->IsString()) {
    Nan::ThrowTypeError("Wrong arguments");
    return;
  }

  double arg0 = info[0]->NumberValue();
  v8::Local<v8::String> arg1 = info[1]->ToString();
  Nan::Utf8String pud(arg1);
  buttonCallback = new Callback(info[2].As<v8::Function>());
  worker = new InvokeCallbackWorker(new Callback(buttonCallback->GetFunction()));

  int pudInt = PUD_OFF;
  if (strcmp(*pud, "PUD_UP") == 0) {
    pudInt = PUD_UP;
  } else if (strcmp(*pud, "PUD_DOWN") == 0) {
    pudInt = PUD_DOWN;
  }
  int port = (int)arg0;

  pinMode (port, INPUT);
  pullUpDnControl(port, pudInt);
  if ( wiringPiISR (port, INT_EDGE_BOTH, &onInputButton1Both) < 0 ) {
      fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
      Nan::ThrowTypeError("Unable to setup ISR for button");
  }
}

void buttonGet(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Wrong number of arguments, expected 2");
    return;
  }

  if (!info[0]->IsNumber()) {
    Nan::ThrowTypeError("Wrong arguments");
    return;
  }

  double arg0 = info[0]->NumberValue();
  int port = (int)arg0;

  int val = digitalRead (port);  // 1/0
  info.GetReturnValue().Set(val);
}


void Init(v8::Local<v8::Object> exports) {
  // sets up the wiringPi library
  if (wiringPiSetup () < 0) {
      fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno));
  }

  // Quadrature-Decoder
  exports->Set(Nan::New("qdSetup").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(qdSetup)->GetFunction());
  exports->Set(Nan::New("qdGetCounter").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(qdGetCounter)->GetFunction());
  exports->Set(Nan::New("qdGetIgnoredCount").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(qdGetIgnoredCount)->GetFunction());
  exports->Set(Nan::New("qdGetConflictCount").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(qdGetConflictCount)->GetFunction());

  // onoff output 1 bit
  exports->Set(Nan::New("onoffSetup").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(onoffSetup)->GetFunction());
  exports->Set(Nan::New("onoffSet").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(onoffSet)->GetFunction());

  // button input 1 bit
  exports->Set(Nan::New("buttonSetup").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(buttonSetup)->GetFunction());
  exports->Set(Nan::New("buttonGet").ToLocalChecked(),
               Nan::New<v8::FunctionTemplate>(buttonGet)->GetFunction());
}

NODE_MODULE(isr, Init)
