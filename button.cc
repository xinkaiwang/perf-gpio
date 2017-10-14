#include "isr.h"

#include <wiringPi.h>

namespace {

Nan::Callback * buttonCallback;

Nan::AsyncWorker * worker;
class InvokeCallbackWorker : public Nan::AsyncWorker {
 public:
  InvokeCallbackWorker(Nan::Callback *callback)
    : Nan::AsyncWorker(callback) {}
  ~InvokeCallbackWorker() {}

  void Execute () {
    // Executed inside the worker-thread.
    // nothing to do
  }

  // this function will be run inside the main event loop
  // so it is safe to use V8 again
  void HandleOKCallback () {
    Nan::HandleScope scope;
    worker = new InvokeCallbackWorker(new Nan::Callback(buttonCallback->GetFunction()));

    int currentVal = digitalRead(3);
    v8::Local<v8::Value> argv[] = {
        Nan::Null()
      , Nan::New<v8::Number>(currentVal)
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
    Nan::AsyncQueueWorker(worker);
    worker = static_cast<InvokeCallbackWorker *>(0);
  }
}
} // anonymous namespace

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
  buttonCallback = new Nan::Callback(info[2].As<v8::Function>());
  worker = new InvokeCallbackWorker(new Nan::Callback(buttonCallback->GetFunction()));

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
