#include "isr.h"

#include <errno.h>
#include <wiringPi.h>

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
