#include "isr.h"

#include <wiringPi.h>

#define MAX_BUTTON_COUNT 8

namespace {

bool isButtonSlotInUse[MAX_BUTTON_COUNT] = {0};
int ports[MAX_BUTTON_COUNT] = {-1};
Nan::Callback * buttonCallbacks[MAX_BUTTON_COUNT];
Nan::AsyncWorker * workers[MAX_BUTTON_COUNT];

class InvokeCallbackWorker : public Nan::AsyncWorker {
 public:
  InvokeCallbackWorker(int slot, Nan::Callback *callback)
    : Nan::AsyncWorker(callback), slot_(slot) {}
  ~InvokeCallbackWorker() {}

  void Execute () {
    // Executed inside the worker-thread.
    // nothing to do
  }

  // this function will be run inside the main event loop
  // so it is safe to use V8 again
  void HandleOKCallback () {
    Nan::HandleScope scope;
    if (isButtonSlotInUse[slot_]) {
        workers[slot_] = new InvokeCallbackWorker(slot_, new Nan::Callback(buttonCallbacks[slot_]->GetFunction()));

        int currentVal = digitalRead(ports[slot_]);
        v8::Local<v8::Value> argv[] = {
            Nan::Null()
          , Nan::New<v8::Number>(currentVal)
        };

        callback->Call(2, argv);
    }
  }

 private:
  int slot_;
};

// TODO: maybe should use template or macro?
void onInputButton0Both(void) {
  // This is in interrupt routine, so you don't want to call the node callback directly from current thread.
  // We need to let event loop handle it instead.
  if (!isButtonSlotInUse[0]) return;
  if (workers[0]) {
    Nan::AsyncQueueWorker(workers[0]);
    workers[0] = static_cast<InvokeCallbackWorker *>(0);
  }
}
void onInputButton1Both(void) {
  // This is in interrupt routine, so you don't want to call the node callback directly from current thread.
  // We need to let event loop handle it instead.
  if (!isButtonSlotInUse[1]) return;
  if (workers[1]) {
    Nan::AsyncQueueWorker(workers[1]);
    workers[1] = static_cast<InvokeCallbackWorker *>(0);
  }
}
void onInputButton2Both(void) {
  // This is in interrupt routine, so you don't want to call the node callback directly from current thread.
  // We need to let event loop handle it instead.
  if (!isButtonSlotInUse[2]) return;
  if (workers[2]) {
    Nan::AsyncQueueWorker(workers[2]);
    workers[2] = static_cast<InvokeCallbackWorker *>(0);
  }
}
void onInputButton3Both(void) {
  // This is in interrupt routine, so you don't want to call the node callback directly from current thread.
  // We need to let event loop handle it instead.
  if (!isButtonSlotInUse[3]) return;
  if (workers[3]) {
    Nan::AsyncQueueWorker(workers[3]);
    workers[3] = static_cast<InvokeCallbackWorker *>(0);
  }
}
void onInputButton4Both(void) {
  // This is in interrupt routine, so you don't want to call the node callback directly from current thread.
  // We need to let event loop handle it instead.
  if (!isButtonSlotInUse[4]) return;
  if (workers[4]) {
    Nan::AsyncQueueWorker(workers[4]);
    workers[4] = static_cast<InvokeCallbackWorker *>(0);
  }
}
void onInputButton5Both(void) {
  // This is in interrupt routine, so you don't want to call the node callback directly from current thread.
  // We need to let event loop handle it instead.
  if (!isButtonSlotInUse[5]) return;
  if (workers[5]) {
    Nan::AsyncQueueWorker(workers[5]);
    workers[5] = static_cast<InvokeCallbackWorker *>(0);
  }
}
void onInputButton6Both(void) {
  // This is in interrupt routine, so you don't want to call the node callback directly from current thread.
  // We need to let event loop handle it instead.
  if (!isButtonSlotInUse[6]) return;
  if (workers[6]) {
    Nan::AsyncQueueWorker(workers[6]);
    workers[6] = static_cast<InvokeCallbackWorker *>(0);
  }
}
void onInputButton7Both(void) {
  // This is in interrupt routine, so you don't want to call the node callback directly from current thread.
  // We need to let event loop handle it instead.
  if (!isButtonSlotInUse[7]) return;
  if (workers[7]) {
    Nan::AsyncQueueWorker(workers[7]);
    workers[7] = static_cast<InvokeCallbackWorker *>(0);
  }
}

void (*isrCallbacks[])() = {
    onInputButton0Both,
    onInputButton1Both,
    onInputButton2Both,
    onInputButton3Both,
    onInputButton4Both,
    onInputButton5Both,
    onInputButton6Both,
    onInputButton7Both
};

int findAvaSlot() {
    for (int i=0; i<MAX_BUTTON_COUNT; i++) {
        if (!isButtonSlotInUse[i])
            return i;
    }
    Nan::ThrowTypeError("Run out of available button slots");
    return -1; // should never reach here
}
} // anonymous namespace

// 3 parameters: port, "PUD_UP", callback
// return: slotId
void buttonSetup(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 3) {
    Nan::ThrowTypeError("Wrong number of arguments, expected 3");
    return;
  }

  if (!info[0]->IsNumber() || !info[1]->IsString() || !info[2]->IsFunction()) {
    Nan::ThrowTypeError("Wrong arguments");
    return;
  }

  double arg0 = info[0]->NumberValue();
  int port = (int)arg0;
  v8::Local<v8::String> arg1 = info[1]->ToString();
  Nan::Utf8String pud(arg1);

  int slot = findAvaSlot();
  if (slot < 0) return; // run out of slots?

  isButtonSlotInUse[slot] = true;
  ports[slot] = port;
  buttonCallbacks[slot] = new Nan::Callback(info[2].As<v8::Function>());
  workers[slot] = new InvokeCallbackWorker(slot, new Nan::Callback(buttonCallbacks[slot]->GetFunction()));

  int pudInt = PUD_OFF;
  if (strcmp(*pud, "PUD_UP") == 0) {
    pudInt = PUD_UP;
  } else if (strcmp(*pud, "PUD_DOWN") == 0) {
    pudInt = PUD_DOWN;
  }

  pinMode (port, INPUT);
  pullUpDnControl(port, pudInt);
  if ( wiringPiISR (port, INT_EDGE_BOTH, isrCallbacks[slot]) < 0 ) {
      fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno));
      Nan::ThrowTypeError("Unable to setup ISR for button");
  }

  info.GetReturnValue().Set(slot);
}


void buttonGet(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Wrong number of arguments, expected 1");
    return;
  }

  if (!info[0]->IsNumber()) {
    Nan::ThrowTypeError("Wrong arguments");
    return;
  }

  double arg0 = info[0]->NumberValue();
  int slot = (int)arg0;

  int val = digitalRead (ports[slot]);  // 1/0
  info.GetReturnValue().Set(val);
}

void buttonRelease(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (info.Length() < 1) {
    Nan::ThrowTypeError("Wrong number of arguments, expected 1");
    return;
  }

  if (!info[0]->IsNumber()) {
    Nan::ThrowTypeError("Wrong arguments");
    return;
  }

  double arg0 = info[0]->NumberValue();
  int slot = (int)arg0;

  isButtonSlotInUse[slot] = false;
  ports[slot] = -1;
  delete(buttonCallbacks[slot]);
  buttonCallbacks[slot] = static_cast<Nan::Callback*>(0);
  workers[slot] = static_cast<Nan::AsyncWorker*>(0);
}
