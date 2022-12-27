#pragma once

#include <nan.h>

// quadratureHighRes
void qdHighResGetCounter(const Nan::FunctionCallbackInfo<v8::Value>& info);
void qdHighResGetConflictCount(const Nan::FunctionCallbackInfo<v8::Value>& info);
void qdHighResGetIgnoredCount(const Nan::FunctionCallbackInfo<v8::Value>& info);
void qdHighResSetup(const Nan::FunctionCallbackInfo<v8::Value>& info);

// quadratureLowRes
void qdLowResGetCounter(const Nan::FunctionCallbackInfo<v8::Value>& info);
void qdLowResGetConflictCount(const Nan::FunctionCallbackInfo<v8::Value>& info);
void qdLowResSetup(const Nan::FunctionCallbackInfo<v8::Value>& info);

// onoff
NAN_METHOD(onoffSetup);
NAN_METHOD(onoffSet);
NAN_METHOD(onoffRelease);
// void onoffSetup(const Nan::FunctionCallbackInfo<v8::Value>& info);
// void onoffSet(const Nan::FunctionCallbackInfo<v8::Value>& info);
// void onoffRelease(const Nan::FunctionCallbackInfo<v8::Value>& info);

// button
void buttonSetup(const Nan::FunctionCallbackInfo<v8::Value>& info);
void buttonGet(const Nan::FunctionCallbackInfo<v8::Value>& info);

// inputBank
void inputBankSetup(const Nan::FunctionCallbackInfo<v8::Value>& info);
void inputBankGet(const Nan::FunctionCallbackInfo<v8::Value>& info);

// clockOut
NAN_METHOD(wiringPiSetup);
NAN_METHOD(clockOutSetup);
NAN_METHOD(clockOutSetFeq);
NAN_METHOD(resetPinMode);
// void wiringPiSetup(const Nan::FunctionCallbackInfo<v8::Value>& info);
// void clockOutSetup(const Nan::FunctionCallbackInfo<v8::Value>& info);
// void clockOutSetFeq(const Nan::FunctionCallbackInfo<v8::Value>& info);
// void resetPinMode(const Nan::FunctionCallbackInfo<v8::Value>& info);
