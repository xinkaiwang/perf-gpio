#include "isr.h"

#include <errno.h>
#include <wiringPi.h>

using namespace v8;

NAN_MODULE_INIT(Init) {

  // Quadrature_Decoder_high_res
  Nan::Set(target, Nan::New("qdHighResSetup").ToLocalChecked(),
               Nan::GetFunction(Nan::New<FunctionTemplate>(qdHighResSetup)).ToLocalChecked());
  Nan::Set(target, Nan::New("qdHighResGetCounter").ToLocalChecked(),
               Nan::GetFunction(Nan::New<FunctionTemplate>(qdHighResGetCounter)).ToLocalChecked());
  Nan::Set(target, Nan::New("qdHighResGetIgnoredCount").ToLocalChecked(),
               Nan::GetFunction(Nan::New<FunctionTemplate>(qdHighResGetIgnoredCount)).ToLocalChecked());
  Nan::Set(target, Nan::New("qdHighResGetConflictCount").ToLocalChecked(),
               Nan::GetFunction(Nan::New<FunctionTemplate>(qdHighResGetConflictCount)).ToLocalChecked());

  // onoff
  Nan::Set(target, Nan::New("onoffSetup").ToLocalChecked(),
               Nan::GetFunction(Nan::New<FunctionTemplate>(onoffSetup)).ToLocalChecked());
  Nan::Set(target, Nan::New("onoffSet").ToLocalChecked(),
               Nan::GetFunction(Nan::New<FunctionTemplate>(onoffSet)).ToLocalChecked());
  Nan::Set(target, Nan::New("onoffRelease").ToLocalChecked(),
               Nan::GetFunction(Nan::New<FunctionTemplate>(onoffRelease)).ToLocalChecked());

  // clock out
  Nan::Set(target, Nan::New("wiringPiSetup").ToLocalChecked(),
               Nan::GetFunction(Nan::New<FunctionTemplate>(wiringPiSetup)).ToLocalChecked());
  Nan::Set(target, Nan::New("clockOutSetup").ToLocalChecked(),
               Nan::GetFunction(Nan::New<FunctionTemplate>(clockOutSetup)).ToLocalChecked());
  Nan::Set(target, Nan::New("clockOutSetFeq").ToLocalChecked(),
               Nan::GetFunction(Nan::New<FunctionTemplate>(clockOutSetFeq)).ToLocalChecked());
  Nan::Set(target, Nan::New("resetPinMode").ToLocalChecked(),
               Nan::GetFunction(Nan::New<FunctionTemplate>(resetPinMode)).ToLocalChecked());
}

// void Init(v8::Local<v8::Object> exports) {
//   // sets up the wiringPi library
//   if (wiringPiSetup () < 0) {
//       fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno));
//   }

//   // Quadrature_Decoder_high_res

//   exports->Set(Nan::New("qdHighResSetup").ToLocalChecked(),
//                Nan::New<v8::FunctionTemplate>(qdHighResSetup)->GetFunction());
//   exports->Set(Nan::New("qdHighResGetCounter").ToLocalChecked(),
//                Nan::New<v8::FunctionTemplate>(qdHighResGetCounter)->GetFunction());
//   exports->Set(Nan::New("qdHighResGetIgnoredCount").ToLocalChecked(),
//                Nan::New<v8::FunctionTemplate>(qdHighResGetIgnoredCount)->GetFunction());
//   exports->Set(Nan::New("qdHighResGetConflictCount").ToLocalChecked(),
//                Nan::New<v8::FunctionTemplate>(qdHighResGetConflictCount)->GetFunction());

//   // Quadrature_Decoder_low_res
//   exports->Set(Nan::New("qdLowResSetup").ToLocalChecked(),
//                Nan::New<v8::FunctionTemplate>(qdLowResSetup)->GetFunction());
//   exports->Set(Nan::New("qdLowResGetCounter").ToLocalChecked(),
//                Nan::New<v8::FunctionTemplate>(qdLowResGetCounter)->GetFunction());
//   exports->Set(Nan::New("qdLowResGetConflictCount").ToLocalChecked(),
//                Nan::New<v8::FunctionTemplate>(qdLowResGetConflictCount)->GetFunction());

//   // onoff output 1 bit
//   exports->Set(Nan::New("onoffSetup").ToLocalChecked(),
//                Nan::New<v8::FunctionTemplate>(onoffSetup)->GetFunction());
//   exports->Set(Nan::New("onoffSet").ToLocalChecked(),
//                Nan::New<v8::FunctionTemplate>(onoffSet)->GetFunction());
//   exports->Set(Nan::New("onoffRelease").ToLocalChecked(),
//                Nan::New<v8::FunctionTemplate>(onoffRelease)->GetFunction());

//   // button input 1 bit
//   exports->Set(Nan::New("buttonSetup").ToLocalChecked(),
//                Nan::New<v8::FunctionTemplate>(buttonSetup)->GetFunction());
//   exports->Set(Nan::New("buttonGet").ToLocalChecked(),
//                Nan::New<v8::FunctionTemplate>(buttonGet)->GetFunction());

//   // input bank N bits (0<=N<=31)
//   exports->Set(Nan::New("inputBankSetup").ToLocalChecked(),
//                Nan::New<v8::FunctionTemplate>(inputBankSetup)->GetFunction());
//   exports->Set(Nan::New("inputBankGet").ToLocalChecked(),
//                Nan::New<v8::FunctionTemplate>(inputBankGet)->GetFunction());

//   // clock out
//   exports->Set(Nan::New("wiringPiSetup").ToLocalChecked(),
//                Nan::New<v8::FunctionTemplate>(wiringPiSetup)->GetFunction());
//   exports->Set(Nan::New("clockOutSetup").ToLocalChecked(),
//                Nan::New<v8::FunctionTemplate>(clockOutSetup)->GetFunction());
//   exports->Set(Nan::New("clockOutSetFeq").ToLocalChecked(),
//                Nan::New<v8::FunctionTemplate>(clockOutSetFeq)->GetFunction());
//   exports->Set(Nan::New("resetPinMode").ToLocalChecked(),
//                Nan::New<v8::FunctionTemplate>(resetPinMode)->GetFunction());

// }

NODE_MODULE(isr, Init)
