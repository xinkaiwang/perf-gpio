#pragma once

#include <nan.h>

// quadratureDecoder
void qdGetCounter(const Nan::FunctionCallbackInfo<v8::Value>& info);
void qdGetConflictCount(const Nan::FunctionCallbackInfo<v8::Value>& info);
void qdGetIgnoredCount(const Nan::FunctionCallbackInfo<v8::Value>& info);
void qdSetup(const Nan::FunctionCallbackInfo<v8::Value>& info);

// onoff
void onoffSetup(const Nan::FunctionCallbackInfo<v8::Value>& info);
void onoffSet(const Nan::FunctionCallbackInfo<v8::Value>& info);

// button
void buttonSetup(const Nan::FunctionCallbackInfo<v8::Value>& info);
void buttonGet(const Nan::FunctionCallbackInfo<v8::Value>& info);
