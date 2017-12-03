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
void onoffSetup(const Nan::FunctionCallbackInfo<v8::Value>& info);
void onoffSet(const Nan::FunctionCallbackInfo<v8::Value>& info);
void onoffRelease(const Nan::FunctionCallbackInfo<v8::Value>& info);

// button
void buttonSetup(const Nan::FunctionCallbackInfo<v8::Value>& info);
void buttonGet(const Nan::FunctionCallbackInfo<v8::Value>& info);
