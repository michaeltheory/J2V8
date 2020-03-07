/*******************************************************************************
* Copyright (c) 2014 EclipseSource and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*
* Contributors:
*    EclipseSource - initial API and implementation -- test
******************************************************************************/
#include <jni.h>
#include <libplatform/libplatform.h>
#include <iostream>
#include <v8.h>
#include <string.h>
#include <map>
#include <cstdlib>
#include "com_eclipsesource_v8_V8Impl.h"
#include <android/log.h>
#include <GLES3/gl3.h>

#define TAG "J2V8_V8Impl"

#define LOGD(fmt, ...) __android_log_print(ANDROID_LOG_DEBUG, TAG, fmt "\n--> %s\n----> %s:%d", ##__VA_ARGS__, __FILE__, __FUNCTION__, __LINE__)
#define LOGE(fmt, ...) __android_log_print(ANDROID_LOG_ERROR, TAG, fmt "\n--> %s\n----> %s:%d", ##__VA_ARGS__, __FILE__, __FUNCTION__, __LINE__)

#pragma comment(lib, "userenv.lib")
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "WINMM.lib")
#pragma comment( lib, "psapi.lib" )

using namespace std;
using namespace v8;

class MethodDescriptor {
public:
  jlong methodID;
  jlong v8RuntimePtr;
};

class WeakReferenceDescriptor {
public:
  jlong v8RuntimePtr;
  jlong objectHandle;
};

class V8Runtime {
public:
  Isolate* isolate;
  Persistent<Context> context_;
  Persistent<Object>* globalObject;
  Locker* locker;
  jobject v8;
  jthrowable pendingException;


};

std::unique_ptr<v8::Platform> v8Platform = nullptr;

const char* ToCString(const String::Utf8Value& value) {
  return *value ? *value : "<string conversion failed>";
}

JavaVM* jvm = nullptr;
jclass v8cls = nullptr;
jclass v8ObjectCls = nullptr;
jclass v8ArrayCls = nullptr;
jclass v8TypedArrayCls = nullptr;
jclass v8ArrayBufferCls = nullptr;
jclass v8FunctionCls = nullptr;
jclass undefinedV8ObjectCls = nullptr;
jclass undefinedV8ArrayCls = nullptr;
jclass v8ResultsUndefinedCls = nullptr;
jclass v8ScriptCompilationCls = nullptr;
jclass v8ScriptExecutionException = nullptr;
jclass v8RuntimeExceptionCls = nullptr;
jclass throwableCls = nullptr;
jclass stringCls = nullptr;
jclass integerCls = nullptr;
jclass doubleCls = nullptr;
jclass booleanCls = nullptr;
jclass errorCls = nullptr;
jclass unsupportedOperationExceptionCls = nullptr;
jmethodID v8ArrayInitMethodID = nullptr;
jmethodID v8TypedArrayInitMethodID = nullptr;
jmethodID v8ArrayBufferInitMethodID = nullptr;
jmethodID v8ArrayGetHandleMethodID = nullptr;
jmethodID v8CallVoidMethodID = nullptr;
jmethodID v8ObjectReleaseMethodID = nullptr;
jmethodID v8DisposeMethodID = nullptr;
jmethodID v8WeakReferenceReleased = nullptr;
jmethodID v8ArrayReleaseMethodID = nullptr;
jmethodID v8ObjectIsUndefinedMethodID = nullptr;
jmethodID v8ObjectGetHandleMethodID = nullptr;
jmethodID throwableGetMessageMethodID = nullptr;
jmethodID integerIntValueMethodID = nullptr;
jmethodID booleanBoolValueMethodID = nullptr;
jmethodID doubleDoubleValueMethodID = nullptr;
jmethodID v8CallObjectJavaMethodMethodID = nullptr;
jmethodID v8ScriptCompilationInitMethodID = nullptr;
jmethodID v8ScriptExecutionExceptionInitMethodID = nullptr;
jmethodID undefinedV8ArrayInitMethodID = nullptr;
jmethodID undefinedV8ObjectInitMethodID = nullptr;
jmethodID integerInitMethodID = nullptr;
jmethodID doubleInitMethodID = nullptr;
jmethodID booleanInitMethodID = nullptr;
jmethodID v8FunctionInitMethodID = nullptr;
jmethodID v8ObjectInitMethodID = nullptr;
jmethodID v8RuntimeExceptionInitMethodID = nullptr;

void throwParseException(JNIEnv *env, const Local<Context>& context, Isolate* isolate, TryCatch* tryCatch);
void throwExecutionException(JNIEnv *env, const Local<Context>& context, Isolate* isolate, TryCatch* tryCatch, jlong v8RuntimePtr);
void throwError(JNIEnv *env, const char *message);
void throwV8RuntimeException(JNIEnv *env,  String::Value *message);
void throwResultUndefinedException(JNIEnv *env, const char *message);
Isolate* getIsolate(JNIEnv *env, jlong handle);
int getType(Handle<Value> v8Value);
jobject getResult(JNIEnv *env, const Local<Context>& context, jobject &v8, jlong v8RuntimePtr, Handle<Value> &result, jint expectedType);

#define SETUP(env, v8RuntimePtr, errorReturnResult) getIsolate(env, v8RuntimePtr);\
    if ( isolate == nullptr ) {\
      return errorReturnResult;\
                                }\
    V8Runtime* runtime = reinterpret_cast<V8Runtime*>(v8RuntimePtr);\
    Isolate::Scope isolateScope(isolate);\
    HandleScope handle_scope(isolate);\
    Local<Context> context = Local<Context>::New(isolate,runtime->context_);\
    Context::Scope context_scope(context);
#define ASSERT_IS_NUMBER(v8Value) \
    if (v8Value.IsEmpty() || v8Value->IsUndefined() || !v8Value->IsNumber()) {\
      throwResultUndefinedException(env, "");\
      return 0;\
                                }
#define ASSERT_IS_STRING(v8Value)\
    if (v8Value.IsEmpty() || v8Value->IsUndefined() || !v8Value->IsString()) {\
      if ( v8Value->IsNull() ) {\
        return 0;\
      }\
      throwResultUndefinedException(env, "");\
      return 0;\
                                }
#define ASSERT_IS_BOOLEAN(v8Value)\
    if (v8Value.IsEmpty() || v8Value->IsUndefined() || !v8Value->IsBoolean() ) {\
      throwResultUndefinedException(env, "");\
      return 0;\
                                }
void release(JNIEnv* env, jobject object) {
  env->CallVoidMethod(object, v8ObjectReleaseMethodID);
}

void releaseArray(JNIEnv* env, jobject object) {
  env->CallVoidMethod(object, v8ArrayReleaseMethodID);
}

int isUndefined(JNIEnv* env, jobject object) {
  return env->CallBooleanMethod(object, v8ObjectIsUndefinedMethodID);
}

jlong getHandle(JNIEnv* env, jobject object) {
  return env->CallLongMethod(object, v8ObjectGetHandleMethodID);
}

JNIEXPORT jstring JNICALL Java_com_eclipsesource_v8_V8__1getVersion (JNIEnv *env, jclass) {
  const char* utfString = v8::V8::GetVersion();
  return env->NewStringUTF(utfString);
}


JNIEXPORT jstring JNICALL Java_com_eclipsesource_v8_V8__1getConstructorName
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0)
  Handle<Object> object = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  String::Value unicodeString(isolate, object->GetConstructorName());
  return env->NewString(*unicodeString, unicodeString.length());
}

Local<String> createV8String(JNIEnv *env, Isolate *isolate, jstring &string) {
  const uint16_t* unicodeString = env->GetStringChars(string, nullptr);
  int length = env->GetStringLength(string);
  MaybeLocal<String> twoByteString = String::NewFromTwoByte(isolate, unicodeString, v8::NewStringType::kNormal, length);
  if (twoByteString.IsEmpty()) {
    return Local<String>();
  }
  Local<String> result = twoByteString.ToLocalChecked();
  env->ReleaseStringChars(string, unicodeString);
  return result;
}

Handle<Value> getValueWithKey(JNIEnv* env, const Local<Context>& context, Isolate* isolate, jlong &v8RuntimePtr, jlong &objectHandle, jstring &key) {
  Handle<Object> object = Local<Object>::New(isolate, *reinterpret_cast<PersistentBase<Object>*>(objectHandle));
  Local<String> v8Key = createV8String(env, isolate, key);
  return object->Get(context, v8Key).ToLocalChecked();
}

void addValueWithKey(JNIEnv* env, const Local<Context> context, Isolate* isolate, jlong &v8RuntimePtr, jlong &objectHandle, jstring &key, Handle<Value> value) {
  Handle<Object> object = Local<Object>::New(isolate, *reinterpret_cast<PersistentBase<Object>*>(objectHandle));
  const uint16_t* unicodeString_key = env->GetStringChars(key, NULL);
  int length = env->GetStringLength(key);
  Local<String> v8Key = String::NewFromTwoByte(isolate, unicodeString_key, v8::NewStringType::kNormal, length).ToLocalChecked();
  object->Set(context, v8Key, value);
  env->ReleaseStringChars(key, unicodeString_key);
}

void getJNIEnv(JNIEnv*& env) {
  int getEnvStat = jvm->GetEnv((void **)&env, JNI_VERSION_1_6);
  if (getEnvStat == JNI_EDETACHED) {
#ifdef __ANDROID_API__
    if (jvm->AttachCurrentThread(&env, NULL) != 0) {
#else
    if (jvm->AttachCurrentThread((void **)&env, nullptr) != 0) {
#endif
      std::cout << "Failed to attach" << std::endl;
    }
  }
  else if (getEnvStat == JNI_OK) {
  }
  else if (getEnvStat == JNI_EVERSION) {
    std::cout << "GetEnv: version not supported" << std::endl;
  }
}

static void jsWindowObjectAccessor(Local<String> property,
  const PropertyCallbackInfo<Value>& info) {
  info.GetReturnValue().Set(info.GetIsolate()->GetCurrentContext()->Global());
}

class ShellArrayBufferAllocator : public v8::ArrayBuffer::Allocator {
 public:
  virtual void* Allocate(size_t length) {
    void* data = AllocateUninitialized(length);
    return data == NULL ? data : memset(data, 0, length);
  }
  virtual void* AllocateUninitialized(size_t length) { return malloc(length); }
  virtual void Free(void* data, size_t) { free(data); }
};

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    jint onLoad_err = -1;
    if ( vm->GetEnv((void **)&env, JNI_VERSION_1_6) != JNI_OK ) {
        return onLoad_err;
    }
    if (env == nullptr) {
        return onLoad_err;
    }

    v8::V8::InitializeICU();
    v8Platform = v8::platform::NewDefaultPlatform();
    v8::V8::InitializePlatform(v8Platform.get());
    v8::V8::Initialize();

    // on first creation, store the JVM and a handle to J2V8 classes
    jvm = vm;
    v8cls = (jclass)env->NewGlobalRef((env)->FindClass("com/eclipsesource/v8/V8"));
    v8ObjectCls = (jclass)env->NewGlobalRef((env)->FindClass("com/eclipsesource/v8/V8Object"));
    v8ArrayCls = (jclass)env->NewGlobalRef((env)->FindClass("com/eclipsesource/v8/V8Array"));
    v8TypedArrayCls = (jclass)env->NewGlobalRef((env)->FindClass("com/eclipsesource/v8/V8TypedArray"));
    v8ArrayBufferCls = (jclass)env->NewGlobalRef((env)->FindClass("com/eclipsesource/v8/V8ArrayBuffer"));
    v8FunctionCls = (jclass)env->NewGlobalRef((env)->FindClass("com/eclipsesource/v8/V8Function"));
    undefinedV8ObjectCls = (jclass)env->NewGlobalRef((env)->FindClass("com/eclipsesource/v8/V8Object$Undefined"));
    undefinedV8ArrayCls = (jclass)env->NewGlobalRef((env)->FindClass("com/eclipsesource/v8/V8Array$Undefined"));
    stringCls = (jclass)env->NewGlobalRef((env)->FindClass("java/lang/String"));
    integerCls = (jclass)env->NewGlobalRef((env)->FindClass("java/lang/Integer"));
    doubleCls = (jclass)env->NewGlobalRef((env)->FindClass("java/lang/Double"));
    booleanCls = (jclass)env->NewGlobalRef((env)->FindClass("java/lang/Boolean"));
    throwableCls = (jclass)env->NewGlobalRef((env)->FindClass("java/lang/Throwable"));
    v8ResultsUndefinedCls = (jclass)env->NewGlobalRef((env)->FindClass("com/eclipsesource/v8/V8ResultUndefined"));
    v8ScriptCompilationCls = (jclass)env->NewGlobalRef((env)->FindClass("com/eclipsesource/v8/V8ScriptCompilationException"));
    v8ScriptExecutionException = (jclass)env->NewGlobalRef((env)->FindClass("com/eclipsesource/v8/V8ScriptExecutionException"));
    v8RuntimeExceptionCls = (jclass)env->NewGlobalRef((env)->FindClass("com/eclipsesource/v8/V8RuntimeException"));
    errorCls = (jclass)env->NewGlobalRef((env)->FindClass("java/lang/Error"));
    unsupportedOperationExceptionCls = (jclass)env->NewGlobalRef((env)->FindClass("java/lang/UnsupportedOperationException"));

    // Get all method IDs
    v8ArrayInitMethodID = env->GetMethodID(v8ArrayCls, "<init>", "(Lcom/eclipsesource/v8/V8;)V");
	  v8TypedArrayInitMethodID = env->GetMethodID(v8TypedArrayCls, "<init>", "(Lcom/eclipsesource/v8/V8;)V");
    v8ArrayBufferInitMethodID = env->GetMethodID(v8ArrayBufferCls, "<init>", "(Lcom/eclipsesource/v8/V8;Ljava/nio/ByteBuffer;)V");
    v8ArrayGetHandleMethodID = env->GetMethodID(v8ArrayCls, "getHandle", "()J");
    v8CallVoidMethodID = (env)->GetMethodID(v8cls, "callVoidJavaMethod", "(JLcom/eclipsesource/v8/V8Object;Lcom/eclipsesource/v8/V8Array;)V");
    v8ObjectReleaseMethodID = env->GetMethodID(v8ObjectCls, "release", "()V");
    v8ArrayReleaseMethodID = env->GetMethodID(v8ArrayCls, "release", "()V");
    v8ObjectIsUndefinedMethodID = env->GetMethodID(v8ObjectCls, "isUndefined", "()Z");
    v8ObjectGetHandleMethodID = env->GetMethodID(v8ObjectCls, "getHandle", "()J");
    throwableGetMessageMethodID = env->GetMethodID(throwableCls, "getMessage", "()Ljava/lang/String;");
    integerIntValueMethodID = env->GetMethodID(integerCls, "intValue", "()I");
    booleanBoolValueMethodID = env->GetMethodID(booleanCls, "booleanValue", "()Z");
    doubleDoubleValueMethodID = env->GetMethodID(doubleCls, "doubleValue", "()D");
    v8CallObjectJavaMethodMethodID = (env)->GetMethodID(v8cls, "callObjectJavaMethod", "(JLcom/eclipsesource/v8/V8Object;Lcom/eclipsesource/v8/V8Array;)Ljava/lang/Object;");
    v8DisposeMethodID = (env)->GetMethodID(v8cls, "disposeMethodID", "(J)V");
    v8WeakReferenceReleased = (env)->GetMethodID(v8cls, "weakReferenceReleased", "(J)V");
    v8ScriptCompilationInitMethodID = env->GetMethodID(v8ScriptCompilationCls, "<init>", "(Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;II)V");
    v8ScriptExecutionExceptionInitMethodID = env->GetMethodID(v8ScriptExecutionException, "<init>", "(Ljava/lang/String;ILjava/lang/String;Ljava/lang/String;IILjava/lang/String;Ljava/lang/Throwable;)V");
    undefinedV8ArrayInitMethodID = env->GetMethodID(undefinedV8ArrayCls, "<init>", "()V");
    undefinedV8ObjectInitMethodID = env->GetMethodID(undefinedV8ObjectCls, "<init>", "()V");
    v8RuntimeExceptionInitMethodID = env->GetMethodID(v8RuntimeExceptionCls, "<init>", "(Ljava/lang/String;)V");
    integerInitMethodID = env->GetMethodID(integerCls, "<init>", "(I)V");
    doubleInitMethodID = env->GetMethodID(doubleCls, "<init>", "(D)V");
    booleanInitMethodID = env->GetMethodID(booleanCls, "<init>", "(Z)V");
    v8FunctionInitMethodID = env->GetMethodID(v8FunctionCls, "<init>", "(Lcom/eclipsesource/v8/V8;)V");
    v8ObjectInitMethodID = env->GetMethodID(v8ObjectCls, "<init>", "(Lcom/eclipsesource/v8/V8;)V");

    return JNI_VERSION_1_6;
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1setFlags
 (JNIEnv *env, jclass, jstring v8flags) {
    if (v8flags) {
        char const* str = env->GetStringUTFChars(v8flags, nullptr);
        v8::V8::SetFlagsFromString(str, env->GetStringUTFLength(v8flags));
        env->ReleaseStringUTFChars(v8flags, str);
        v8::V8::Initialize();
    }
}

ShellArrayBufferAllocator array_buffer_allocator;

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1startNodeJS
  (JNIEnv * env, jclass, jlong v8RuntimePtr, jstring fileName) {
    (env)->ThrowNew(unsupportedOperationExceptionCls, "startNodeJS Not Supported.");
}

JNIEXPORT jboolean JNICALL Java_com_eclipsesource_v8_V8__1pumpMessageLoop
  (JNIEnv * env, jclass, jlong v8RuntimePtr) {
    (env)->ThrowNew(unsupportedOperationExceptionCls, "pumpMessageLoop Not Supported.");
 return false;
}

JNIEXPORT jboolean JNICALL Java_com_eclipsesource_v8_V8__1isRunning
  (JNIEnv *env, jclass, jlong v8RuntimePtr) {
 (env)->ThrowNew(unsupportedOperationExceptionCls, "isRunning Not Supported.");
 return false;
}

JNIEXPORT jboolean JNICALL Java_com_eclipsesource_v8_V8__1isNodeCompatible
  (JNIEnv * env, jclass) {
    (env)->ThrowNew(unsupportedOperationExceptionCls, "isNodeCompatible Not Supported.");
   return false;
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1createIsolate
 (JNIEnv *env, jobject v8, jstring globalAlias) {
    V8Runtime* runtime = new V8Runtime();
    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = &array_buffer_allocator;
    runtime->isolate = v8::Isolate::New(create_params);
    runtime->locker = new Locker(runtime->isolate);
    v8::Isolate::Scope isolate_scope(runtime->isolate);
    runtime->v8 = env->NewGlobalRef(v8);
    runtime->pendingException = nullptr;
    HandleScope handle_scope(runtime->isolate);
    Handle<ObjectTemplate> globalObject = ObjectTemplate::New(runtime->isolate);
    if (globalAlias == nullptr) {
      Handle<Context> context = Context::New(runtime->isolate, nullptr, globalObject);
      runtime->context_.Reset(runtime->isolate, context);
      runtime->globalObject = new Persistent<Object>;
      runtime->globalObject->Reset(runtime->isolate, context->Global()->GetPrototype()->ToObject(context).ToLocalChecked());
    }
    else {
      Local<String> utfAlias = createV8String(env, runtime->isolate, globalAlias);
      globalObject->SetAccessor(utfAlias, jsWindowObjectAccessor);
      Handle<Context> context = Context::New(runtime->isolate, nullptr, globalObject);
      runtime->context_.Reset(runtime->isolate, context);
      runtime->globalObject = new Persistent<Object>;
      runtime->globalObject->Reset(runtime->isolate, context->Global()->GetPrototype()->ToObject(context).ToLocalChecked());
    }
    delete(runtime->locker);
    return reinterpret_cast<jlong>(runtime);
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1acquireLock
  (JNIEnv *env, jobject, jlong v8RuntimePtr) {
  V8Runtime* runtime = reinterpret_cast<V8Runtime*>(v8RuntimePtr);
  if(runtime->isolate->InContext()) {
    jstring exceptionString = env->NewStringUTF("Cannot acquire lock while in a V8 Context");
    jthrowable exception = static_cast<jthrowable>(env->NewObject(v8RuntimeExceptionCls, v8RuntimeExceptionInitMethodID, exceptionString));
    (env)->Throw(exception);
    env->DeleteLocalRef(exceptionString);
    return;
  }
  runtime->locker = new Locker(runtime->isolate);
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1releaseLock
  (JNIEnv *env, jobject, jlong v8RuntimePtr) {
  V8Runtime* runtime = reinterpret_cast<V8Runtime*>(v8RuntimePtr);
    if(runtime->isolate->InContext()) {
    jstring exceptionString = env->NewStringUTF("Cannot release lock while in a V8 Context");
    jthrowable exception = static_cast<jthrowable>(env->NewObject(v8RuntimeExceptionCls, v8RuntimeExceptionInitMethodID, exceptionString));
    (env)->Throw(exception);
    env->DeleteLocalRef(exceptionString);
    return;
  }
  delete(runtime->locker);
  runtime->locker = nullptr;
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1lowMemoryNotification
  (JNIEnv *env, jobject, jlong v8RuntimePtr) {
  V8Runtime* runtime = reinterpret_cast<V8Runtime*>(v8RuntimePtr);
  runtime->isolate->LowMemoryNotification();
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initEmptyContainer
(JNIEnv *env, jobject, jlong v8RuntimePtr) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0)
  Persistent<Object>* container = new Persistent<Object>;
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8Object
(JNIEnv *env, jobject, jlong v8RuntimePtr) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0)
  Local<Object> obj = Object::New(isolate);
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, obj);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1getGlobalObject
  (JNIEnv *env, jobject, jlong v8RuntimePtr) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0)
  Local<Object> obj = Object::New(isolate);
  return reinterpret_cast<jlong>(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->globalObject);
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1createTwin
  (JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jlong twinObjectHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, )
  Handle<Object> obj = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  reinterpret_cast<Persistent<Object>*>(twinObjectHandle)->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, obj);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8Array
(JNIEnv *env, jobject, jlong v8RuntimePtr) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0)
  Local<Array> array = Array::New(isolate);
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, array);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8Int8Array
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong bufferHandle, jint offset, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0)
  Handle<ArrayBuffer> arrayBuffer = Local<ArrayBuffer>::New(isolate, *reinterpret_cast<Persistent<ArrayBuffer>*>(bufferHandle));
  Local<Int8Array> array = Int8Array::New(arrayBuffer, static_cast<size_t>(offset), static_cast<size_t>(length));
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, array);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8UInt8Array
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong bufferHandle, jint offset, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0)
  Handle<ArrayBuffer> arrayBuffer = Local<ArrayBuffer>::New(isolate, *reinterpret_cast<Persistent<ArrayBuffer>*>(bufferHandle));
  Local<Uint8Array> array = Uint8Array::New(arrayBuffer, static_cast<size_t>(offset), static_cast<size_t>(length));
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, array);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8UInt8ClampedArray
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong bufferHandle, jint offset, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0)
  Handle<ArrayBuffer> arrayBuffer = Local<ArrayBuffer>::New(isolate, *reinterpret_cast<Persistent<ArrayBuffer>*>(bufferHandle));
  Local<Uint8ClampedArray> array = Uint8ClampedArray::New(arrayBuffer, static_cast<size_t>(offset), static_cast<size_t>(length));
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, array);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8Int32Array
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong bufferHandle, jint offset, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0)
  Handle<ArrayBuffer> arrayBuffer = Local<ArrayBuffer>::New(isolate, *reinterpret_cast<Persistent<ArrayBuffer>*>(bufferHandle));
  Local<Int32Array> array = Int32Array::New(arrayBuffer, static_cast<size_t>(offset), static_cast<size_t>(length));
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, array);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8UInt32Array
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong bufferHandle, jint offset, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0)
  Handle<ArrayBuffer> arrayBuffer = Local<ArrayBuffer>::New(isolate, *reinterpret_cast<Persistent<ArrayBuffer>*>(bufferHandle));
  Local<Uint32Array> array = Uint32Array::New(arrayBuffer, static_cast<size_t>(offset), static_cast<size_t>(length));
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, array);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8UInt16Array
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong bufferHandle, jint offset, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0)
  Handle<ArrayBuffer> arrayBuffer = Local<ArrayBuffer>::New(isolate, *reinterpret_cast<Persistent<ArrayBuffer>*>(bufferHandle));
  Local<Uint16Array> array = Uint16Array::New(arrayBuffer, static_cast<size_t>(offset), static_cast<size_t>(length));
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, array);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8Int16Array
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong bufferHandle, jint offset, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0)
  Handle<ArrayBuffer> arrayBuffer = Local<ArrayBuffer>::New(isolate, *reinterpret_cast<Persistent<ArrayBuffer>*>(bufferHandle));
  Local<Int16Array> array = Int16Array::New(arrayBuffer, static_cast<size_t>(offset), static_cast<size_t>(length));
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, array);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8Float32Array
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong bufferHandle, jint offset, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0)
  Handle<ArrayBuffer> arrayBuffer = Local<ArrayBuffer>::New(isolate, *reinterpret_cast<Persistent<ArrayBuffer>*>(bufferHandle));
  Local<Float32Array> array = Float32Array::New(arrayBuffer, static_cast<size_t>(offset), static_cast<size_t>(length));
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, array);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8Float64Array
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong bufferHandle, jint offset, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0)
  Handle<ArrayBuffer> arrayBuffer = Local<ArrayBuffer>::New(isolate, *reinterpret_cast<Persistent<ArrayBuffer>*>(bufferHandle));
  Local<Float64Array> array = Float64Array::New(arrayBuffer, static_cast<size_t>(offset), static_cast<size_t>(length));
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, array);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8ArrayBuffer__JI
(JNIEnv *env, jobject, jlong v8RuntimePtr, jint capacity) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0)
  Local<ArrayBuffer> arrayBuffer = ArrayBuffer::New(isolate, static_cast<size_t>(capacity));
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, arrayBuffer);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8ArrayBuffer__JLjava_nio_ByteBuffer_2I
(JNIEnv *env, jobject, jlong v8RuntimePtr, jobject byteBuffer, jint capacity) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0)
  Local<ArrayBuffer> arrayBuffer = ArrayBuffer::New(isolate, env->GetDirectBufferAddress(byteBuffer), static_cast<size_t>(capacity));
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, arrayBuffer);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jobject JNICALL Java_com_eclipsesource_v8_V8__1createV8ArrayBufferBackingStore
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jint capacity) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, nullptr)
  Handle<ArrayBuffer> arrayBuffer = Local<ArrayBuffer>::New(isolate, *reinterpret_cast<Persistent<ArrayBuffer>*>(objectHandle));
  void* dataPtr = arrayBuffer->GetContents().Data();
  jobject byteBuffer = env->NewDirectByteBuffer(dataPtr, capacity);
  return byteBuffer;
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1release
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle) {
  if (v8RuntimePtr == 0) {
    return;
  }
  Isolate* isolate = getIsolate(env, v8RuntimePtr);
  Locker locker(isolate);
  HandleScope handle_scope(isolate);
  reinterpret_cast<Persistent<Object>*>(objectHandle)->Reset();
  delete(reinterpret_cast<Persistent<Object>*>(objectHandle));
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1terminateExecution
  (JNIEnv * env, jobject, jlong v8RuntimePtr) {
	if (v8RuntimePtr == 0) {
	  return;
	}
	Isolate* isolate = getIsolate(env, v8RuntimePtr);
	isolate->TerminateExecution();
	return;
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1releaseRuntime
(JNIEnv *env, jobject, jlong v8RuntimePtr) {
  if (v8RuntimePtr == 0) {
    return;
  }
  Isolate* isolate = getIsolate(env, v8RuntimePtr);
  reinterpret_cast<V8Runtime*>(v8RuntimePtr)->context_.Reset();
  reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate->Dispose();
  env->DeleteGlobalRef(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->v8);
  V8Runtime* runtime = reinterpret_cast<V8Runtime*>(v8RuntimePtr);
  delete(runtime);
}

JNIEXPORT jboolean JNICALL Java_com_eclipsesource_v8_V8__1contains
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, false);
  Handle<Object> object = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  Local<String> v8Key = createV8String(env, isolate, key);
  return object->Has(context, v8Key).FromMaybe(false);
}

JNIEXPORT jobjectArray JNICALL Java_com_eclipsesource_v8_V8__1getKeys
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL);
  Handle<Object> object = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  MaybeLocal<Array> properties = object->GetOwnPropertyNames(context);
  if (!properties.IsEmpty()) {
    int size = properties.ToLocalChecked()->Length();
    jobjectArray keys = (env)->NewObjectArray(size, stringCls, nullptr);
    for (int i = 0; i < size; i++) {
      MaybeLocal<Value> property = properties.ToLocalChecked()->Get(context, i);
      if (property.IsEmpty()) {
        continue;
      }
      String::Value unicodeString(isolate, property.ToLocalChecked());
      jobject key = (env)->NewString(*unicodeString, unicodeString.length());
      (env)->SetObjectArrayElement(keys, i, key);
      (env)->DeleteLocalRef(key);
    }
    return keys;
  }
  return (env)->NewObjectArray(0, stringCls, nullptr);
}

ScriptOrigin* createScriptOrigin(JNIEnv * env, Isolate* isolate, jstring &jscriptName, jint jlineNumber = 0) {
  Local<String> scriptName = createV8String(env, isolate, jscriptName);
  return new ScriptOrigin(scriptName, Integer::New(isolate, jlineNumber));
}

bool compileScript(const Local<Context>& context, Isolate *isolate, jstring &jscript, JNIEnv *env, jstring jscriptName, jint &jlineNumber, Local<Script> &script, TryCatch* tryCatch) {
  Local<String> source = createV8String(env, isolate, jscript);
  ScriptOrigin* scriptOriginPtr = nullptr;
  if (jscriptName != nullptr) {
    scriptOriginPtr = createScriptOrigin(env, isolate, jscriptName, jlineNumber);
  }
  MaybeLocal<Script> script_result = Script::Compile(context, source, scriptOriginPtr);
  if (!script_result.IsEmpty()) {
      script = script_result.ToLocalChecked();
      if (scriptOriginPtr != nullptr) {
        delete(scriptOriginPtr);
      }
  }
  if (tryCatch->HasCaught()) {
    throwParseException(env, context, isolate, tryCatch);
    return false;
  }
  return true;
}

bool runScript(const Local<Context>& context, Isolate* isolate, JNIEnv *env, Local<Script> *script, TryCatch* tryCatch, jlong v8RuntimePtr) {
  (*script)->Run(context);
  if (tryCatch->HasCaught()) {
    throwExecutionException(env, context, isolate, tryCatch, v8RuntimePtr);
    return false;
  }
  return true;
}

bool runScript(const Local<Context>& context, Isolate* isolate, JNIEnv *env, Local<Script> *script, TryCatch* tryCatch, Local<Value> &result, jlong v8RuntimePtr) {
  MaybeLocal<Value> local_result = (*script)->Run(context);
  if (!local_result.IsEmpty()) {
    result = local_result.ToLocalChecked();
    return true;
  }
  if (tryCatch->HasCaught()) {
    throwExecutionException(env, context, isolate, tryCatch, v8RuntimePtr);
    return false;
  }
  return true;
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1executeVoidScript
(JNIEnv * env, jobject v8, jlong v8RuntimePtr, jstring jjstring, jstring jscriptName = nullptr, jint jlineNumber = 0) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, )
  TryCatch tryCatch(isolate);
  Local<Script> script;
  if (!compileScript(context, isolate, jjstring, env, jscriptName, jlineNumber, script, &tryCatch))
    return;
  runScript(context, isolate, env, &script, &tryCatch, v8RuntimePtr);
}

JNIEXPORT jdouble JNICALL Java_com_eclipsesource_v8_V8__1executeDoubleScript
(JNIEnv * env, jobject v8, jlong v8RuntimePtr, jstring jjstring, jstring jscriptName = nullptr, jint jlineNumber = 0) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0)
  TryCatch tryCatch(isolate);
  Local<Script> script;
  Local<Value> result;
  if (!compileScript(context, isolate, jjstring, env, jscriptName, jlineNumber, script, &tryCatch))
    return 0;
  if (!runScript(context, isolate, env, &script, &tryCatch, result, v8RuntimePtr))
    return 0;
  ASSERT_IS_NUMBER(result);
  return result->NumberValue(context).FromMaybe(0);
}

JNIEXPORT jboolean JNICALL Java_com_eclipsesource_v8_V8__1executeBooleanScript
(JNIEnv *env, jobject v8, jlong v8RuntimePtr, jstring jjstring, jstring jscriptName = nullptr, jint jlineNumber = 0) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, false)
  TryCatch tryCatch(isolate);
  Local<Script> script;
  Local<Value> result;
  if (!compileScript(context, isolate, jjstring, env, jscriptName, jlineNumber, script, &tryCatch))
    return false;
  if (!runScript(context, isolate, env, &script, &tryCatch, result, v8RuntimePtr))
    return false;
  ASSERT_IS_BOOLEAN(result);
  return result->BooleanValue(isolate);
}

JNIEXPORT jstring JNICALL Java_com_eclipsesource_v8_V8__1executeStringScript
(JNIEnv *env, jobject v8, jlong v8RuntimePtr, jstring jjstring, jstring jscriptName = nullptr, jint jlineNumber = 0) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL)
  TryCatch tryCatch(isolate);
  Local<Script> script;
  Local<Value> result;
  if (!compileScript(context, isolate, jjstring, env, jscriptName, jlineNumber, script, &tryCatch))
    return nullptr;
  if (!runScript(context, isolate, env, &script, &tryCatch, result, v8RuntimePtr))
    return nullptr;
  ASSERT_IS_STRING(result)
  String::Value unicodeString(isolate, result);

  return env->NewString(*unicodeString, unicodeString.length());
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1executeIntegerScript
(JNIEnv * env, jobject v8, jlong v8RuntimePtr, jstring jjstring, jstring jscriptName = nullptr, jint jlineNumber = 0) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0)
  TryCatch tryCatch(isolate);
  Local<Script> script;
  Local<Value> result;
  if (!compileScript(context, isolate, jjstring, env, jscriptName, jlineNumber, script, &tryCatch))
    return 0;
  if (!runScript(context, isolate, env, &script, &tryCatch, result, v8RuntimePtr))
    return 0;
  ASSERT_IS_NUMBER(result);
  return result->Int32Value(context).FromJust();
}

JNIEXPORT jobject JNICALL Java_com_eclipsesource_v8_V8__1executeScript
(JNIEnv *env, jobject v8, jlong v8RuntimePtr, jint expectedType, jstring jjstring, jstring jscriptName = nullptr, jint jlineNumber = 0) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL)
  TryCatch tryCatch(isolate);
  Local<Script> script;
  Local<Value> result;
  if (!compileScript(context, isolate, jjstring, env, jscriptName, jlineNumber, script, &tryCatch)) { return nullptr; }
  if (!runScript(context, isolate, env, &script, &tryCatch, result, v8RuntimePtr)) { return nullptr; }
  return getResult(env, context, v8, v8RuntimePtr, result, expectedType);
}

bool invokeFunction(JNIEnv *env, const Local<Context>& context, Isolate* isolate, jlong &v8RuntimePtr, jlong &receiverHandle, jlong &functionHandle, jlong &parameterHandle, Handle<Value> &result) {
  int size = 0;
  Handle<Value>* args = nullptr;
  if (parameterHandle != 0) {
    Handle<Object> parameters = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(parameterHandle));
    size = Array::Cast(*parameters)->Length();
    args = new Handle<Value>[size];
    for (int i = 0; i < size; i++) {
      args[i] = parameters->Get(context, i).ToLocalChecked();
    }
  }
  Handle<Object> object = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(functionHandle));
  Handle<Object> receiver = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(receiverHandle));
  Handle<Function> func = Handle<Function>::Cast(object);
  TryCatch tryCatch(isolate);
  result = func->Call(context, receiver, size, args).ToLocalChecked();
  if (args != nullptr) {
    delete(args);
  }
  if (tryCatch.HasCaught()) {
    throwExecutionException(env, context, isolate, &tryCatch, v8RuntimePtr);
    return false;
  }
  return true;
}

bool invokeFunction(JNIEnv *env, const Local<Context>& context, Isolate* isolate, jlong &v8RuntimePtr, jlong &objectHandle, jstring &jfunctionName, jlong &parameterHandle, Handle<Value> &result) {
  Local<String> functionName = createV8String(env, isolate, jfunctionName);
  Handle<Object> parentObject = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  int size = 0;
  Handle<Value>* args = nullptr;
  if (parameterHandle != 0) {
    Handle<Object> parameters = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(parameterHandle));
    size = Array::Cast(*parameters)->Length();
    args = new Handle<Value>[size];
    for (int i = 0; i < size; i++) {
      args[i] = parameters->Get(context, i).ToLocalChecked();
    }
  }
  TryCatch tryCatch(isolate);
  MaybeLocal<Value> result_value = parentObject->Get(context, functionName);
  if (!result_value.IsEmpty()) {
      Handle<Value> value = result_value.ToLocalChecked();
      Handle<Function> func = Handle<Function>::Cast(value);
      MaybeLocal<Value> function_call_result = func->Call(context, parentObject, size, args);
      if (!function_call_result.IsEmpty()) {
        result = function_call_result.ToLocalChecked();
      }
  }
  if (args != nullptr) {
    delete(args);
  }
  if (tryCatch.HasCaught()) {
    throwExecutionException(env, context, isolate, &tryCatch, v8RuntimePtr);
    return false;
  }
  return true;
}

JNIEXPORT jobject JNICALL Java_com_eclipsesource_v8_V8__1executeFunction__JJJJ
  (JNIEnv *env, jobject v8, jlong v8RuntimePtr, jlong receiverHandle, jlong functionHandle, jlong parameterHandle) {
    Isolate* isolate = SETUP(env, v8RuntimePtr, NULL)
    Handle<Value> result;
    if (!invokeFunction(env, context, isolate, v8RuntimePtr, receiverHandle, functionHandle, parameterHandle, result))
        return nullptr;
    return getResult(env, context, v8, v8RuntimePtr, result, com_eclipsesource_v8_V8_UNKNOWN);
}

JNIEXPORT jobject JNICALL Java_com_eclipsesource_v8_V8__1executeFunction__JIJLjava_lang_String_2J
(JNIEnv *env, jobject v8, jlong v8RuntimePtr, jint expectedType, jlong objectHandle, jstring jfunctionName, jlong parameterHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL)
  Handle<Value> result;
  if (!invokeFunction(env, context, isolate, v8RuntimePtr, objectHandle, jfunctionName, parameterHandle, result))
    return nullptr;
  return getResult(env, context, v8, v8RuntimePtr, result, expectedType);
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1executeIntegerFunction
(JNIEnv *env, jobject v8, jlong v8RuntimePtr, jlong objectHandle, jstring jfunctionName, jlong parameterHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0)
  Handle<Value> result;
  if (!invokeFunction(env, context, isolate, v8RuntimePtr, objectHandle, jfunctionName, parameterHandle, result))
    return 0;
  ASSERT_IS_NUMBER(result);
  return result->Int32Value(context).FromJust();
}

JNIEXPORT jdouble JNICALL Java_com_eclipsesource_v8_V8__1executeDoubleFunction
(JNIEnv *env, jobject v8, jlong v8RuntimePtr, jlong objectHandle, jstring jfunctionName, jlong parameterHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Value> result;
  if (!invokeFunction(env, context, isolate, v8RuntimePtr, objectHandle, jfunctionName, parameterHandle, result))
    return 0;
  ASSERT_IS_NUMBER(result);
  return result->NumberValue(context).FromJust();
}

JNIEXPORT jboolean JNICALL Java_com_eclipsesource_v8_V8__1executeBooleanFunction
(JNIEnv *env, jobject v8, jlong v8RuntimePtr, jlong objectHandle, jstring jfunctionName, jlong parameterHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, false);
  Handle<Value> result;
  if (!invokeFunction(env, context, isolate, v8RuntimePtr, objectHandle, jfunctionName, parameterHandle, result))
    return false;
  ASSERT_IS_BOOLEAN(result);
  return result->BooleanValue(isolate);
}

JNIEXPORT jstring JNICALL Java_com_eclipsesource_v8_V8__1executeStringFunction
(JNIEnv *env, jobject v8, jlong v8RuntimePtr, jlong objectHandle, jstring jfunctionName, jlong parameterHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL)
  Handle<Value> result;
  if (!invokeFunction(env, context, isolate, v8RuntimePtr, objectHandle, jfunctionName, parameterHandle, result))
    return nullptr;
  ASSERT_IS_STRING(result)
  String::Value unicodeString(isolate, result->ToString(context).ToLocalChecked());

  return env->NewString(*unicodeString, unicodeString.length());
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1executeVoidFunction
(JNIEnv *env, jobject v8, jlong v8RuntimePtr, jlong objectHandle, jstring jfunctionName, jlong parameterHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  Handle<Value> result;
  invokeFunction(env, context, isolate, v8RuntimePtr, objectHandle, jfunctionName, parameterHandle, result);
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1addUndefined
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  addValueWithKey(env, context, isolate, v8RuntimePtr, objectHandle, key, Undefined(isolate));
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1addNull
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  addValueWithKey(env, context, isolate, v8RuntimePtr, objectHandle, key, Null(isolate));
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1add__JJLjava_lang_String_2I
(JNIEnv * env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key, jint value) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  addValueWithKey(env, context, isolate, v8RuntimePtr, objectHandle, key, Int32::New(isolate, value));
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1add__JJLjava_lang_String_2D
(JNIEnv * env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key, jdouble value) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  addValueWithKey(env, context, isolate, v8RuntimePtr, objectHandle, key, Number::New(isolate, value));
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1add__JJLjava_lang_String_2Ljava_lang_String_2
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key, jstring value) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  Handle<Value> v8Value = createV8String(env, isolate, value);
  addValueWithKey(env, context, isolate, v8RuntimePtr, objectHandle, key, v8Value);
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1add__JJLjava_lang_String_2Z
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key, jboolean value) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  addValueWithKey(env, context, isolate, v8RuntimePtr, objectHandle, key, Boolean::New(isolate, value));
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1addObject
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key, jlong valueHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  Handle<Value> value = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(valueHandle));
  addValueWithKey(env, context, isolate, v8RuntimePtr, objectHandle, key, value);
}

JNIEXPORT jobject JNICALL Java_com_eclipsesource_v8_V8__1get
(JNIEnv *env, jobject v8, jlong v8RuntimePtr, jint expectedType, jlong objectHandle, jstring key) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, nullptr)
  Handle<Value> result = getValueWithKey(env, context, isolate, v8RuntimePtr, objectHandle, key);
  return getResult(env, context, v8, v8RuntimePtr, result, expectedType);
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1getInteger
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Value> v8Value = getValueWithKey(env, context, isolate, v8RuntimePtr, objectHandle, key);
  ASSERT_IS_NUMBER(v8Value);
  return v8Value->Int32Value(context).FromJust();
}

JNIEXPORT jdouble JNICALL Java_com_eclipsesource_v8_V8__1getDouble
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Value> v8Value = getValueWithKey(env, context, isolate, v8RuntimePtr, objectHandle, key);
  ASSERT_IS_NUMBER(v8Value);
  return v8Value->NumberValue(context).FromJust();
}

JNIEXPORT jstring JNICALL Java_com_eclipsesource_v8_V8__1getString
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Value> v8Value = getValueWithKey(env, context, isolate, v8RuntimePtr, objectHandle, key);
  ASSERT_IS_STRING(v8Value);
  String::Value unicode(isolate, v8Value);

  return env->NewString(*unicode, unicode.length());
}

JNIEXPORT jboolean JNICALL Java_com_eclipsesource_v8_V8__1getBoolean
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, false);
  Handle<Value> v8Value = getValueWithKey(env, context, isolate, v8RuntimePtr, objectHandle, key);
  ASSERT_IS_BOOLEAN(v8Value);
  return v8Value->BooleanValue(isolate);
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1getType__JJLjava_lang_String_2
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Value> v8Value = getValueWithKey(env, context, isolate, v8RuntimePtr, objectHandle, key);
  int type = getType(v8Value);
  if (type < 0) {
    throwResultUndefinedException(env, "");
  }
  return type;
}

bool isNumber(int type) {
  return type == com_eclipsesource_v8_V8_DOUBLE || type == com_eclipsesource_v8_V8_INTEGER;
}

bool isObject(int type) {
  return type == com_eclipsesource_v8_V8_V8_OBJECT || type == com_eclipsesource_v8_V8_V8_ARRAY;
}

bool isNumber(int type1, int type2) {
  return isNumber(type1) && isNumber(type2);
}

bool isObject(int type1, int type2) {
  return isObject(type1) && isObject(type2);
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1getArrayType
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  int length = 0;
  if ( array->IsTypedArray() ) {
      if ( array->IsFloat64Array() ) {
        return com_eclipsesource_v8_V8_DOUBLE;
      } else if ( array->IsFloat32Array() ) {
        return com_eclipsesource_v8_V8_FLOAT_32_ARRAY;
      } else if ( array->IsInt32Array() ) {
        return com_eclipsesource_v8_V8_INT_32_ARRAY;
      } else if ( array->IsUint32Array() ) {
        return com_eclipsesource_v8_V8_UNSIGNED_INT_32_ARRAY;
      } else if ( array->IsInt16Array() ) {
        return com_eclipsesource_v8_V8_INT_16_ARRAY;
      } else if ( array->IsUint16Array() ) {
        return com_eclipsesource_v8_V8_UNSIGNED_INT_16_ARRAY;
      } else if ( array->IsInt8Array() ) {
        return com_eclipsesource_v8_V8_INT_8_ARRAY;
      } else if ( array->IsUint8Array() ) {
        return com_eclipsesource_v8_V8_UNSIGNED_INT_8_ARRAY;
      } else if ( array->IsUint8ClampedArray() ) {
        return com_eclipsesource_v8_V8_UNSIGNED_INT_8_CLAMPED_ARRAY;
      }
      return com_eclipsesource_v8_V8_INTEGER;
  } else {
      length = Array::Cast(*array)->Length();
  }
  int arrayType = com_eclipsesource_v8_V8_UNDEFINED;
  for (int index = 0; index < length; index++) {
    int type = getType(array->Get(context, index).ToLocalChecked());
    if (type < 0) {
      throwResultUndefinedException(env, "");
    }
    else if (index == 0) {
      arrayType = type;
    }
    else if (type == arrayType) {
      // continue
    }
    else if (isNumber(arrayType, type)) {
      arrayType = com_eclipsesource_v8_V8_DOUBLE;
    }
    else if (isObject(arrayType, type)) {
      arrayType = com_eclipsesource_v8_V8_V8_OBJECT;
    }
    else {
      return com_eclipsesource_v8_V8_UNDEFINED;
    }
  }
  return arrayType;
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1arrayGetSize
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  if ( array->IsTypedArray() ) {
	  return TypedArray::Cast(*array)->Length();
  }
  return Array::Cast(*array)->Length();
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1arrayGetInteger
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint index) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  Handle<Value> v8Value = array->Get(context, index).ToLocalChecked();
  ASSERT_IS_NUMBER(v8Value);
  return v8Value->Int32Value(context).FromJust();
}

int fillIntArray(JNIEnv *env, const Local<Context> context, Handle<Object> &array, int start, int length, jintArray &result) {
  jint * fill = new jint[length];
  for (int i = start; i < start + length; i++) {
    Handle<Value> v8Value = array->Get(context, i).ToLocalChecked();
    ASSERT_IS_NUMBER(v8Value);
    fill[i - start] = v8Value->Int32Value(context).FromJust();
  }
  (env)->SetIntArrayRegion(result, 0, length, fill);
  delete[] fill;
  return length;
}

int fillDoubleArray(JNIEnv *env, const Local<Context> context, Handle<Object> &array, int start, int length, jdoubleArray &result) {
  jdouble * fill = new jdouble[length];
  for (int i = start; i < start + length; i++) {
    Handle<Value> v8Value = array->Get(context, i).ToLocalChecked();
    ASSERT_IS_NUMBER(v8Value);
    fill[i - start] = v8Value->NumberValue(context).FromJust();
  }
  (env)->SetDoubleArrayRegion(result, 0, length, fill);
  delete[] fill;
  return length;
}

int fillByteArray(JNIEnv *env, const Local<Context> context, Handle<Object> &array, int start, int length, jbyteArray &result) {
  jbyte * fill = new jbyte[length];
  for (int i = start; i < start + length; i++) {
    Handle<Value> v8Value = array->Get(context, i).ToLocalChecked();
    ASSERT_IS_NUMBER(v8Value);
    fill[i - start] = (jbyte)v8Value->Int32Value(context).FromJust();
  }
  (env)->SetByteArrayRegion(result, 0, length, fill);
  delete[] fill;
  return length;
}

int fillBooleanArray(JNIEnv *env, const Local<Context> context, Isolate* isolate, Handle<Object> &array, int start, int length, jbooleanArray &result) {
  jboolean * fill = new jboolean[length];
  for (int i = start; i < start + length; i++) {
    Handle<Value> v8Value = array->Get(context, i).ToLocalChecked();
    ASSERT_IS_BOOLEAN(v8Value);
    fill[i - start] = v8Value->BooleanValue(isolate);
  }
  (env)->SetBooleanArrayRegion(result, 0, length, fill);
  delete[] fill;
  return length;
}

int fillStringArray(JNIEnv *env, const Local<Context> context, Isolate* isolate, Handle<Object> &array, int start, int length, jobjectArray &result) {
  for (int i = start; i < start + length; i++) {
    Handle<Value> v8Value = array->Get(context, i).ToLocalChecked();
    ASSERT_IS_STRING(v8Value);
    String::Value unicodeString(isolate, v8Value);
    jstring string = env->NewString(*unicodeString, unicodeString.length());
    env->SetObjectArrayElement(result, i - start, string);
    (env)->DeleteLocalRef(string);
  }

  return length;
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1arrayGetIntegers__JJII_3I
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint start, jint length, jintArray result) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));

  return fillIntArray(env, context, array, start, length, result);
}

JNIEXPORT jintArray JNICALL Java_com_eclipsesource_v8_V8__1arrayGetIntegers__JJII
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint start, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  jintArray result = env->NewIntArray(length);
  fillIntArray(env, context, array, start, length, result);

  return result;
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1arrayGetDoubles__JJII_3D
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint start, jint length, jdoubleArray result) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  return fillDoubleArray(env, context, array, start, length, result);
}

JNIEXPORT jdoubleArray JNICALL Java_com_eclipsesource_v8_V8__1arrayGetDoubles__JJII
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint start, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  jdoubleArray result = env->NewDoubleArray(length);
  fillDoubleArray(env, context, array, start, length, result);
  return result;
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1arrayGetBooleans__JJII_3Z
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint start, jint length, jbooleanArray result) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  return fillBooleanArray(env, context, isolate, array, start, length, result);
}

JNIEXPORT jbyteArray JNICALL Java_com_eclipsesource_v8_V8__1arrayGetBytes__JJII
  (JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint start, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  jbyteArray result = env->NewByteArray(length);
  fillByteArray(env, context, array, start, length, result);
  return result;
}

JNIEXPORT jbooleanArray JNICALL Java_com_eclipsesource_v8_V8__1arrayGetBooleans__JJII
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint start, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  jbooleanArray result = env->NewBooleanArray(length);
  fillBooleanArray(env, context, isolate, array, start, length, result);
  return result;
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1arrayGetBytes__JJII_3B
  (JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint start, jint length, jbyteArray result) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  return fillByteArray(env, context, array, start, length, result);
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1arrayGetStrings__JJII_3Ljava_lang_String_2
(JNIEnv * env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint start, jint length, jobjectArray result) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));

  return fillStringArray(env, context, isolate, array, start, length, result);
}

JNIEXPORT jobjectArray JNICALL Java_com_eclipsesource_v8_V8__1arrayGetStrings__JJII
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint start, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  jobjectArray result = env->NewObjectArray(length, stringCls, NULL);
  fillStringArray(env, context, isolate, array, start, length, result);

  return result;
}

JNIEXPORT jboolean JNICALL Java_com_eclipsesource_v8_V8__1arrayGetBoolean
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint index) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, false);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  Handle<Value> v8Value = array->Get(context, index).ToLocalChecked();
  ASSERT_IS_BOOLEAN(v8Value);
  return v8Value->BooleanValue(isolate);
}

JNIEXPORT jbyte JNICALL Java_com_eclipsesource_v8_V8__1arrayGetByte
  (JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint index) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, false);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  Handle<Value> v8Value = array->Get(context, index).ToLocalChecked();
  ASSERT_IS_NUMBER(v8Value);
  return v8Value->Int32Value(context).FromJust();
}

JNIEXPORT jdouble JNICALL Java_com_eclipsesource_v8_V8__1arrayGetDouble
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint index) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  Handle<Value> v8Value = array->Get(context, index).ToLocalChecked();
  ASSERT_IS_NUMBER(v8Value);
  return v8Value->NumberValue(context).FromJust();
}

JNIEXPORT jstring JNICALL Java_com_eclipsesource_v8_V8__1arrayGetString
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint index) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  Handle<Value> v8Value = array->Get(context, index).ToLocalChecked();
  ASSERT_IS_STRING(v8Value);
  String::Value unicodeString(isolate, v8Value);

  return env->NewString(*unicodeString, unicodeString.length());
}

JNIEXPORT jobject JNICALL Java_com_eclipsesource_v8_V8__1arrayGet
(JNIEnv *env, jobject v8, jlong v8RuntimePtr, jint expectedType, jlong arrayHandle, jint index) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  Handle<Value> result = array->Get(context, static_cast<uint32_t>(index)).ToLocalChecked();
  return getResult(env, context, v8, v8RuntimePtr, result, expectedType);
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1addArrayNullItem
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  if ( array->IsTypedArray() ) {
     Local<String> string = String::NewFromUtf8(isolate, "Cannot push to a Typed Array.");
     v8::String::Value strValue(isolate, string);
     throwV8RuntimeException(env, &strValue);
     return;
  }
  uint32_t index = Array::Cast(*array)->Length();
  array->Set(context, index, Null(isolate));
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1addArrayUndefinedItem
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  if ( array->IsTypedArray() ) {
     Local<String> string = String::NewFromUtf8(isolate, "Cannot push to a Typed Array.");
     v8::String::Value strValue(isolate, string);
     throwV8RuntimeException(env, &strValue);
     return;
  }
  int index = Array::Cast(*array)->Length();
  array->Set(context, index, Undefined(isolate));
}


JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1addArrayIntItem
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint value) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  if ( array->IsTypedArray() ) {
     Local<String> string = String::NewFromUtf8(isolate, "Cannot push to a Typed Array.");
     v8::String::Value strValue(isolate, string);
     throwV8RuntimeException(env, &strValue);
     return;
  }
  Local<Value> v8Value = Int32::New(isolate, value);
  int index = Array::Cast(*array)->Length();
  array->Set(context, index, v8Value);
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1addArrayDoubleItem
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jdouble value) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  if ( array->IsTypedArray() ) {
     Local<String> string = String::NewFromUtf8(isolate, "Cannot push to a Typed Array.");
     v8::String::Value strValue(isolate, string);
     throwV8RuntimeException(env, &strValue);
     return;
  }
  Local<Value> v8Value = Number::New(isolate, value);
  int index = Array::Cast(*array)->Length();
  array->Set(context, index, v8Value);
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1addArrayBooleanItem
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jboolean value) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  if ( array->IsTypedArray() ) {
     Local<String> string = String::NewFromUtf8(isolate, "Cannot push to a Typed Array.");
     v8::String::Value strValue(isolate, string);
     throwV8RuntimeException(env, &strValue);
     return;
  }
  Local<Value> v8Value = Boolean::New(isolate, value);
  int index = Array::Cast(*array)->Length();
  array->Set(context, index, v8Value);
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1addArrayStringItem
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jstring value) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  if ( array->IsTypedArray() ) {
     Local<String> string = String::NewFromUtf8(isolate, "Cannot push to a Typed Array.");
     v8::String::Value strValue(isolate, string);
     throwV8RuntimeException(env, &strValue);
     return;
  }
  int index = Array::Cast(*array)->Length();
  Local<String> v8Value = createV8String(env, isolate, value);
  array->Set(context, index, v8Value);
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1addArrayObjectItem
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jlong valueHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  if ( array->IsTypedArray() ) {
     Local<String> string = String::NewFromUtf8(isolate, "Cannot push to a Typed Array.");
     v8::String::Value strValue(isolate, string);
     throwV8RuntimeException(env, &strValue);
     return;
  }
  int index = Array::Cast(*array)->Length();
  Local<Value> v8Value = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(valueHandle));
  array->Set(context, index, v8Value);
}

int getType(Handle<Value> v8Value) {
  if (v8Value.IsEmpty() || v8Value->IsUndefined()) {
    return com_eclipsesource_v8_V8_UNDEFINED;
  }
  else if (v8Value->IsNull()) {
    return com_eclipsesource_v8_V8_NULL;
  }
  else if (v8Value->IsInt32()) {
    return com_eclipsesource_v8_V8_INTEGER;
  }
  else if (v8Value->IsNumber()) {
    return com_eclipsesource_v8_V8_DOUBLE;
  }
  else if (v8Value->IsBoolean()) {
    return com_eclipsesource_v8_V8_BOOLEAN;
  }
  else if (v8Value->IsString()) {
    return com_eclipsesource_v8_V8_STRING;
  }
  else if (v8Value->IsFunction()) {
    return com_eclipsesource_v8_V8_V8_FUNCTION;
  }
  else if (v8Value->IsArrayBuffer()) {
    return com_eclipsesource_v8_V8_V8_ARRAY_BUFFER;
  }
  else if (v8Value->IsTypedArray()) {
    return com_eclipsesource_v8_V8_V8_TYPED_ARRAY;
  }
  else if (v8Value->IsArray()) {
    return com_eclipsesource_v8_V8_V8_ARRAY;
  }
  else if (v8Value->IsObject()) {
    return com_eclipsesource_v8_V8_V8_OBJECT;
  }
  return -1;
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1getType__JJI
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jint index) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0)
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  Handle<Value> v8Value = array->Get(context, static_cast<uint32_t>(index)).ToLocalChecked();
  int type = getType(v8Value);
  if (type < 0) {
    throwResultUndefinedException(env, "");
  }
  return type;
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1getType__JJ
  (JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0)
  Handle<Value> v8Value = Local<Value>::New(isolate, *reinterpret_cast<Persistent<Value>*>(objectHandle));
  return getType(v8Value);
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1getType__JJII
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint start, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0)
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  int result = -1;
  for (int i = start; i < start + length; i++) {
    Handle<Value> v8Value = array->Get(context, i).ToLocalChecked();
    int type = getType(v8Value);
    if (result >= 0 && result != type) {
      throwResultUndefinedException(env, "");
      return -1;
    }
    else if (type < 0) {
      throwResultUndefinedException(env, "");
      return -1;
    }
    result = type;
  }
  if (result < 0) {
    throwResultUndefinedException(env, "");
  }
  return result;
}

jobject createParameterArray(JNIEnv* env, const Local<Context>& context, jlong v8RuntimePtr, jobject v8, int size, const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = getIsolate(env, v8RuntimePtr);
  jobject result = env->NewObject(v8ArrayCls, v8ArrayInitMethodID, v8);
  jlong parameterHandle = env->CallLongMethod(result, v8ArrayGetHandleMethodID);
  Handle<Object> parameters = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(parameterHandle));
  for (int i = 0; i < size; i++) {
    Maybe<bool> unusedResult = parameters->Set(context, static_cast<uint32_t>(i), args[i]);
    unusedResult.Check();
  }
  return result;
}

void voidCallback(const FunctionCallbackInfo<Value>& args) {
  int size = args.Length();
  Local<External> data = Local<External>::Cast(args.Data());
  void *methodDescriptorPtr = data->Value();
  MethodDescriptor* md = static_cast<MethodDescriptor*>(methodDescriptorPtr);
  V8Runtime* v8Runtime = reinterpret_cast<V8Runtime*>(md->v8RuntimePtr);
  jobject v8 = v8Runtime->v8;
  Isolate* isolate = v8Runtime->isolate;
  Isolate::Scope isolateScope(isolate);
  Local<Context> context = v8Runtime->context_.Get(isolate);
  JNIEnv * env;
  getJNIEnv(env);
  jobject parameters = createParameterArray(env, context, md->v8RuntimePtr, v8, size, args);
  Handle<Value> receiver = args.This();
  jobject jreceiver = getResult(env, context, v8, md->v8RuntimePtr, receiver, com_eclipsesource_v8_V8_UNKNOWN);
  env->CallVoidMethod(v8, v8CallVoidMethodID, md->methodID, jreceiver, parameters);
  if (env->ExceptionCheck()) {
    Isolate* isolate = getIsolate(env, md->v8RuntimePtr);
    reinterpret_cast<V8Runtime*>(md->v8RuntimePtr)->pendingException = env->ExceptionOccurred();
    env->ExceptionClear();
    jstring exceptionMessage = static_cast<jstring>(env->CallObjectMethod(reinterpret_cast<V8Runtime*>(md->v8RuntimePtr)->pendingException, throwableGetMessageMethodID));
    if (exceptionMessage != nullptr) {
      Local<String> v8String = createV8String(env, isolate, exceptionMessage);
      isolate->ThrowException(v8String);
    }
    else {
      isolate->ThrowException(String::NewFromUtf8(isolate, "Unhandled Java Exception"));
    }
  }
  env->CallVoidMethod(parameters, v8ArrayReleaseMethodID);
  env->CallVoidMethod(jreceiver, v8ObjectReleaseMethodID);
  env->DeleteLocalRef(jreceiver);
  env->DeleteLocalRef(parameters);
}

int getReturnType(JNIEnv* env, jobject &object) {
  int result = com_eclipsesource_v8_V8_NULL;
  if (env->IsInstanceOf(object, integerCls)) {
    result = com_eclipsesource_v8_V8_INTEGER;
  }
  else if (env->IsInstanceOf(object, doubleCls)) {
    result = com_eclipsesource_v8_V8_DOUBLE;
  }
  else if (env->IsInstanceOf(object, booleanCls)) {
    result = com_eclipsesource_v8_V8_BOOLEAN;
  }
  else if (env->IsInstanceOf(object, stringCls)) {
    result = com_eclipsesource_v8_V8_STRING;
  }
  else if (env->IsInstanceOf(object, v8ArrayCls)) {
    result = com_eclipsesource_v8_V8_V8_ARRAY;
  }
  else if (env->IsInstanceOf(object, v8ObjectCls)) {
    result = com_eclipsesource_v8_V8_V8_OBJECT;
  }
  else if (env->IsInstanceOf(object, v8ArrayBufferCls)) {
    result = com_eclipsesource_v8_V8_V8_ARRAY_BUFFER;
  }
  return result;
}

int getInteger(JNIEnv* env, jobject &object) {
  return env->CallIntMethod(object, integerIntValueMethodID);
}

bool getBoolean(JNIEnv* env, jobject &object) {
  return env->CallBooleanMethod(object, booleanBoolValueMethodID);
}

double getDouble(JNIEnv* env, jobject &object) {
  return env->CallDoubleMethod(object, doubleDoubleValueMethodID);
}

void objectCallback(const FunctionCallbackInfo<Value>& args) {
  int size = args.Length();
  Local<External> data = Local<External>::Cast(args.Data());
  void *methodDescriptorPtr = data->Value();
  MethodDescriptor* md = static_cast<MethodDescriptor*>(methodDescriptorPtr);
  V8Runtime* v8Runtime = reinterpret_cast<V8Runtime*>(md->v8RuntimePtr);
  jobject v8 = v8Runtime->v8;
  Isolate* isolate = v8Runtime->isolate;
  Isolate::Scope isolateScope(isolate);
  Local<Context> context = v8Runtime->context_.Get(isolate);
  JNIEnv * env;
  getJNIEnv(env);
  jobject parameters = createParameterArray(env, context, md->v8RuntimePtr, v8, size, args);
  Handle<Value> receiver = args.This();
  jobject jreceiver = getResult(env, context, v8, md->v8RuntimePtr, receiver, com_eclipsesource_v8_V8_UNKNOWN);
  jobject resultObject = env->CallObjectMethod(v8, v8CallObjectJavaMethodMethodID, md->methodID, jreceiver, parameters);
  if (env->ExceptionCheck()) {
    resultObject = nullptr;
    Isolate* isolate = getIsolate(env, md->v8RuntimePtr);
    reinterpret_cast<V8Runtime*>(md->v8RuntimePtr)->pendingException = env->ExceptionOccurred();
    env->ExceptionClear();
    jstring exceptionMessage = (jstring)env->CallObjectMethod(reinterpret_cast<V8Runtime*>(md->v8RuntimePtr)->pendingException, throwableGetMessageMethodID);
    if (exceptionMessage != nullptr) {
      Local<String> v8String = createV8String(env, isolate, exceptionMessage);
      isolate->ThrowException(v8String);
    }
    else {
      isolate->ThrowException(String::NewFromUtf8(isolate, "Unhandled Java Exception"));
    }
  }
  else if (resultObject == nullptr) {
    args.GetReturnValue().SetNull();
  }
  else {
    int returnType = getReturnType(env, resultObject);
    if (returnType == com_eclipsesource_v8_V8_INTEGER) {
      args.GetReturnValue().Set(getInteger(env, resultObject));
    }
    else if (returnType == com_eclipsesource_v8_V8_BOOLEAN) {
      args.GetReturnValue().Set(getBoolean(env, resultObject));
    }
    else if (returnType == com_eclipsesource_v8_V8_DOUBLE) {
      args.GetReturnValue().Set(getDouble(env, resultObject));
    }
    else if (returnType == com_eclipsesource_v8_V8_STRING) {
      jstring stringResult = (jstring)resultObject;
      Local<String> result = createV8String(env, reinterpret_cast<V8Runtime*>(md->v8RuntimePtr)->isolate, stringResult);
      args.GetReturnValue().Set(result);
    }
    else if (returnType == com_eclipsesource_v8_V8_V8_ARRAY) {
      if (isUndefined(env, resultObject)) {
        args.GetReturnValue().SetUndefined();
      }
      else {
        jlong resultHandle = getHandle(env, resultObject);
        Handle<Object> result = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(resultHandle));
        releaseArray(env, resultObject);
        args.GetReturnValue().Set(result);
      }
    }
    else if (returnType == com_eclipsesource_v8_V8_V8_OBJECT) {
      if (isUndefined(env, resultObject)) {
        args.GetReturnValue().SetUndefined();
      }
      else {
        jlong resultHandle = getHandle(env, resultObject);
        Handle<Object> result = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(resultHandle));
        release(env, resultObject);
        args.GetReturnValue().Set(result);
      }
    }
    else if (returnType == com_eclipsesource_v8_V8_V8_ARRAY_BUFFER) {
      if (isUndefined(env, resultObject)) {
        args.GetReturnValue().SetUndefined();
      }
      else {
        jlong resultHandle = getHandle(env, resultObject);
        Handle<Object> result = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(resultHandle));
        release(env, resultObject);
        args.GetReturnValue().Set(result);
      }
    }
    else {
      args.GetReturnValue().SetUndefined();
    }
  }
  if (resultObject != nullptr) {
    env->DeleteLocalRef(resultObject);
  }
  env->CallVoidMethod(parameters, v8ArrayReleaseMethodID);
  env->CallVoidMethod(jreceiver, v8ObjectReleaseMethodID);
  env->DeleteLocalRef(jreceiver);
  env->DeleteLocalRef(parameters);
}

JNIEXPORT jlongArray JNICALL Java_com_eclipsesource_v8_V8__1initNewV8Function
(JNIEnv *env, jobject, jlong v8RuntimePtr) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  MethodDescriptor* md = new MethodDescriptor();
  Local<External> ext = External::New(isolate, md);
  Persistent<External> pext(isolate, ext);
  isolate->IdleNotificationDeadline(1);
  pext.SetWeak(md, [](v8::WeakCallbackInfo<MethodDescriptor> const& data) {
    MethodDescriptor* md = data.GetParameter();
    jobject v8 = reinterpret_cast<V8Runtime*>(md->v8RuntimePtr)->v8;
    JNIEnv * env;
    getJNIEnv(env);
    env->CallVoidMethod(v8, v8DisposeMethodID, md->methodID);
    delete(md);
  }, WeakCallbackType::kParameter);

  Local<Function> function = Function::New(context, objectCallback, ext).ToLocalChecked();
  md->v8RuntimePtr = v8RuntimePtr;
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, function);
  md->methodID = reinterpret_cast<jlong>(md);

  // Position 0 is the pointer to the container, position 1 is the pointer to the descriptor
  jlongArray result = env->NewLongArray(2);
  jlong * fill = new jlong[2];
  fill[0] = reinterpret_cast<jlong>(container);
  fill[1] = md->methodID;
  (env)->SetLongArrayRegion(result, 0, 2, fill);
  return result;
}

v8::Isolate* isolate_;
Local<v8::Context> context_;

#define JS_STR(val) String::NewFromUtf8(isolate_, val)
#define JS_INT(val) Integer::New(isolate_, val)
#define JS_NUM(val) Number::New(isolate_, val)
#define JS_FLOAT(val) Number::New(isolate_, val)
#define JS_BOOL(val) Boolean::New(isolate_, val)
#define NATIVE_INT(val) val->Int32Value(context_).FromJust()
#define NATIVE_UINT(val) val->Uint32Value(context_).FromJust()
#define NATIVE_NUM(val) val->NumberValue(context_).FromJust()
#define NATIVE_FLOAT(val) val->NumberValue(context_).FromJust()
#define NATIVE_BOOL(val) val->BooleanValue(context_).FromJust()
#define FUNC(val) Function::New(context_, val).ToLocalChecked()

const char* GLErrorToString(GLenum err) {
    switch (err) {
    case GL_NO_ERROR:                      return "GL_NO_ERROR";
    case GL_INVALID_ENUM:                  return "GL_INVALID_ENUM";
    case GL_INVALID_VALUE:                 return "GL_INVALID_VALUE";
    case GL_INVALID_OPERATION:             return "GL_INVALID_OPERATION";
    case GL_OUT_OF_MEMORY:                 return "GL_OUT_OF_MEMORY";
    case 0x8031: /* not core */            return "GL_TABLE_TOO_LARGE_EXT";
    case 0x8065: /* not core */            return "GL_TEXTURE_TOO_LARGE_EXT";
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
    default:
        return NULL;
    }
}

#define BAD_PARAMETER_TYPE(x) x->ThrowException(v8::Exception::Error(String::NewFromUtf8(x, "Bad parameter type")))
bool CHECK_GL_ERRORS = false;

#define GL_ERROR_THROW(x) \
if (CHECK_GL_ERRORS) { \
  GLenum err = glGetError(); \
  if (err != GL_NO_ERROR) { \
      x->ThrowException(v8::Exception::Error(String::NewFromUtf8(x, GLErrorToString(err))));\
  } \
} \

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a)                                   \
  ((sizeof(a) / sizeof(*(a))) /                         \
  static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))
#endif

v8::Persistent<Function> _PromiseRejection;
void AURA_PromiseRejectCallback(v8::PromiseRejectMessage message) {
    Local<Promise> promise = message.GetPromise();
    Isolate* isolate = promise->GetIsolate();
    Local<Value> value = message.GetValue();
    Local<Integer> event = Integer::New(isolate, message.GetEvent());

    v8::Local<v8::Function> fn = v8::Local<v8::Function>::New(isolate, _PromiseRejection);

    if (value.IsEmpty())
      value = Undefined(isolate);

    Local<Value> args[] = { event, promise, value };

    fn->Call(context_, v8::Null(isolate), ARRAY_SIZE(args), args);
}
void AURA_BindPromiseRejection(const FunctionCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();
    Local<Function> fn = Local<Function>::Cast(args[0]);
    _PromiseRejection.Reset(isolate, fn);
    isolate->SetPromiseRejectCallback(AURA_PromiseRejectCallback);
}

void AURA_CheckErrors (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];

    CHECK_GL_ERRORS = NATIVE_BOOL(arg0);
}

void v8Bind_ActiveTexture (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glActiveTexture((GLenum)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
}

void v8Bind_AttachShader (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glAttachShader((GLuint)NATIVE_INT(arg0),(GLuint)NATIVE_INT(arg1));
    GL_ERROR_THROW(isolate);
}

void v8Bind_BindAttribLocation (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsString()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    String::Utf8Value utf8_value(isolate, arg2);
    glBindAttribLocation((GLuint)NATIVE_INT(arg0),(GLuint)NATIVE_INT(arg1),*utf8_value);
    GL_ERROR_THROW(isolate);
}

void v8Bind_BindBuffer (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glBindBuffer((GLenum)NATIVE_INT(arg0),(GLuint)NATIVE_INT(arg1));
    GL_ERROR_THROW(isolate);
}

void v8Bind_BindFramebuffer (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glBindFramebuffer((GLenum)NATIVE_INT(arg0),(GLuint)NATIVE_INT(arg1));
    GL_ERROR_THROW(isolate);
}

void v8Bind_BindRenderbuffer (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glBindRenderbuffer((GLenum)NATIVE_INT(arg0),(GLuint)NATIVE_INT(arg1));
    GL_ERROR_THROW(isolate);
}

void v8Bind_BindTexture (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLenum type = (GLenum)NATIVE_INT(arg0);
    GLuint handle = (GLuint)NATIVE_UINT(arg1);

    glBindTexture(type,handle);
    GL_ERROR_THROW(isolate);
}

void v8Bind_BlendColor (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    if(!arg0->IsNumber() || !arg1->IsNumber() || !arg2->IsNumber() || !arg3->IsNumber()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glBlendColor((GLclampf)NATIVE_NUM(arg0),(GLclampf)NATIVE_NUM(arg1),(GLclampf)NATIVE_NUM(arg2),(GLclampf)NATIVE_NUM(arg3));
    GL_ERROR_THROW(isolate);
}

void v8Bind_BlendEquation (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glBlendEquation((GLenum)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
}

void v8Bind_BlendEquationSeparate (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glBlendEquationSeparate((GLenum)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1));
    GL_ERROR_THROW(isolate);
}

void v8Bind_BlendFunc (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glBlendFunc((GLenum)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1));
    GL_ERROR_THROW(isolate);
}

void v8Bind_BlendFuncSeparate (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsInt32() || !arg3->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glBlendFuncSeparate((GLenum)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1),(GLenum)NATIVE_INT(arg2),(GLenum)NATIVE_INT(arg3));
    GL_ERROR_THROW(isolate);
}

void v8Bind_BufferData (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !(arg2->IsTypedArray() || arg2->IsNull()) || !arg3->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    const GLvoid* ptr0= ( arg2->IsTypedArray() ) ?
                        (const GLvoid*)v8::Local<v8::TypedArray>::Cast(arg2)->Buffer()->GetContents().Data() :
                        nullptr;

    glBufferData((GLenum)NATIVE_INT(arg0),(GLsizeiptr)NATIVE_INT(arg1),ptr0,(GLenum)NATIVE_INT(arg3));
    GL_ERROR_THROW(isolate);
}

void v8Bind_BufferSubData (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsInt32() || !(arg3->IsTypedArray() || arg3->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    const GLvoid* ptr0= ( arg3->IsTypedArray() ) ?
                        (const GLvoid*)v8::Local<v8::TypedArray>::Cast(arg3)->Buffer()->GetContents().Data() :
                        nullptr;

    glBufferSubData((GLenum)NATIVE_INT(arg0),(GLintptr)NATIVE_INT(arg1),(GLsizeiptr)NATIVE_INT(arg2),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_CheckFramebufferStatus (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLenum ret=  glCheckFramebufferStatus((GLenum)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
    args.GetReturnValue().Set( v8::Integer::New( isolate, ret ) );
}

void v8Bind_Clear (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glClear((GLbitfield)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
}

void v8Bind_ClearColor (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    if(!arg0->IsNumber() || !arg1->IsNumber() || !arg2->IsNumber() || !arg3->IsNumber()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glClearColor((GLclampf)NATIVE_NUM(arg0),(GLclampf)NATIVE_NUM(arg1),(GLclampf)NATIVE_NUM(arg2),(GLclampf)NATIVE_NUM(arg3));
    GL_ERROR_THROW(isolate);
}

void v8Bind_ClearDepthf (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsNumber()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glClearDepthf((GLclampf)NATIVE_NUM(arg0));
    GL_ERROR_THROW(isolate);
}

void v8Bind_ClearStencil (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glClearStencil((GLint)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
}

void v8Bind_ColorMask (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    if(!arg0->IsBoolean() || !arg1->IsBoolean() || !arg2->IsBoolean() || !arg3->IsBoolean()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glColorMask((GLboolean)NATIVE_BOOL(arg0),(GLboolean)NATIVE_BOOL(arg1),(GLboolean)NATIVE_BOOL(arg2),(GLboolean)NATIVE_BOOL(arg3));
    GL_ERROR_THROW(isolate);
}

void v8Bind_CompileShader (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glCompileShader((GLuint)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
}

void v8Bind_CompressedTexImage2D (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    v8::Local<v8::Value> arg4= args[4];
    v8::Local<v8::Value> arg5= args[5];
    v8::Local<v8::Value> arg6= args[6];
    v8::Local<v8::Value> arg7= args[7];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsInt32() || !arg3->IsInt32() || !arg4->IsInt32() || !arg5->IsInt32() || !arg6->IsInt32() || !(arg7->IsTypedArray() || arg7->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    const GLvoid* ptr0= ( arg7->IsTypedArray() ) ?
                        (const GLvoid*)v8::Local<v8::TypedArray>::Cast(arg7)->Buffer()->GetContents().Data() :
                        nullptr;

    glCompressedTexImage2D((GLenum)NATIVE_INT(arg0),(GLint)NATIVE_INT(arg1),(GLenum)NATIVE_INT(arg2),(GLsizei)NATIVE_INT(arg3),(GLsizei)NATIVE_INT(arg4),(GLint)NATIVE_INT(arg5),(GLsizei)NATIVE_INT(arg6),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_CompressedTexSubImage2D (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    v8::Local<v8::Value> arg4= args[4];
    v8::Local<v8::Value> arg5= args[5];
    v8::Local<v8::Value> arg6= args[6];
    v8::Local<v8::Value> arg7= args[7];
    v8::Local<v8::Value> arg8= args[8];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsInt32() || !arg3->IsInt32() || !arg4->IsInt32() || !arg5->IsInt32() || !arg6->IsInt32() || !arg7->IsInt32() || !(arg8->IsTypedArray() || arg8->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    const GLvoid* ptr0= ( arg8->IsTypedArray() ) ?
                        (const GLvoid*)v8::Local<v8::TypedArray>::Cast(arg8)->Buffer()->GetContents().Data() :
                        nullptr;

    glCompressedTexSubImage2D((GLenum)NATIVE_INT(arg0),(GLint)NATIVE_INT(arg1),(GLint)NATIVE_INT(arg2),(GLint)NATIVE_INT(arg3),(GLsizei)NATIVE_INT(arg4),(GLsizei)NATIVE_INT(arg5),(GLenum)NATIVE_INT(arg6),(GLsizei)NATIVE_INT(arg7),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_CopyTexImage2D (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    v8::Local<v8::Value> arg4= args[4];
    v8::Local<v8::Value> arg5= args[5];
    v8::Local<v8::Value> arg6= args[6];
    v8::Local<v8::Value> arg7= args[7];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsInt32() || !arg3->IsInt32() || !arg4->IsInt32() || !arg5->IsInt32() || !arg6->IsInt32() || !arg7->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glCopyTexImage2D((GLenum)NATIVE_INT(arg0),(GLint)NATIVE_INT(arg1),(GLenum)NATIVE_INT(arg2),(GLint)NATIVE_INT(arg3),(GLint)NATIVE_INT(arg4),(GLsizei)NATIVE_INT(arg5),(GLsizei)NATIVE_INT(arg6),(GLint)NATIVE_INT(arg7));
    GL_ERROR_THROW(isolate);
}

void v8Bind_CopyTexSubImage2D (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    v8::Local<v8::Value> arg4= args[4];
    v8::Local<v8::Value> arg5= args[5];
    v8::Local<v8::Value> arg6= args[6];
    v8::Local<v8::Value> arg7= args[7];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsInt32() || !arg3->IsInt32() || !arg4->IsInt32() || !arg5->IsInt32() || !arg6->IsInt32() || !arg7->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glCopyTexSubImage2D((GLenum)NATIVE_INT(arg0),(GLint)NATIVE_INT(arg1),(GLint)NATIVE_INT(arg2),(GLint)NATIVE_INT(arg3),(GLint)NATIVE_INT(arg4),(GLint)NATIVE_INT(arg5),(GLsizei)NATIVE_INT(arg6),(GLsizei)NATIVE_INT(arg7));
    GL_ERROR_THROW(isolate);
}

void v8Bind_CreateProgram (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    GLuint ret=  glCreateProgram();
    GL_ERROR_THROW(isolate);
    args.GetReturnValue().Set( v8::Integer::New( isolate, ret ) );
}

void v8Bind_CreateShader (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLuint ret=  glCreateShader((GLenum)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
    args.GetReturnValue().Set( v8::Integer::New( isolate, ret ) );
}

void v8Bind_CullFace (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glCullFace((GLenum)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
}

void v8Bind_DeleteBuffers (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !(arg1->IsTypedArray() || arg1->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    const GLuint* ptr0= ( arg1->IsTypedArray() ) ?
                        (const GLuint*)v8::Local<v8::TypedArray>::Cast(arg1)->Buffer()->GetContents().Data() :
                        nullptr;

    glDeleteBuffers((GLsizei)NATIVE_INT(arg0),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_DeleteFramebuffers (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !(arg1->IsTypedArray() || arg1->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    const GLuint* ptr0= ( arg1->IsTypedArray() ) ?
                        (const GLuint*)v8::Local<v8::TypedArray>::Cast(arg1)->Buffer()->GetContents().Data() :
                        nullptr;

    glDeleteFramebuffers((GLsizei)NATIVE_INT(arg0),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_DeleteProgram (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glDeleteProgram((GLuint)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
}

void v8Bind_DeleteRenderbuffers (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !(arg1->IsTypedArray() || arg1->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    const GLuint* ptr0= ( arg1->IsTypedArray() ) ?
                        (const GLuint*)v8::Local<v8::TypedArray>::Cast(arg1)->Buffer()->GetContents().Data() :
                        nullptr;

    glDeleteRenderbuffers((GLsizei)NATIVE_INT(arg0),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_DeleteShader (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glDeleteShader((GLuint)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
}

void v8Bind_DeleteTextures (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !(arg1->IsTypedArray() || arg1->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    const GLuint* ptr0= ( arg1->IsTypedArray() ) ?
                        (const GLuint*)v8::Local<v8::TypedArray>::Cast(arg1)->Buffer()->GetContents().Data() :
                        nullptr;

    glDeleteTextures((GLsizei)NATIVE_INT(arg0),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_DepthFunc (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glDepthFunc((GLenum)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
}

void v8Bind_DepthMask (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsBoolean()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glDepthMask((GLboolean)NATIVE_BOOL(arg0));
    GL_ERROR_THROW(isolate);
}

void v8Bind_DepthRangef (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsNumber() || !arg1->IsNumber()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glDepthRangef((GLclampf)NATIVE_NUM(arg0),(GLclampf)NATIVE_NUM(arg1));
    GL_ERROR_THROW(isolate);
}

void v8Bind_DetachShader (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glDetachShader((GLuint)NATIVE_INT(arg0),(GLuint)NATIVE_INT(arg1));
    GL_ERROR_THROW(isolate);
}

void v8Bind_Disable (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glDisable((GLenum)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
}

void v8Bind_DisableVertexAttribArray (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glDisableVertexAttribArray((GLuint)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
}

void v8Bind_DrawArrays (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glDrawArrays((GLenum)NATIVE_INT(arg0),(GLint)NATIVE_INT(arg1),(GLsizei)NATIVE_INT(arg2));
    GL_ERROR_THROW(isolate);
}

void v8Bind_DrawElements (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsInt32() || !arg3->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glDrawElements((GLenum)NATIVE_INT(arg0),(GLsizei)NATIVE_INT(arg1),(GLenum)NATIVE_INT(arg2),(GLvoid*)NATIVE_INT(arg3));
    GL_ERROR_THROW(isolate);
}

void v8Bind_Enable (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glEnable((GLenum)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
}

void v8Bind_EnableVertexAttribArray (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glEnableVertexAttribArray((GLuint)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
}

void v8Bind_Finish (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    glFinish();
    GL_ERROR_THROW(isolate);
}

void v8Bind_Flush (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    glFlush();
    GL_ERROR_THROW(isolate);
}

void v8Bind_FramebufferRenderbuffer (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsInt32() || !arg3->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glFramebufferRenderbuffer((GLenum)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1),(GLenum)NATIVE_INT(arg2),(GLuint)NATIVE_INT(arg3));
    GL_ERROR_THROW(isolate);
}

void v8Bind_FramebufferTexture2D (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    v8::Local<v8::Value> arg4= args[4];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsInt32() || !arg3->IsInt32() || !arg4->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glFramebufferTexture2D((GLenum)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1),(GLenum)NATIVE_INT(arg2),(GLuint)NATIVE_INT(arg3),(GLint)NATIVE_INT(arg4));
    GL_ERROR_THROW(isolate);
}

void v8Bind_FrontFace (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glFrontFace((GLenum)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
}

void v8Bind_GenBuffers (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !(arg1->IsTypedArray() || arg1->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLuint* ptr0= ( arg1->IsTypedArray() ) ?
                  (GLuint*)v8::Local<v8::TypedArray>::Cast(arg1)->Buffer()->GetContents().Data() :
                  nullptr;

    glGenBuffers((GLsizei)NATIVE_INT(arg0),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_GenerateMipmap (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glGenerateMipmap((GLenum)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
}

void v8Bind_GenFramebuffers (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !(arg1->IsTypedArray() || arg1->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLuint* ptr0= ( arg1->IsTypedArray() ) ?
                  (GLuint*)v8::Local<v8::TypedArray>::Cast(arg1)->Buffer()->GetContents().Data() :
                  nullptr;

    glGenFramebuffers((GLsizei)NATIVE_INT(arg0),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_GenRenderbuffers (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !(arg1->IsTypedArray() || arg1->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLuint* ptr0= ( arg1->IsTypedArray() ) ?
                  (GLuint*)v8::Local<v8::TypedArray>::Cast(arg1)->Buffer()->GetContents().Data() :
                  nullptr;

    glGenRenderbuffers((GLsizei)NATIVE_INT(arg0),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_GenTextures (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !(arg1->IsTypedArray() || arg1->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLuint* ptr0= ( arg1->IsTypedArray() ) ?
                  (GLuint*)v8::Local<v8::TypedArray>::Cast(arg1)->Buffer()->GetContents().Data() :
                  nullptr;

    glGenTextures((GLsizei)NATIVE_INT(arg0),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_GetActiveAttrib (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32() ) {
        BAD_PARAMETER_TYPE(isolate);
    }

    int bufSize = 512;
    int retlength;
    GLint retsize;
    GLenum type;
    GLchar *name;

    name = new GLchar[512];

    glGetActiveAttrib(
            (GLuint)NATIVE_INT(arg0),
            (GLuint)NATIVE_INT(arg1),
            bufSize,
            &retlength,
            &retsize,
            &type,
            name
    );
    GL_ERROR_THROW(isolate);

    v8::HandleScope hs(isolate);

    v8::Local<v8::Object> obj = v8::Object::New(isolate);
    obj->Set(
            JS_STR("name"),
            JS_STR(name)
    );
    obj->Set(
            JS_STR("size"),
            JS_NUM(retsize)
    );
    obj->Set(
            JS_STR("type"),
            JS_NUM(type)
    );

    args.GetReturnValue().Set(obj);

    delete name;


}

void v8Bind_GetActiveUniform (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];

    if(!arg0->IsInt32() || !arg1->IsInt32() ) {
        BAD_PARAMETER_TYPE(isolate);
    }

    int bufSize = 512;
    int retlength;
    GLint retsize;
    GLenum type;
    GLchar *name;

    name = new GLchar[512];

    glGetActiveUniform(
            (GLuint)NATIVE_INT(arg0),
            (GLuint)NATIVE_INT(arg1),
            bufSize,
            &retlength,
            &retsize,
            &type,
            name
    );
    GL_ERROR_THROW(isolate);

    v8::HandleScope hs(isolate);

    v8::Local<v8::Object> obj = v8::Object::New(isolate);
    obj->Set(
            JS_STR("name"),
            JS_STR(name)
    );
    obj->Set(
            JS_STR("size"),
            JS_NUM(retsize)
    );
    obj->Set(
            JS_STR("type"),
            JS_NUM(type)
    );

    args.GetReturnValue().Set(obj);

    delete name;
}

void v8Bind_GetAttachedShaders (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !(arg2->IsTypedArray() || arg2->IsNull()) || !(arg3->IsTypedArray() || arg3->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLsizei* ptr0= ( arg2->IsTypedArray() ) ?
                   (GLsizei*)v8::Local<v8::TypedArray>::Cast(arg2)->Buffer()->GetContents().Data() :
                   nullptr;
    GLuint* ptr1= ( arg3->IsTypedArray() ) ?
                  (GLuint*)v8::Local<v8::TypedArray>::Cast(arg3)->Buffer()->GetContents().Data() :
                  nullptr;

    glGetAttachedShaders((GLuint)NATIVE_INT(arg0),(GLsizei)NATIVE_INT(arg1),ptr0,ptr1);
    GL_ERROR_THROW(isolate);
}

void v8Bind_GetAttribLocation (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsString()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    String::Utf8Value utf8_value(isolate, arg1);
    GLint ret=  glGetAttribLocation((GLuint)NATIVE_INT(arg0),*utf8_value);
    GL_ERROR_THROW(isolate);
    args.GetReturnValue().Set( v8::Integer::New( isolate, ret ) );
}

void v8Bind_GetBooleanv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !(arg1->IsTypedArray() || arg1->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLboolean* ptr0= ( arg1->IsTypedArray() ) ?
                     (GLboolean*)v8::Local<v8::TypedArray>::Cast(arg1)->Buffer()->GetContents().Data() :
                     nullptr;

    glGetBooleanv((GLenum)NATIVE_INT(arg0),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_GetBufferParameteriv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !(arg2->IsTypedArray() || arg2->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLint* ptr0= ( arg2->IsTypedArray() ) ?
                 (GLint*)v8::Local<v8::TypedArray>::Cast(arg2)->Buffer()->GetContents().Data() :
                 nullptr;

    glGetBufferParameteriv((GLenum)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_GetError (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    GLenum ret=  glGetError();
    GL_ERROR_THROW(isolate);
    args.GetReturnValue().Set( v8::Integer::New( isolate, ret ) );
}

void v8Bind_GetFloatv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !(arg1->IsTypedArray() || arg1->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLfloat* ptr0= ( arg1->IsTypedArray() ) ?
                   (GLfloat*)v8::Local<v8::TypedArray>::Cast(arg1)->Buffer()->GetContents().Data() :
                   nullptr;

    glGetFloatv((GLenum)NATIVE_INT(arg0),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_GetFramebufferAttachmentParameteriv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsInt32() || !(arg3->IsTypedArray() || arg3->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLint* ptr0= ( arg3->IsTypedArray() ) ?
                 (GLint*)v8::Local<v8::TypedArray>::Cast(arg3)->Buffer()->GetContents().Data() :
                 nullptr;

    glGetFramebufferAttachmentParameteriv((GLenum)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1),(GLenum)NATIVE_INT(arg2),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_GetIntegerv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !(arg1->IsTypedArray() || arg1->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLint* ptr0= ( arg1->IsTypedArray() ) ?
                 (GLint*)v8::Local<v8::TypedArray>::Cast(arg1)->Buffer()->GetContents().Data() :
                 nullptr;

    GLenum p= (GLenum)NATIVE_INT(arg0);
    glGetIntegerv(p,ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_GetProgramiv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !(arg2->IsTypedArray() || arg2->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLint* ptr0= ( arg2->IsTypedArray() ) ?
                 (GLint*)v8::Local<v8::TypedArray>::Cast(arg2)->Buffer()->GetContents().Data() :
                 nullptr;

    glGetProgramiv((GLuint)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_GetProgramInfoLog (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    int len = 1024;
    int retlen = 0;
    char buff[1024];

    glGetProgramInfoLog((GLuint)NATIVE_INT(arg0),len, &retlen, buff);
    GL_ERROR_THROW(isolate);

    args.GetReturnValue().Set( JS_STR( buff ) ) ;
}

void v8Bind_GetRenderbufferParameteriv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !(arg2->IsTypedArray() || arg2->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLint* ptr0= ( arg2->IsTypedArray() ) ?
                 (GLint*)v8::Local<v8::TypedArray>::Cast(arg2)->Buffer()->GetContents().Data() :
                 nullptr;

    glGetRenderbufferParameteriv((GLenum)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_GetShaderiv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !(arg2->IsTypedArray() || arg2->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLint* ptr0= ( arg2->IsTypedArray() ) ?
                 (GLint*)v8::Local<v8::TypedArray>::Cast(arg2)->Buffer()->GetContents().Data() :
                 nullptr;

    glGetShaderiv((GLuint)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_GetShaderInfoLog (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32() ) {
        BAD_PARAMETER_TYPE(isolate);
    }

    int len = 1024;
    int retlen = 0;
    char buff[1024];

    glGetShaderInfoLog((GLuint)NATIVE_INT(arg0),len, &retlen, buff);
    GL_ERROR_THROW(isolate);

    args.GetReturnValue().Set( JS_STR( buff ) ) ;
}

void v8Bind_GetShaderPrecisionFormat (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLint precision[2];
    GLint decimal;

    glGetShaderPrecisionFormat( (GLenum)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1),precision,&decimal);
    GL_ERROR_THROW(isolate);

    v8::HandleScope hs(isolate);

    v8::Local<v8::Object> ret = v8::Object::New(isolate);
    ret->Set( JS_STR("rangeMin"), JS_INT(precision[0]) );
    ret->Set( JS_STR("rangeMax"), JS_INT(precision[1]) );
    ret->Set( JS_STR("precision"), JS_INT(decimal) );

    args.GetReturnValue().Set( ret );
}

void v8Bind_GetShaderSource (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !(arg2->IsTypedArray() || arg2->IsNull()) || !arg3->IsString()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLsizei* ptr0= ( arg2->IsTypedArray() ) ?
                   (GLsizei*)v8::Local<v8::TypedArray>::Cast(arg2)->Buffer()->GetContents().Data() :
                   nullptr;

    String::Utf8Value utf8_value(isolate, arg3);
    glGetShaderSource((GLuint)NATIVE_INT(arg0),(GLsizei)NATIVE_INT(arg1),ptr0,*utf8_value);
    GL_ERROR_THROW(isolate);
}

void v8Bind_GetString (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    const GLubyte* ret=  glGetString((GLenum)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
    args.GetReturnValue().Set( JS_STR( (const char*)ret ) );
}

void v8Bind_GetTexParameterfv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !(arg2->IsTypedArray() || arg2->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLfloat* ptr0= ( arg2->IsTypedArray() ) ?
                   (GLfloat*)v8::Local<v8::TypedArray>::Cast(arg2)->Buffer()->GetContents().Data() :
                   nullptr;

    glGetTexParameterfv((GLenum)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_GetTexParameteriv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !(arg2->IsTypedArray() || arg2->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLint* ptr0= ( arg2->IsTypedArray() ) ?
                 (GLint*)v8::Local<v8::TypedArray>::Cast(arg2)->Buffer()->GetContents().Data() :
                 nullptr;

    glGetTexParameteriv((GLenum)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_GetUniformfv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !(arg2->IsTypedArray() || arg2->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLfloat* ptr0= ( arg2->IsTypedArray() ) ?
                   (GLfloat*)v8::Local<v8::TypedArray>::Cast(arg2)->Buffer()->GetContents().Data() :
                   nullptr;

    glGetUniformfv((GLuint)NATIVE_INT(arg0),(GLint)NATIVE_INT(arg1),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_GetUniformiv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !(arg2->IsTypedArray() || arg2->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLint* ptr0= ( arg2->IsTypedArray() ) ?
                 (GLint*)v8::Local<v8::TypedArray>::Cast(arg2)->Buffer()->GetContents().Data() :
                 nullptr;

    glGetUniformiv((GLuint)NATIVE_INT(arg0),(GLint)NATIVE_INT(arg1),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_GetUniformLocation (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsString()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    String::Utf8Value utf8_value(isolate, arg1);
    GLint ret=  glGetUniformLocation((GLuint)NATIVE_INT(arg0),*utf8_value);
    GL_ERROR_THROW(isolate);
    args.GetReturnValue().Set( v8::Integer::New( isolate, ret ) );
}

void v8Bind_GetVertexAttribfv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !(arg2->IsTypedArray() || arg2->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLfloat* ptr0= ( arg2->IsTypedArray() ) ?
                   (GLfloat*)v8::Local<v8::TypedArray>::Cast(arg2)->Buffer()->GetContents().Data() :
                   nullptr;

    glGetVertexAttribfv((GLuint)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_GetVertexAttribiv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !(arg2->IsTypedArray() || arg2->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLint* ptr0= ( arg2->IsTypedArray() ) ?
                 (GLint*)v8::Local<v8::TypedArray>::Cast(arg2)->Buffer()->GetContents().Data() :
                 nullptr;

    glGetVertexAttribiv((GLuint)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_GetVertexAttribPointerv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsTypedArray()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    void *ptr0= v8::Local<v8::TypedArray>::Cast(arg2)->Buffer()->GetContents().Data();

    glGetVertexAttribPointerv((GLuint)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1),&ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_Hint (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glHint((GLenum)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1));
    GL_ERROR_THROW(isolate);
}

void v8Bind_IsBuffer (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLboolean ret=  glIsBuffer((GLuint)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
    args.GetReturnValue().Set( v8::Boolean::New( isolate, ret ) );
}

void v8Bind_IsEnabled (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLboolean ret=  glIsEnabled((GLenum)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
    args.GetReturnValue().Set( v8::Boolean::New( isolate, ret ) );
}

void v8Bind_IsFramebuffer (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLboolean ret=  glIsFramebuffer((GLuint)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
    args.GetReturnValue().Set( v8::Boolean::New( isolate, ret ) );
}

void v8Bind_IsProgram (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLboolean ret=  glIsProgram((GLuint)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
    args.GetReturnValue().Set( v8::Boolean::New( isolate, ret ) );
}

void v8Bind_IsRenderbuffer (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLboolean ret=  glIsRenderbuffer((GLuint)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
    args.GetReturnValue().Set( v8::Boolean::New( isolate, ret ) );
}

void v8Bind_IsShader (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLboolean ret=  glIsShader((GLuint)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
    args.GetReturnValue().Set( v8::Boolean::New( isolate, ret ) );
}

void v8Bind_IsTexture (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLboolean ret=  glIsTexture((GLuint)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
    args.GetReturnValue().Set( v8::Boolean::New( isolate, ret ) );
}

void v8Bind_LineWidth (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsNumber()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glLineWidth((GLfloat)NATIVE_NUM(arg0));
    GL_ERROR_THROW(isolate);
}

void v8Bind_LinkProgram (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glLinkProgram((GLuint)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
}

void v8Bind_PixelStorei (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLenum pname = NATIVE_INT(arg0);
    GLenum param = NATIVE_INT(arg1);

    //Handle WebGL specific extensions
    switch(pname) {
        case 0x9240:
            // unpack_flip_y = param != 0;
            break;

        case 0x9241:
            // unpack_premultiply_alpha = param != 0;
            break;

        case 0x9243:
            // unpack_colorspace_conversion = param;
            break;

        case GL_UNPACK_ALIGNMENT:
            // unpack_alignment = param;
            glPixelStorei(pname, param);
            break;

        default:
            glPixelStorei(pname, param);
            break;
    }

    GL_ERROR_THROW(isolate);
}

void v8Bind_PolygonOffset (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsNumber() || !arg1->IsNumber()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glPolygonOffset((GLfloat)NATIVE_NUM(arg0),(GLfloat)NATIVE_NUM(arg1));
    GL_ERROR_THROW(isolate);
}

void v8Bind_ReadPixels (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    v8::Local<v8::Value> arg4= args[4];
    v8::Local<v8::Value> arg5= args[5];
    v8::Local<v8::Value> arg6= args[6];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsInt32() || !arg3->IsInt32() || !arg4->IsInt32() || !arg5->IsInt32() || !(arg6->IsTypedArray() || arg6->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLvoid* ptr0= ( arg6->IsTypedArray() ) ?
                  (GLvoid*)v8::Local<v8::TypedArray>::Cast(arg6)->Buffer()->GetContents().Data() :
                  nullptr;

    glReadPixels((GLint)NATIVE_INT(arg0),(GLint)NATIVE_INT(arg1),(GLsizei)NATIVE_INT(arg2),(GLsizei)NATIVE_INT(arg3),(GLenum)NATIVE_INT(arg4),(GLenum)NATIVE_INT(arg5),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_ReleaseShaderCompiler (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    glReleaseShaderCompiler();
    GL_ERROR_THROW(isolate);
}

void v8Bind_RenderbufferStorage (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsInt32() || !arg3->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glRenderbufferStorage((GLenum)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1),(GLsizei)NATIVE_INT(arg2),(GLsizei)NATIVE_INT(arg3));
    GL_ERROR_THROW(isolate);
}

void v8Bind_SampleCoverage (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsNumber() || !arg1->IsBoolean()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glSampleCoverage((GLclampf)NATIVE_NUM(arg0),(GLboolean)NATIVE_BOOL(arg1));
    GL_ERROR_THROW(isolate);
}

void v8Bind_Scissor (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsInt32() || !arg3->IsInt32()) {
//    BAD_PARAMETER_TYPE(isolate);
    }

    glScissor((GLint)NATIVE_INT(arg0),(GLint)NATIVE_INT(arg1),(GLsizei)NATIVE_INT(arg2),(GLsizei)NATIVE_INT(arg3));
    GL_ERROR_THROW(isolate);
}

void v8Bind_ShaderBinary (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    v8::Local<v8::Value> arg4= args[4];
    if(!arg0->IsInt32() || !(arg1->IsTypedArray() || arg1->IsNull()) || !arg2->IsInt32() || !(arg3->IsTypedArray() || arg3->IsNull()) || !arg4->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    const GLuint* ptr0= ( arg1->IsTypedArray() ) ?
                        (const GLuint*)v8::Local<v8::TypedArray>::Cast(arg1)->Buffer()->GetContents().Data() :
                        nullptr;
    const GLvoid* ptr1= ( arg3->IsTypedArray() ) ?
                        (const GLvoid*)v8::Local<v8::TypedArray>::Cast(arg3)->Buffer()->GetContents().Data() :
                        nullptr;

    glShaderBinary((GLsizei)NATIVE_INT(arg0),ptr0,(GLenum)NATIVE_INT(arg2),ptr1,(GLsizei)NATIVE_INT(arg4));
    GL_ERROR_THROW(isolate);
}

void v8Bind_ShaderSource (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsString()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    String::Utf8Value srcCode(isolate, arg1);
    std::string code = std::string(*srcCode);

    const char *shader_source = code.c_str();
    glShaderSource((GLuint)NATIVE_INT(arg0), 1, &shader_source, NULL);
    GL_ERROR_THROW(isolate);
}

void v8Bind_StencilFunc (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glStencilFunc((GLenum)NATIVE_INT(arg0),(GLint)NATIVE_INT(arg1),(GLuint)NATIVE_INT(arg2));
    GL_ERROR_THROW(isolate);
}

void v8Bind_StencilFuncSeparate (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsInt32() || !arg3->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glStencilFuncSeparate((GLenum)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1),(GLint)NATIVE_INT(arg2),(GLuint)NATIVE_INT(arg3));
    GL_ERROR_THROW(isolate);
}

void v8Bind_StencilMask (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glStencilMask((GLuint)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
}

void v8Bind_StencilMaskSeparate (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glStencilMaskSeparate((GLenum)NATIVE_INT(arg0),(GLuint)NATIVE_INT(arg1));
    GL_ERROR_THROW(isolate);
}

void v8Bind_StencilOp (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glStencilOp((GLenum)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1),(GLenum)NATIVE_INT(arg2));
    GL_ERROR_THROW(isolate);
}

void v8Bind_StencilOpSeparate (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsInt32() || !arg3->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glStencilOpSeparate((GLenum)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1),(GLenum)NATIVE_INT(arg2),(GLenum)NATIVE_INT(arg3));
    GL_ERROR_THROW(isolate);
}

void v8Bind_TexParameterf (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsNumber()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glTexParameterf((GLenum)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1),(GLfloat)NATIVE_NUM(arg2));
    GL_ERROR_THROW(isolate);
}

void v8Bind_TexParameterfv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !(arg2->IsTypedArray() || arg2->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    const GLfloat* ptr0= ( arg2->IsTypedArray() ) ?
                         (const GLfloat*)v8::Local<v8::TypedArray>::Cast(arg2)->Buffer()->GetContents().Data() :
                         nullptr;

    glTexParameterfv((GLenum)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_TexParameteri (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glTexParameteri((GLenum)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1),(GLint)NATIVE_INT(arg2));
    GL_ERROR_THROW(isolate);
}

void v8Bind_TexParameteriv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !(arg2->IsTypedArray() || arg2->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    const GLint* ptr0= ( arg2->IsTypedArray() ) ?
                       (const GLint*)v8::Local<v8::TypedArray>::Cast(arg2)->Buffer()->GetContents().Data() :
                       nullptr;

    glTexParameteriv((GLenum)NATIVE_INT(arg0),(GLenum)NATIVE_INT(arg1),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_TexSubImage2D (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    v8::Local<v8::Value> arg4= args[4];
    v8::Local<v8::Value> arg5= args[5];
    v8::Local<v8::Value> arg6= args[6];
    v8::Local<v8::Value> arg7= args[7];
    v8::Local<v8::Value> arg8= args[8];

    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsInt32() || !arg3->IsInt32() || !arg4->IsInt32() || !arg5->IsInt32() || !arg6->IsInt32() || !arg7->IsInt32() || !(arg8->IsTypedArray() || arg8->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    const GLvoid* ptr0= ( arg8->IsTypedArray() ) ?
                        (const GLvoid*)v8::Local<v8::TypedArray>::Cast(arg8)->Buffer()->GetContents().Data() :
                        nullptr;

    glTexSubImage2D((GLenum)NATIVE_INT(arg0),(GLint)NATIVE_INT(arg1),(GLint)NATIVE_INT(arg2),(GLint)NATIVE_INT(arg3),(GLsizei)NATIVE_INT(arg4),(GLsizei)NATIVE_INT(arg5),(GLenum)NATIVE_INT(arg6),(GLenum)NATIVE_INT(arg7),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_Uniform1f (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsNumber()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glUniform1f((GLint)NATIVE_INT(arg0),(GLfloat)NATIVE_NUM(arg1));
    GL_ERROR_THROW(isolate);
}

void v8Bind_Uniform1fv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    if ( arg1->IsTypedArray() ) {
        const GLfloat *ptr0 = nullptr;
        v8::Local<v8::TypedArray> ta;
        if (arg1->IsTypedArray()) {
            ta = v8::Local<v8::TypedArray>::Cast(arg1);
            ptr0 = (const GLfloat *) ta->Buffer()->GetContents().Data();
        }

        glUniform1fv((GLint)  NATIVE_INT(arg0), ta->ByteLength(), ptr0);
        GL_ERROR_THROW(isolate);
    } else if ( arg1->IsArray() ) {
        v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast( arg1 );
        glUniform1f(
                (GLint)  NATIVE_INT(arg0),
                (GLfloat)NATIVE_NUM(arr->Get(0))
        );
        GL_ERROR_THROW(isolate);
    } else {
        BAD_PARAMETER_TYPE(isolate);
    }
}

void v8Bind_Uniform1i (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glUniform1i((GLint)NATIVE_INT(arg0),(GLint)NATIVE_INT(arg1));
    GL_ERROR_THROW(isolate);
}

void v8Bind_Uniform1iv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    if ( arg1->IsTypedArray() ) {
        const GLint *ptr0 = nullptr;
        v8::Local<v8::TypedArray> ta;
        if (arg1->IsTypedArray()) {
            ta = v8::Local<v8::TypedArray>::Cast(arg1);
            ptr0 = (const GLint *) ta->Buffer()->GetContents().Data();
        }

        glUniform1iv((GLint)  NATIVE_INT(arg0), ta->ByteLength(), ptr0);
        GL_ERROR_THROW(isolate);
    } else if ( arg1->IsArray() ) {
        v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast( arg1 );
        glUniform1i(
                (GLint)  NATIVE_INT(arg0),
                NATIVE_INT(arr->Get(0))
        );
        GL_ERROR_THROW(isolate);
    } else {
        BAD_PARAMETER_TYPE(isolate);
    }
}

void v8Bind_Uniform2f (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    if(!arg0->IsInt32() || !arg1->IsNumber() || !arg2->IsNumber()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glUniform2f((GLint)NATIVE_INT(arg0),(GLfloat)NATIVE_NUM(arg1),(GLfloat)NATIVE_NUM(arg2));
    GL_ERROR_THROW(isolate);
}

void v8Bind_Uniform2fv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    if ( arg1->IsTypedArray() ) {
        const GLfloat *ptr0 = nullptr;
        v8::Local<v8::TypedArray> ta;
        if (arg1->IsTypedArray()) {
            ta = v8::Local<v8::TypedArray>::Cast(arg1);
            ptr0 = (const GLfloat *) ta->Buffer()->GetContents().Data();
        }

        glUniform2fv((GLint)  NATIVE_INT(arg0), ta->ByteLength(), ptr0);
        GL_ERROR_THROW(isolate);
    } else if ( arg1->IsArray() ) {
        v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast( arg1 );
        glUniform2f(
                (GLint)  NATIVE_INT(arg0),
                (GLfloat)NATIVE_NUM(arr->Get(0)),
                (GLfloat)NATIVE_NUM(arr->Get(1))
        );
        GL_ERROR_THROW(isolate);
    } else {
        BAD_PARAMETER_TYPE(isolate);
    }

}

void v8Bind_Uniform2i (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glUniform2i((GLint)NATIVE_INT(arg0),(GLint)NATIVE_INT(arg1),(GLint)NATIVE_INT(arg2));
    GL_ERROR_THROW(isolate);
}

void v8Bind_Uniform2iv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    if ( arg1->IsTypedArray() ) {
        const GLint *ptr0 = nullptr;
        v8::Local<v8::TypedArray> ta;
        if (arg1->IsTypedArray()) {
            ta = v8::Local<v8::TypedArray>::Cast(arg1);
            ptr0 = (const GLint *) ta->Buffer()->GetContents().Data();
        }

        glUniform2iv((GLint)  NATIVE_INT(arg0), ta->ByteLength(), ptr0);
        GL_ERROR_THROW(isolate);
    } else if ( arg1->IsArray() ) {
        v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast( arg1 );
        glUniform2i(
                (GLint)  NATIVE_INT(arg0),
                NATIVE_INT(arr->Get(0)),
                NATIVE_INT(arr->Get(1))
        );
        GL_ERROR_THROW(isolate);
    } else {
        BAD_PARAMETER_TYPE(isolate);
    }
}

void v8Bind_Uniform3f (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    if(!arg0->IsInt32() || !arg1->IsNumber() || !arg2->IsNumber() || !arg3->IsNumber()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glUniform3f((GLint)NATIVE_INT(arg0),(GLfloat)NATIVE_NUM(arg1),(GLfloat)NATIVE_NUM(arg2),(GLfloat)NATIVE_NUM(arg3));
    GL_ERROR_THROW(isolate);
}

void v8Bind_Uniform3fv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    if ( arg1->IsTypedArray() ) {
        const GLfloat *ptr0 = nullptr;
        v8::Local<v8::TypedArray> ta;
        if (arg1->IsTypedArray()) {
            ta = v8::Local<v8::TypedArray>::Cast(arg1);
            ptr0 = (const GLfloat *) ta->Buffer()->GetContents().Data();
        }

        glUniform3fv((GLint)  NATIVE_INT(arg0), ta->ByteLength(), ptr0);
        GL_ERROR_THROW(isolate);
    } else if ( arg1->IsArray() ) {
        v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast( arg1 );
        glUniform3f(
                (GLint)  NATIVE_INT(arg0),
                (GLfloat)NATIVE_NUM(arr->Get(0)),
                (GLfloat)NATIVE_NUM(arr->Get(1)),
                (GLfloat)NATIVE_NUM(arr->Get(2))
        );
        GL_ERROR_THROW(isolate);
    } else {
        BAD_PARAMETER_TYPE(isolate);
    }
}

void v8Bind_Uniform3i (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsInt32() || !arg3->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glUniform3i((GLint)NATIVE_INT(arg0),(GLint)NATIVE_INT(arg1),(GLint)NATIVE_INT(arg2),(GLint)NATIVE_INT(arg3));
    GL_ERROR_THROW(isolate);
}

void v8Bind_Uniform3iv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    if ( arg1->IsTypedArray() ) {
        const GLint *ptr0 = nullptr;
        v8::Local<v8::TypedArray> ta;
        if (arg1->IsTypedArray()) {
            ta = v8::Local<v8::TypedArray>::Cast(arg1);
            ptr0 = (const GLint *) ta->Buffer()->GetContents().Data();
        }

        glUniform3iv((GLint)  NATIVE_INT(arg0), ta->ByteLength(), ptr0);
        GL_ERROR_THROW(isolate);
    } else if ( arg1->IsArray() ) {
        v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast( arg1 );
        glUniform3i(
                (GLint)  NATIVE_INT(arg0),
                NATIVE_INT(arr->Get(0)),
                NATIVE_INT(arr->Get(1)),
                NATIVE_INT(arr->Get(2))
        );
        GL_ERROR_THROW(isolate);
    } else {
        BAD_PARAMETER_TYPE(isolate);
    }
}

void v8Bind_Uniform4f (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    v8::Local<v8::Value> arg4= args[4];
    if(!arg0->IsInt32() || !arg1->IsNumber() || !arg2->IsNumber() || !arg3->IsNumber() || !arg4->IsNumber()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glUniform4f((GLint)NATIVE_INT(arg0),(GLfloat)NATIVE_NUM(arg1),(GLfloat)NATIVE_NUM(arg2),(GLfloat)NATIVE_NUM(arg3),(GLfloat)NATIVE_NUM(arg4));
    GL_ERROR_THROW(isolate);
}

void v8Bind_Uniform4fv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    if ( arg1->IsTypedArray() ) {
        const GLfloat *ptr0 = nullptr;
        v8::Local<v8::TypedArray> ta;
        if (arg1->IsTypedArray()) {
            ta = v8::Local<v8::TypedArray>::Cast(arg1);
            ptr0 = (const GLfloat *) ta->Buffer()->GetContents().Data();
        }

        glUniform4fv((GLint)  NATIVE_INT(arg0), ta->ByteLength(), ptr0);
        GL_ERROR_THROW(isolate);
    } else if ( arg1->IsArray() ) {
        v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast( arg1 );
        glUniform4f(
                (GLint)  NATIVE_INT(arg0),
                (GLfloat)NATIVE_NUM(arr->Get(0)),
                (GLfloat)NATIVE_NUM(arr->Get(1)),
                (GLfloat)NATIVE_NUM(arr->Get(2)),
                (GLfloat)NATIVE_NUM(arr->Get(3))
        );
        GL_ERROR_THROW(isolate);
    } else {
        BAD_PARAMETER_TYPE(isolate);
    }
}

void v8Bind_Uniform4i (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    v8::Local<v8::Value> arg4= args[4];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsInt32() || !arg3->IsInt32() || !arg4->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glUniform4i((GLint)NATIVE_INT(arg0),(GLint)NATIVE_INT(arg1),(GLint)NATIVE_INT(arg2),(GLint)NATIVE_INT(arg3),(GLint)NATIVE_INT(arg4));
    GL_ERROR_THROW(isolate);
}

void v8Bind_Uniform4iv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    if ( arg1->IsTypedArray() ) {
        const GLint *ptr0 = nullptr;
        v8::Local<v8::TypedArray> ta;
        if (arg1->IsTypedArray()) {
            ta = v8::Local<v8::TypedArray>::Cast(arg1);
            ptr0 = (const GLint *) ta->Buffer()->GetContents().Data();
        }

        glUniform4iv((GLint)  NATIVE_INT(arg0), ta->ByteLength(), ptr0);
        GL_ERROR_THROW(isolate);
    } else if ( arg1->IsArray() ) {
        v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast( arg1 );
        glUniform4i(
                (GLint)  NATIVE_INT(arg0),
                NATIVE_INT(arr->Get(0)),
                NATIVE_INT(arr->Get(1)),
                NATIVE_INT(arr->Get(2)),
                NATIVE_INT(arr->Get(3))
        );
        GL_ERROR_THROW(isolate);
    } else {
        BAD_PARAMETER_TYPE(isolate);
    }
}

void v8Bind_UniformMatrix2fv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];

    v8::Local<v8::Value> arg2= args[1];
    v8::Local<v8::Value> arg3= args[2];
    if(!arg0->IsInt32() || !arg2->IsBoolean() || !(arg3->IsTypedArray() || arg3->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    const GLfloat* ptr0= ( arg3->IsTypedArray() ) ?
                         (const GLfloat*)v8::Local<v8::TypedArray>::Cast(arg3)->Buffer()->GetContents().Data() :
                         nullptr;

    glUniformMatrix2fv((GLint)NATIVE_INT(arg0),1,(GLboolean)NATIVE_BOOL(arg2),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_UniformMatrix3fv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];

    v8::Local<v8::Value> arg2= args[1];
    v8::Local<v8::Value> arg3= args[2];
    if(!arg0->IsInt32()  || !arg2->IsBoolean() || !(arg3->IsTypedArray() || arg3->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    const GLfloat* ptr0= ( arg3->IsTypedArray() ) ?
                         (const GLfloat*)v8::Local<v8::TypedArray>::Cast(arg3)->Buffer()->GetContents().Data() :
                         nullptr;

    glUniformMatrix3fv((GLint)NATIVE_INT(arg0),1,(GLboolean)NATIVE_BOOL(arg2),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_UniformMatrix4fv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];

    v8::Local<v8::Value> arg2= args[1];
    v8::Local<v8::Value> arg3= args[2];
    if(!arg0->IsInt32() || !arg2->IsBoolean() || !(arg3->IsTypedArray() || arg3->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    const GLfloat* ptr0= ( arg3->IsTypedArray() ) ?
                         (const GLfloat*)v8::Local<v8::TypedArray>::Cast(arg3)->Buffer()->GetContents().Data() :
                         nullptr;

    glUniformMatrix4fv((GLint)NATIVE_INT(arg0),1,(GLboolean)NATIVE_BOOL(arg2),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_UseProgram (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glUseProgram((GLuint)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
}

void v8Bind_ValidateProgram (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glValidateProgram((GLuint)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
}

void v8Bind_VertexAttrib1f (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsNumber()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glVertexAttrib1f((GLuint)NATIVE_INT(arg0),(GLfloat)NATIVE_NUM(arg1));
    GL_ERROR_THROW(isolate);
}

void v8Bind_VertexAttrib1fv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !(arg1->IsTypedArray() || arg1->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    const GLfloat* ptr0= ( arg1->IsTypedArray() ) ?
                         (const GLfloat*)v8::Local<v8::TypedArray>::Cast(arg1)->Buffer()->GetContents().Data() :
                         nullptr;

    glVertexAttrib1fv((GLuint)NATIVE_INT(arg0),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_VertexAttrib2f (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    if(!arg0->IsInt32() || !arg1->IsNumber() || !arg2->IsNumber()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glVertexAttrib2f((GLuint)NATIVE_INT(arg0),(GLfloat)NATIVE_NUM(arg1),(GLfloat)NATIVE_NUM(arg2));
    GL_ERROR_THROW(isolate);
}

void v8Bind_VertexAttrib2fv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !(arg1->IsTypedArray() || arg1->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    const GLfloat* ptr0= ( arg1->IsTypedArray() ) ?
                         (const GLfloat*)v8::Local<v8::TypedArray>::Cast(arg1)->Buffer()->GetContents().Data() :
                         nullptr;

    glVertexAttrib2fv((GLuint)NATIVE_INT(arg0),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_VertexAttrib3f (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    if(!arg0->IsInt32() || !arg1->IsNumber() || !arg2->IsNumber() || !arg3->IsNumber()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glVertexAttrib3f((GLuint)NATIVE_INT(arg0),(GLfloat)NATIVE_NUM(arg1),(GLfloat)NATIVE_NUM(arg2),(GLfloat)NATIVE_NUM(arg3));
    GL_ERROR_THROW(isolate);
}

void v8Bind_VertexAttrib3fv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !(arg1->IsTypedArray() || arg1->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    const GLfloat* ptr0= ( arg1->IsTypedArray() ) ?
                         (const GLfloat*)v8::Local<v8::TypedArray>::Cast(arg1)->Buffer()->GetContents().Data() :
                         nullptr;

    glVertexAttrib3fv((GLuint)NATIVE_INT(arg0),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_VertexAttrib4f (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    v8::Local<v8::Value> arg4= args[4];
    if(!arg0->IsInt32() || !arg1->IsNumber() || !arg2->IsNumber() || !arg3->IsNumber() || !arg4->IsNumber()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glVertexAttrib4f((GLuint)NATIVE_INT(arg0),(GLfloat)NATIVE_NUM(arg1),(GLfloat)NATIVE_NUM(arg2),(GLfloat)NATIVE_NUM(arg3),(GLfloat)NATIVE_NUM(arg4));
    GL_ERROR_THROW(isolate);
}

void v8Bind_VertexAttrib4fv (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !(arg1->IsTypedArray() || arg1->IsNull())) {
        BAD_PARAMETER_TYPE(isolate);
    }

    const GLfloat* ptr0= ( arg1->IsTypedArray() ) ?
                         (const GLfloat*)v8::Local<v8::TypedArray>::Cast(arg1)->Buffer()->GetContents().Data() :
                         nullptr;

    glVertexAttrib4fv((GLuint)NATIVE_INT(arg0),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_VertexAttribPointer (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    v8::Local<v8::Value> arg4= args[4];
    v8::Local<v8::Value> arg5= args[5];
    if(!arg0->IsInt32() || !arg1->IsInt32() || !arg2->IsInt32() || !arg3->IsBoolean() || !arg4->IsInt32() || !arg5->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glVertexAttribPointer((GLuint)NATIVE_INT(arg0),(GLint)NATIVE_INT(arg1),(GLenum)NATIVE_INT(arg2),(GLboolean)NATIVE_BOOL(arg3),(GLsizei)NATIVE_INT(arg4),(GLvoid*)NATIVE_INT(arg5));
    GL_ERROR_THROW(isolate);
}

void v8Bind_Viewport (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];

    glViewport((GLint)NATIVE_INT(arg0),(GLint)NATIVE_INT(arg1),(GLsizei)NATIVE_INT(arg2),(GLsizei)NATIVE_INT(arg3));
    GL_ERROR_THROW(isolate);
}

void v8Bind_GetExtension (const v8::FunctionCallbackInfo<v8::Value>& args) {

//  v8::Isolate* isolate = args.GetIsolate();
    args.GetReturnValue().SetNull();
}

void v8Bind_GetParameter (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    v8::HandleScope hs(isolate);

    v8::Local<v8::Value> arg0= args[0];

    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLenum param = (GLenum)NATIVE_INT(arg0);

    switch( param ) {
        // bool
        case GL_BLEND:
        case GL_CULL_FACE:
        case GL_DEPTH_TEST:
        case GL_DEPTH_WRITEMASK:
        case GL_DITHER:
        case GL_POLYGON_OFFSET_FILL:
        case GL_SAMPLE_COVERAGE_INVERT:
        case GL_SCISSOR_TEST:
        case GL_STENCIL_TEST:
//        case GL_UNPACK_FLIP_Y_WEBGL:
//        case GL_UNPACK_PREMULTIPLY_ALPHA_WEBGL:
        {
            GLboolean ret;
            glGetBooleanv(param, &ret);
            args.GetReturnValue().Set(ret);
        }
            break;

            // float
        case GL_DEPTH_CLEAR_VALUE:
        case GL_LINE_WIDTH:
        case GL_POLYGON_OFFSET_FACTOR:
        case GL_POLYGON_OFFSET_UNITS:
        case GL_SAMPLE_COVERAGE_VALUE:
        {
            GLfloat ret;
            glGetFloatv(param, &ret);
            args.GetReturnValue().Set((double)ret);
        }
            break;

            // uint
        case GL_STENCIL_VALUE_MASK:
        case GL_STENCIL_WRITEMASK:
        case GL_STENCIL_BACK_VALUE_MASK:
        case GL_STENCIL_BACK_WRITEMASK: {
            GLint ret;
            glGetIntegerv(param, &ret);
            args.GetReturnValue().Set((uint32_t)ret);
        }
            break;

            // int32array[2]
        case GL_MAX_VIEWPORT_DIMS: {
            GLint ret[2] = { 0,0 };
            glGetIntegerv( param, ret );

            v8::Local<v8::ArrayBuffer> ab = v8::ArrayBuffer::New(isolate,sizeof(uint32_t)*2);
            v8::Local<v8::Int32Array> fab = v8::Int32Array::New(ab, 0, 2);
            fab->Set( 0, v8::Int32::New(isolate, ret[0] ) );
            fab->Set( 1, v8::Int32::New(isolate, ret[1] ) );

            args.GetReturnValue().Set( fab );
        }
            break;

            // int32array[4]
        case GL_SCISSOR_BOX:
        case GL_VIEWPORT:{
            GLint ret[4] = { 0,0,0,0 };
            glGetIntegerv( param, ret );

            v8::Local<v8::ArrayBuffer> ab = v8::ArrayBuffer::New(isolate,sizeof(uint32_t)*4);
            v8::Local<v8::Int32Array> fab = v8::Int32Array::New(ab, 0, 4);
            fab->Set( 0, v8::Int32::New(isolate, ret[0] ) );
            fab->Set( 1, v8::Int32::New(isolate, ret[1] ) );
            fab->Set( 2, v8::Int32::New(isolate, ret[2] ) );
            fab->Set( 3, v8::Int32::New(isolate, ret[3] ) );

            args.GetReturnValue().Set( fab );

        }
            break;

            // float32array[2]
        case GL_ALIASED_LINE_WIDTH_RANGE:
        case GL_ALIASED_POINT_SIZE_RANGE:
        case GL_DEPTH_RANGE: {
            GLfloat ret[2] = { 0,0 };
            glGetFloatv( param, ret );

            v8::Local<v8::ArrayBuffer> ab = v8::ArrayBuffer::New(isolate,sizeof(float)*2);
            v8::Local<v8::Float32Array> fab = v8::Float32Array::New(ab, 0, 2);
            fab->Set( 0, Number::New(isolate, ret[0] ) );
            fab->Set( 1, Number::New(isolate, ret[1] ) );

            args.GetReturnValue().Set( fab );

        }
            break;

            // float32arrya[4]
        case GL_BLEND_COLOR:
        case GL_COLOR_CLEAR_VALUE: {
            GLfloat ret[4] = {0, 0, 0, 0};
            glGetFloatv(param, ret);

            v8::Local<v8::ArrayBuffer> ab = v8::ArrayBuffer::New(isolate, sizeof(float) * 4);
            v8::Local<v8::Float32Array> fab = v8::Float32Array::New(ab, 0, 4);
            fab->Set(0, Number::New(isolate, ret[0]));
            fab->Set(1, Number::New(isolate, ret[1]));
            fab->Set(2, Number::New(isolate, ret[2]));
            fab->Set(3, Number::New(isolate, ret[3]));

            args.GetReturnValue().Set(fab);
        }
            break;

            // bool[4]
        case GL_COLOR_WRITEMASK: {
            GLboolean ret[4] = {GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE};
            glGetBooleanv(param, ret);

            v8::Local<v8::Array> ab = v8::Array::New(isolate,4);
            ab->Set(0, Number::New(isolate, ret[0]));
            ab->Set(1, Number::New(isolate, ret[1]));
            ab->Set(2, Number::New(isolate, ret[2]));
            ab->Set(3, Number::New(isolate, ret[3]));

            args.GetReturnValue().Set(ab);
        }
            break;

            // int / enum
//        case UNPACK_COLORSPACE_CONVERSION_WEBGL:
        case GL_UNPACK_ALIGNMENT:
        case GL_STENCIL_BACK_FAIL:
        case GL_STENCIL_BACK_FUNC:
        case GL_STENCIL_BACK_PASS_DEPTH_FAIL:
        case GL_STENCIL_BACK_PASS_DEPTH_PASS:
        case GL_STENCIL_BACK_REF:
        case GL_STENCIL_BITS:
        case GL_STENCIL_CLEAR_VALUE:
        case GL_STENCIL_FAIL:
        case GL_STENCIL_FUNC:
        case GL_STENCIL_PASS_DEPTH_FAIL:
        case GL_STENCIL_PASS_DEPTH_PASS:
        case GL_STENCIL_REF:
        case GL_SAMPLES:
        case GL_RED_BITS:
        case GL_SAMPLE_BUFFERS:
        case GL_MAX_CUBE_MAP_TEXTURE_SIZE:
        case GL_MAX_FRAGMENT_UNIFORM_VECTORS:
        case GL_MAX_RENDERBUFFER_SIZE:
        case GL_MAX_TEXTURE_IMAGE_UNITS:
        case GL_MAX_TEXTURE_SIZE:
        case GL_MAX_VARYING_VECTORS:
        case GL_MAX_VERTEX_ATTRIBS:
        case GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS:
        case GL_MAX_VERTEX_UNIFORM_VECTORS:
        case GL_PACK_ALIGNMENT:
        case GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS:
        case GL_IMPLEMENTATION_COLOR_READ_FORMAT:
        case GL_IMPLEMENTATION_COLOR_READ_TYPE:
        case GL_GREEN_BITS:
        case GL_GENERATE_MIPMAP_HINT:
        case GL_FRONT_FACE:
        case GL_DEPTH_FUNC:
        case GL_DEPTH_BITS:
        case GL_CULL_FACE_MODE:
        case GL_ACTIVE_TEXTURE:
        case GL_ALPHA_BITS:
        case GL_BLEND_DST_ALPHA:
        case GL_BLEND_DST_RGB:
        case GL_BLEND_EQUATION_ALPHA:
        case GL_BLEND_EQUATION_RGB:
        case GL_BLEND_SRC_ALPHA:
        case GL_BLEND_SRC_RGB:
        case GL_BLUE_BITS:
//            // glprogram
        case GL_CURRENT_PROGRAM:
//            // webgl render buffer
        case GL_RENDERBUFFER_BINDING:
            // framebuffer
        case GL_FRAMEBUFFER_BINDING: {
            GLint ret;
            glGetIntegerv(param, &ret);
            args.GetReturnValue().Set(ret);
        }
            break;

        case GL_VERSION: {
            const GLubyte* v= glGetString(param);
            args.GetReturnValue().Set( JS_STR("WebGL 2.0 (ActiveTheory)") );
        }

        case GL_RENDERER:
        case GL_SHADING_LANGUAGE_VERSION:
        case GL_VENDOR:
        case GL_EXTENSIONS:
        {
            // return a string
            char *params = (char *)glGetString(param);

            if (params != NULL) {
                args.GetReturnValue().Set( JS_STR(params) );
            } else {
                args.GetReturnValue().SetNull();
            }
        }
            break;


//
//            // webgl texture
//        case GL_TEXTURE_BINDING_2D:
//        case GL_TEXTURE_BINDING_CUBE_MAP:
//            break;
//
//            // webglbuffer
//        case GL_ARRAY_BUFFER_BINDING:
//        case GL_ELEMENT_ARRAY_BUFFER_BINDING:
//            break;
//
//

//            break;
//

        default:
            isolate->ThrowException( v8::Exception::Error( JS_STR("Unknown getParam value")) );
    }

    GL_ERROR_THROW(isolate);
}

void AURA_TexImage2DNull (const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    Isolate::Scope iscope( isolate );
    HandleScope hs( isolate );
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    v8::Local<v8::Value> arg4= args[4];
    v8::Local<v8::Value> arg5= args[5];
    v8::Local<v8::Value> arg6= args[6];
    v8::Local<v8::Value> arg7= args[7];
    int p7 =  NATIVE_INT(arg7);

    glTexImage2D((GLenum)  NATIVE_INT(arg0), (GLint)  NATIVE_INT(arg1), (GLint)  NATIVE_INT(arg2),
                 (GLsizei)  NATIVE_INT(arg3), (GLsizei)  NATIVE_INT(arg4),
                 (GLint)  NATIVE_INT(arg5), (GLenum)  NATIVE_INT(arg6), (GLenum)  NATIVE_INT(arg7), NULL);

    GL_ERROR_THROW(isolate);
}

void AURA_TexImage2DArray (const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    Isolate::Scope iscope( isolate );
    HandleScope hs( isolate );
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    v8::Local<v8::Value> arg4= args[4];
    v8::Local<v8::Value> arg5= args[5];
    v8::Local<v8::Value> arg6= args[6];
    v8::Local<v8::Value> arg7= args[7];
    v8::Local<v8::Value> arg8= args[8];

    int p2 =  NATIVE_INT(arg2);
    int p7 =  NATIVE_INT(arg7);

    const GLvoid* ptr0= ( arg8->IsTypedArray() ) ?
                        (const GLvoid*)v8::Local<v8::TypedArray>::Cast(arg8)->Buffer()->GetContents().Data() :
                        nullptr;

    glTexImage2D((GLenum)NATIVE_INT(arg0),(GLint)NATIVE_INT(arg1),(GLint)p2,(GLsizei)NATIVE_INT(arg3),(GLsizei)NATIVE_INT(arg4),(GLint)NATIVE_INT(arg5),(GLenum)NATIVE_INT(arg6),(GLenum)p7,ptr0);
    GL_ERROR_THROW(isolate);
}

void AURA_GetExtensions (const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    Isolate::Scope iscope( isolate );
    HandleScope hs( isolate );
    v8::Local<v8::Value> arg0= args[0];

    char *extensionString = (char*) glGetString(GL_EXTENSIONS);

    args.GetReturnValue().Set(JS_STR(extensionString));
    GL_ERROR_THROW(isolate);
}

void AURA_DrawElementsInstanced (const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    Isolate::Scope iscope( isolate );
    HandleScope hs( isolate );
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    v8::Local<v8::Value> arg4= args[4];

    void * vv = reinterpret_cast<void*>(NATIVE_INT(arg3));
    glDrawElementsInstanced((GLenum)NATIVE_INT(arg0), (GLsizei)NATIVE_INT(arg1), (GLenum)NATIVE_INT(arg2), vv, (GLsizei)NATIVE_INT(arg4));
    GL_ERROR_THROW(isolate);
}

void AURA_DrawArraysInstanced (const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    Isolate::Scope iscope( isolate );
    HandleScope hs( isolate );
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];

    glDrawArraysInstanced((GLenum)NATIVE_INT(arg0), (GLint)NATIVE_INT(arg1), (GLsizei)NATIVE_INT(arg2), (GLsizei)NATIVE_INT(arg3));
    GL_ERROR_THROW(isolate);
}

void AURA_VertexAttribDivisor (const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    Isolate::Scope iscope( isolate );
    HandleScope hs( isolate );
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];

    glVertexAttribDivisor((GLuint)NATIVE_INT(arg0), (GLuint)NATIVE_INT(arg1));
    GL_ERROR_THROW(isolate);
}

void AURA_GetUniformBlockIndex (const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    Isolate::Scope iscope( isolate );
    HandleScope hs( isolate );
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];

    String::Utf8Value stringValue(isolate, arg1);
    std::string src = *stringValue;

    GLuint id = glGetUniformBlockIndex((GLuint)NATIVE_INT(arg0), src.c_str());
    args.GetReturnValue().Set(Number::New(isolate, id));
    GL_ERROR_THROW(isolate);
}

void AURA_UniformBlockBinding (const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    Isolate::Scope iscope( isolate );
    HandleScope hs( isolate );
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];

    glUniformBlockBinding((GLuint)NATIVE_INT(arg0), (GLuint)NATIVE_INT(arg1), (GLuint)NATIVE_INT(arg2));
    GL_ERROR_THROW(isolate);
}

void AURA_BindBufferBase (const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    Isolate::Scope iscope( isolate );
    HandleScope hs( isolate );
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];

    glBindBufferBase((GLenum)NATIVE_INT(arg0), (GLuint)NATIVE_INT(arg1), (GLuint)NATIVE_INT(arg2));
    GL_ERROR_THROW(isolate);
}

void AURA_CreateVertexArray (const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    Isolate::Scope iscope( isolate );
    HandleScope hs( isolate );

    GLuint id;
    glGenVertexArrays(1, &id);

    args.GetReturnValue().Set(Number::New(isolate, id));
    GL_ERROR_THROW(isolate);
}

void AURA_BindVertexArray (const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    Isolate::Scope iscope( isolate );
    HandleScope hs( isolate );
    v8::Local<v8::Value> arg0= args[0];

    glBindVertexArray((GLuint)NATIVE_INT(arg0));
    GL_ERROR_THROW(isolate);
}

void AURA_DeleteVertexArray (const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    Isolate::Scope iscope( isolate );
    HandleScope hs( isolate );
    v8::Local<v8::Value> arg0= args[0];

    GLuint params = (GLuint)NATIVE_INT(arg0);
    glDeleteVertexArrays(1, &params);
    GL_ERROR_THROW(isolate);
}

void AURA_DrawBuffers (const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate *isolate = args.GetIsolate();
    Isolate::Scope iscope(isolate);
    HandleScope hs(isolate);

    Local<Array> buffersArray = Local<Array>::Cast(args[0]);

    GLenum buffers[32];
    size_t numBuffers = std::min<size_t>(buffersArray->Length(), sizeof(buffers)/sizeof(buffers[0]));
    for (size_t i = 0; i < numBuffers; i++) {
        buffers[i] = NATIVE_UINT(buffersArray->Get(i));
    }

    glDrawBuffers(numBuffers, buffers);
    GL_ERROR_THROW(isolate);
}

void AURA_BlitFramebuffer (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];
    v8::Local<v8::Value> arg4= args[4];
    v8::Local<v8::Value> arg5= args[5];
    v8::Local<v8::Value> arg6= args[6];
    v8::Local<v8::Value> arg7= args[7];
    v8::Local<v8::Value> arg8= args[8];
    v8::Local<v8::Value> arg9= args[9];

    GLuint x0 = (GLenum)NATIVE_INT(arg0);
    GLuint y0 = (GLenum)NATIVE_INT(arg1);
    GLuint w0 = (GLenum)NATIVE_INT(arg2);
    GLuint h0 = (GLenum)NATIVE_INT(arg3);
    GLuint x1 = (GLenum)NATIVE_INT(arg4);
    GLuint y1 = (GLenum)NATIVE_INT(arg5);
    GLuint w1 = (GLenum)NATIVE_INT(arg6);
    GLuint h1 = (GLenum)NATIVE_INT(arg7);
    GLbitfield target = (GLenum)NATIVE_INT(arg8);
    GLenum type = (GLenum)NATIVE_INT(arg9);

    glBlitFramebuffer(x0, y0, w0, h0, x1, y1, w1, h1, target, type);
    GL_ERROR_THROW(isolate);
}

void initializeAura(V8Runtime* runtime, Local<v8::Context> context, Local<Object> gl) {
    v8::Isolate* isolate = runtime->isolate;
    isolate_ = isolate;
    context_ = context;

    gl->Set(context, JS_STR("DEPTH_BUFFER_BIT"), JS_INT(0x00000100));
    gl->Set(context, JS_STR("STENCIL_BUFFER_BIT"), JS_INT(0x00000400));
    gl->Set(context, JS_STR("COLOR_BUFFER_BIT"), JS_INT(0x00004000));
    gl->Set(context, JS_STR("FALSE"), JS_INT(0));
    gl->Set(context, JS_STR("TRUE"), JS_INT(1));
    gl->Set(context, JS_STR("POINTS"), JS_INT(0x0000));
    gl->Set(context, JS_STR("LINES"), JS_INT(0x0001));
    gl->Set(context, JS_STR("LINE_LOOP"), JS_INT(0x0002));
    gl->Set(context, JS_STR("LINE_STRIP"), JS_INT(0x0003));
    gl->Set(context, JS_STR("TRIANGLES"), JS_INT(0x0004));
    gl->Set(context, JS_STR("TRIANGLE_STRIP"), JS_INT(0x0005));
    gl->Set(context, JS_STR("TRIANGLE_FAN"), JS_INT(0x0006));
    gl->Set(context, JS_STR("ZERO"), JS_INT(0));
    gl->Set(context, JS_STR("ONE"), JS_INT(1));
    gl->Set(context, JS_STR("SRC_COLOR"), JS_INT(0x0300));
    gl->Set(context, JS_STR("ONE_MINUS_SRC_COLOR"), JS_INT(0x0301));
    gl->Set(context, JS_STR("SRC_ALPHA"), JS_INT(0x0302));
    gl->Set(context, JS_STR("ONE_MINUS_SRC_ALPHA"), JS_INT(0x0303));
    gl->Set(context, JS_STR("DST_ALPHA"), JS_INT(0x0304));
    gl->Set(context, JS_STR("ONE_MINUS_DST_ALPHA"), JS_INT(0x0305));
    gl->Set(context, JS_STR("DST_COLOR"), JS_INT(0x0306));
    gl->Set(context, JS_STR("ONE_MINUS_DST_COLOR"), JS_INT(0x0307));
    gl->Set(context, JS_STR("SRC_ALPHA_SATURATE"), JS_INT(0x0308));
    gl->Set(context, JS_STR("FUNC_ADD"), JS_INT(0x8006));
    gl->Set(context, JS_STR("BLEND_EQUATION"), JS_INT(0x8009));
    gl->Set(context, JS_STR("BLEND_EQUATION_RGB"), JS_INT(0x8009));
    gl->Set(context, JS_STR("BLEND_EQUATION_ALPHA"), JS_INT(0x883D));
    gl->Set(context, JS_STR("FUNC_SUBTRACT"), JS_INT(0x800A));
    gl->Set(context, JS_STR("FUNC_REVERSE_SUBTRACT"), JS_INT(0x800B));
    gl->Set(context, JS_STR("BLEND_DST_RGB"), JS_INT(0x80C8));
    gl->Set(context, JS_STR("BLEND_SRC_RGB"), JS_INT(0x80C9));
    gl->Set(context, JS_STR("BLEND_DST_ALPHA"), JS_INT(0x80CA));
    gl->Set(context, JS_STR("BLEND_SRC_ALPHA"), JS_INT(0x80CB));
    gl->Set(context, JS_STR("CONSTANT_COLOR"), JS_INT(0x8001));
    gl->Set(context, JS_STR("ONE_MINUS_CONSTANT_COLOR"), JS_INT(0x8002));
    gl->Set(context, JS_STR("CONSTANT_ALPHA"), JS_INT(0x8003));
    gl->Set(context, JS_STR("ONE_MINUS_CONSTANT_ALPHA"), JS_INT(0x8004));
    gl->Set(context, JS_STR("BLEND_COLOR"), JS_INT(0x8005));
    gl->Set(context, JS_STR("ARRAY_BUFFER"), JS_INT(0x8892));
    gl->Set(context, JS_STR("ELEMENT_ARRAY_BUFFER"), JS_INT(0x8893));
    gl->Set(context, JS_STR("ARRAY_BUFFER_BINDING"), JS_INT(0x8894));
    gl->Set(context, JS_STR("ELEMENT_ARRAY_BUFFER_BINDING"), JS_INT(0x8895));
    gl->Set(context, JS_STR("STREAM_DRAW"), JS_INT(0x88E0));
    gl->Set(context, JS_STR("STATIC_DRAW"), JS_INT(0x88E4));
    gl->Set(context, JS_STR("DYNAMIC_DRAW"), JS_INT(0x88E8));
    gl->Set(context, JS_STR("BUFFER_SIZE"), JS_INT(0x8764));
    gl->Set(context, JS_STR("BUFFER_USAGE"), JS_INT(0x8765));
    gl->Set(context, JS_STR("CURRENT_VERTEX_ATTRIB"), JS_INT(0x8626));
    gl->Set(context, JS_STR("FRONT"), JS_INT(0x0404));
    gl->Set(context, JS_STR("BACK"), JS_INT(0x0405));
    gl->Set(context, JS_STR("FRONT_AND_BACK"), JS_INT(0x0408));
    gl->Set(context, JS_STR("TEXTURE_2D"), JS_INT(0x0DE1));
    gl->Set(context, JS_STR("CULL_FACE"), JS_INT(0x0B44));
    gl->Set(context, JS_STR("BLEND"), JS_INT(0x0BE2));
    gl->Set(context, JS_STR("DITHER"), JS_INT(0x0BD0));
    gl->Set(context, JS_STR("STENCIL_TEST"), JS_INT(0x0B90));
    gl->Set(context, JS_STR("DEPTH_TEST"), JS_INT(0x0B71));
    gl->Set(context, JS_STR("SCISSOR_TEST"), JS_INT(0x0C11));
    gl->Set(context, JS_STR("POLYGON_OFFSET_FILL"), JS_INT(0x8037));
    gl->Set(context, JS_STR("SAMPLE_ALPHA_TO_COVERAGE"), JS_INT(0x809E));
    gl->Set(context, JS_STR("SAMPLE_COVERAGE"), JS_INT(0x80A0));
    gl->Set(context, JS_STR("NO_ERROR"), JS_INT(0));
    gl->Set(context, JS_STR("INVALID_ENUM"), JS_INT(0x0500));
    gl->Set(context, JS_STR("INVALID_VALUE"), JS_INT(0x0501));
    gl->Set(context, JS_STR("INVALID_OPERATION"), JS_INT(0x0502));
    gl->Set(context, JS_STR("OUT_OF_MEMORY"), JS_INT(0x0505));
    gl->Set(context, JS_STR("CW"), JS_INT(0x0900));
    gl->Set(context, JS_STR("CCW"), JS_INT(0x0901));
    gl->Set(context, JS_STR("LINE_WIDTH"), JS_INT(0x0B21));
    gl->Set(context, JS_STR("ALIASED_POINT_SIZE_RANGE"), JS_INT(0x846D));
    gl->Set(context, JS_STR("ALIASED_LINE_WIDTH_RANGE"), JS_INT(0x846E));
    gl->Set(context, JS_STR("CULL_FACE_MODE"), JS_INT(0x0B45));
    gl->Set(context, JS_STR("FRONT_FACE"), JS_INT(0x0B46));
    gl->Set(context, JS_STR("DEPTH_RANGE"), JS_INT(0x0B70));
    gl->Set(context, JS_STR("DEPTH_WRITEMASK"), JS_INT(0x0B72));
    gl->Set(context, JS_STR("DEPTH_CLEAR_VALUE"), JS_INT(0x0B73));
    gl->Set(context, JS_STR("DEPTH_FUNC"), JS_INT(0x0B74));
    gl->Set(context, JS_STR("STENCIL_CLEAR_VALUE"), JS_INT(0x0B91));
    gl->Set(context, JS_STR("STENCIL_FUNC"), JS_INT(0x0B92));
    gl->Set(context, JS_STR("STENCIL_FAIL"), JS_INT(0x0B94));
    gl->Set(context, JS_STR("STENCIL_PASS_DEPTH_FAIL"), JS_INT(0x0B95));
    gl->Set(context, JS_STR("STENCIL_PASS_DEPTH_PASS"), JS_INT(0x0B96));
    gl->Set(context, JS_STR("STENCIL_REF"), JS_INT(0x0B97));
    gl->Set(context, JS_STR("STENCIL_VALUE_MASK"), JS_INT(0x0B93));
    gl->Set(context, JS_STR("STENCIL_WRITEMASK"), JS_INT(0x0B98));
    gl->Set(context, JS_STR("STENCIL_BACK_FUNC"), JS_INT(0x8800));
    gl->Set(context, JS_STR("STENCIL_BACK_FAIL"), JS_INT(0x8801));
    gl->Set(context, JS_STR("STENCIL_BACK_PASS_DEPTH_FAIL"), JS_INT(0x8802));
    gl->Set(context, JS_STR("STENCIL_BACK_PASS_DEPTH_PASS"), JS_INT(0x8803));
    gl->Set(context, JS_STR("STENCIL_BACK_REF"), JS_INT(0x8CA3));
    gl->Set(context, JS_STR("STENCIL_BACK_VALUE_MASK"), JS_INT(0x8CA4));
    gl->Set(context, JS_STR("STENCIL_BACK_WRITEMASK"), JS_INT(0x8CA5));
    gl->Set(context, JS_STR("VIEWPORT"), JS_INT(0x0BA2));
    gl->Set(context, JS_STR("SCISSOR_BOX"), JS_INT(0x0C10));
    gl->Set(context, JS_STR("COLOR_CLEAR_VALUE"), JS_INT(0x0C22));
    gl->Set(context, JS_STR("COLOR_WRITEMASK"), JS_INT(0x0C23));
    gl->Set(context, JS_STR("UNPACK_ALIGNMENT"), JS_INT(GL_UNPACK_ALIGNMENT));
    gl->Set(context, JS_STR("UNPACK_FLIP_Y_WEBGL"), JS_INT(0x9240));
    gl->Set(context, JS_STR("UNPACK_PREMULTIPLY_ALPHA_WEBGL"), JS_INT(0x9241));
    gl->Set(context, JS_STR("PACK_ALIGNMENT"), JS_INT(0x0D05));
    gl->Set(context, JS_STR("MAX_TEXTURE_SIZE"), JS_INT(0x0D33));
    gl->Set(context, JS_STR("MAX_VIEWPORT_DIMS"), JS_INT(0x0D3A));
    gl->Set(context, JS_STR("SUBPIXEL_BITS"), JS_INT(0x0D50));
    gl->Set(context, JS_STR("RED_BITS"), JS_INT(0x0D52));
    gl->Set(context, JS_STR("GREEN_BITS"), JS_INT(0x0D53));
    gl->Set(context, JS_STR("BLUE_BITS"), JS_INT(0x0D54));
    gl->Set(context, JS_STR("ALPHA_BITS"), JS_INT(0x0D55));
    gl->Set(context, JS_STR("DEPTH_BITS"), JS_INT(0x0D56));
    gl->Set(context, JS_STR("STENCIL_BITS"), JS_INT(0x0D57));
    gl->Set(context, JS_STR("POLYGON_OFFSET_UNITS"), JS_INT(0x2A00));
    gl->Set(context, JS_STR("POLYGON_OFFSET_FACTOR"), JS_INT(0x8038));
    gl->Set(context, JS_STR("TEXTURE_BINDING_2D"), JS_INT(0x8069));
    gl->Set(context, JS_STR("SAMPLE_BUFFERS"), JS_INT(0x80A8));
    gl->Set(context, JS_STR("SAMPLES"), JS_INT(0x80A9));
    gl->Set(context, JS_STR("SAMPLE_COVERAGE_VALUE"), JS_INT(0x80AA));
    gl->Set(context, JS_STR("SAMPLE_COVERAGE_INVERT"), JS_INT(0x80AB));
    gl->Set(context, JS_STR("NUM_COMPRESSED_TEXTURE_FORMATS"), JS_INT(0x86A2));
    gl->Set(context, JS_STR("COMPRESSED_TEXTURE_FORMATS"), JS_INT(0x86A3));
    gl->Set(context, JS_STR("DONT_CARE"), JS_INT(0x1100));
    gl->Set(context, JS_STR("FASTEST"), JS_INT(0x1101));
    gl->Set(context, JS_STR("NICEST"), JS_INT(0x1102));
    gl->Set(context, JS_STR("GENERATE_MIPMAP_HINT"), JS_INT(0x8192));
    gl->Set(context, JS_STR("BYTE"), JS_INT(0x1400));
    gl->Set(context, JS_STR("UNSIGNED_BYTE"), JS_INT(0x1401));
    gl->Set(context, JS_STR("SHORT"), JS_INT(0x1402));
    gl->Set(context, JS_STR("UNSIGNED_SHORT"), JS_INT(0x1403));
    gl->Set(context, JS_STR("INT"), JS_INT(0x1404));
    gl->Set(context, JS_STR("UNSIGNED_INT"), JS_INT(0x1405));
    gl->Set(context, JS_STR("FLOAT"), JS_INT(0x1406));
    gl->Set(context, JS_STR("FIXED"), JS_INT(0x140C));
    gl->Set(context, JS_STR("DEPTH_COMPONENT"), JS_INT(0x1902));
    gl->Set(context, JS_STR("ALPHA"), JS_INT(0x1906));
    gl->Set(context, JS_STR("RGB"), JS_INT(0x1907));
    gl->Set(context, JS_STR("RGBA"), JS_INT(0x1908));
    gl->Set(context, JS_STR("LUMINANCE"), JS_INT(0x1909));
    gl->Set(context, JS_STR("LUMINANCE_ALPHA"), JS_INT(0x190A));
    gl->Set(context, JS_STR("UNSIGNED_SHORT_4_4_4_4"), JS_INT(0x8033));
    gl->Set(context, JS_STR("UNSIGNED_SHORT_5_5_5_1"), JS_INT(0x8034));
    gl->Set(context, JS_STR("UNSIGNED_SHORT_5_6_5"), JS_INT(0x8363));
    gl->Set(context, JS_STR("FRAGMENT_SHADER"), JS_INT(0x8B30));
    gl->Set(context, JS_STR("VERTEX_SHADER"), JS_INT(0x8B31));
    gl->Set(context, JS_STR("MAX_VERTEX_ATTRIBS"), JS_INT(0x8869));
    gl->Set(context, JS_STR("MAX_VERTEX_UNIFORM_VECTORS"), JS_INT(0x8DFB));
    gl->Set(context, JS_STR("MAX_VARYING_VECTORS"), JS_INT(0x8DFC));
    gl->Set(context, JS_STR("MAX_COMBINED_TEXTURE_IMAGE_UNITS"), JS_INT(0x8B4D));
    gl->Set(context, JS_STR("MAX_VERTEX_TEXTURE_IMAGE_UNITS"), JS_INT(0x8B4C));
    gl->Set(context, JS_STR("MAX_TEXTURE_IMAGE_UNITS"), JS_INT(0x8872));
    gl->Set(context, JS_STR("MAX_FRAGMENT_UNIFORM_VECTORS"), JS_INT(0x8DFD));
    gl->Set(context, JS_STR("SHADER_TYPE"), JS_INT(0x8B4F));
    gl->Set(context, JS_STR("DELETE_STATUS"), JS_INT(0x8B80));
    gl->Set(context, JS_STR("LINK_STATUS"), JS_INT(0x8B82));
    gl->Set(context, JS_STR("VALIDATE_STATUS"), JS_INT(0x8B83));
    gl->Set(context, JS_STR("ATTACHED_SHADERS"), JS_INT(0x8B85));
    gl->Set(context, JS_STR("ACTIVE_UNIFORMS"), JS_INT(0x8B86));
    gl->Set(context, JS_STR("ACTIVE_UNIFORM_MAX_LENGTH"), JS_INT(0x8B87));
    gl->Set(context, JS_STR("ACTIVE_ATTRIBUTES"), JS_INT(0x8B89));
    gl->Set(context, JS_STR("ACTIVE_ATTRIBUTE_MAX_LENGTH"), JS_INT(0x8B8A));
    gl->Set(context, JS_STR("SHADING_LANGUAGE_VERSION"), JS_INT(0x8B8C));
    gl->Set(context, JS_STR("CURRENT_PROGRAM"), JS_INT(0x8B8D));
    gl->Set(context, JS_STR("NEVER"), JS_INT(0x0200));
    gl->Set(context, JS_STR("LESS"), JS_INT(0x0201));
    gl->Set(context, JS_STR("EQUAL"), JS_INT(0x0202));
    gl->Set(context, JS_STR("LEQUAL"), JS_INT(0x0203));
    gl->Set(context, JS_STR("GREATER"), JS_INT(0x0204));
    gl->Set(context, JS_STR("NOTEQUAL"), JS_INT(0x0205));
    gl->Set(context, JS_STR("GEQUAL"), JS_INT(0x0206));
    gl->Set(context, JS_STR("ALWAYS"), JS_INT(0x0207));
    gl->Set(context, JS_STR("KEEP"), JS_INT(0x1E00));
    gl->Set(context, JS_STR("REPLACE"), JS_INT(0x1E01));
    gl->Set(context, JS_STR("INCR"), JS_INT(0x1E02));
    gl->Set(context, JS_STR("DECR"), JS_INT(0x1E03));
    gl->Set(context, JS_STR("INVERT"), JS_INT(0x150A));
    gl->Set(context, JS_STR("INCR_WRAP"), JS_INT(0x8507));
    gl->Set(context, JS_STR("DECR_WRAP"), JS_INT(0x8508));
    gl->Set(context, JS_STR("VENDOR"), JS_INT(0x1F00));
    gl->Set(context, JS_STR("RENDERER"), JS_INT(0x1F01));
    gl->Set(context, JS_STR("VERSION"), JS_INT(0x1F02));
    gl->Set(context, JS_STR("EXTENSIONS"), JS_INT(0x1F03));
    gl->Set(context, JS_STR("NEAREST"), JS_INT(0x2600));
    gl->Set(context, JS_STR("LINEAR"), JS_INT(0x2601));
    gl->Set(context, JS_STR("NEAREST_MIPMAP_NEAREST"), JS_INT(0x2700));
    gl->Set(context, JS_STR("LINEAR_MIPMAP_NEAREST"), JS_INT(0x2701));
    gl->Set(context, JS_STR("NEAREST_MIPMAP_LINEAR"), JS_INT(0x2702));
    gl->Set(context, JS_STR("LINEAR_MIPMAP_LINEAR"), JS_INT(0x2703));
    gl->Set(context, JS_STR("TEXTURE_MAG_FILTER"), JS_INT(0x2800));
    gl->Set(context, JS_STR("TEXTURE_MIN_FILTER"), JS_INT(0x2801));
    gl->Set(context, JS_STR("TEXTURE_WRAP_S"), JS_INT(0x2802));
    gl->Set(context, JS_STR("TEXTURE_WRAP_T"), JS_INT(0x2803));
    gl->Set(context, JS_STR("TEXTURE"), JS_INT(0x1702));
    gl->Set(context, JS_STR("TEXTURE_CUBE_MAP"), JS_INT(0x8513));
    gl->Set(context, JS_STR("TEXTURE_BINDING_CUBE_MAP"), JS_INT(0x8514));
    gl->Set(context, JS_STR("TEXTURE_CUBE_MAP_POSITIVE_X"), JS_INT(0x8515));
    gl->Set(context, JS_STR("TEXTURE_CUBE_MAP_NEGATIVE_X"), JS_INT(0x8516));
    gl->Set(context, JS_STR("TEXTURE_CUBE_MAP_POSITIVE_Y"), JS_INT(0x8517));
    gl->Set(context, JS_STR("TEXTURE_CUBE_MAP_NEGATIVE_Y"), JS_INT(0x8518));
    gl->Set(context, JS_STR("TEXTURE_CUBE_MAP_POSITIVE_Z"), JS_INT(0x8519));
    gl->Set(context, JS_STR("TEXTURE_CUBE_MAP_NEGATIVE_Z"), JS_INT(0x851A));
    gl->Set(context, JS_STR("MAX_CUBE_MAP_TEXTURE_SIZE"), JS_INT(0x851C));
    gl->Set(context, JS_STR("TEXTURE0"), JS_INT(0x84C0));
    gl->Set(context, JS_STR("TEXTURE1"), JS_INT(0x84C1));
    gl->Set(context, JS_STR("TEXTURE2"), JS_INT(0x84C2));
    gl->Set(context, JS_STR("TEXTURE3"), JS_INT(0x84C3));
    gl->Set(context, JS_STR("TEXTURE4"), JS_INT(0x84C4));
    gl->Set(context, JS_STR("TEXTURE5"), JS_INT(0x84C5));
    gl->Set(context, JS_STR("TEXTURE6"), JS_INT(0x84C6));
    gl->Set(context, JS_STR("TEXTURE7"), JS_INT(0x84C7));
    gl->Set(context, JS_STR("TEXTURE8"), JS_INT(0x84C8));
    gl->Set(context, JS_STR("TEXTURE9"), JS_INT(0x84C9));
    gl->Set(context, JS_STR("TEXTURE10"), JS_INT(0x84CA));
    gl->Set(context, JS_STR("TEXTURE11"), JS_INT(0x84CB));
    gl->Set(context, JS_STR("TEXTURE12"), JS_INT(0x84CC));
    gl->Set(context, JS_STR("TEXTURE13"), JS_INT(0x84CD));
    gl->Set(context, JS_STR("TEXTURE14"), JS_INT(0x84CE));
    gl->Set(context, JS_STR("TEXTURE15"), JS_INT(0x84CF));
    gl->Set(context, JS_STR("TEXTURE16"), JS_INT(0x84D0));
    gl->Set(context, JS_STR("TEXTURE17"), JS_INT(0x84D1));
    gl->Set(context, JS_STR("TEXTURE18"), JS_INT(0x84D2));
    gl->Set(context, JS_STR("TEXTURE19"), JS_INT(0x84D3));
    gl->Set(context, JS_STR("TEXTURE20"), JS_INT(0x84D4));
    gl->Set(context, JS_STR("TEXTURE21"), JS_INT(0x84D5));
    gl->Set(context, JS_STR("TEXTURE22"), JS_INT(0x84D6));
    gl->Set(context, JS_STR("TEXTURE23"), JS_INT(0x84D7));
    gl->Set(context, JS_STR("TEXTURE24"), JS_INT(0x84D8));
    gl->Set(context, JS_STR("TEXTURE25"), JS_INT(0x84D9));
    gl->Set(context, JS_STR("TEXTURE26"), JS_INT(0x84DA));
    gl->Set(context, JS_STR("TEXTURE27"), JS_INT(0x84DB));
    gl->Set(context, JS_STR("TEXTURE28"), JS_INT(0x84DC));
    gl->Set(context, JS_STR("TEXTURE29"), JS_INT(0x84DD));
    gl->Set(context, JS_STR("TEXTURE30"), JS_INT(0x84DE));
    gl->Set(context, JS_STR("TEXTURE31"), JS_INT(0x84DF));
    gl->Set(context, JS_STR("ACTIVE_TEXTURE"), JS_INT(0x84E0));
    gl->Set(context, JS_STR("REPEAT"), JS_INT(0x2901));
    gl->Set(context, JS_STR("CLAMP_TO_EDGE"), JS_INT(0x812F));
    gl->Set(context, JS_STR("MIRRORED_REPEAT"), JS_INT(0x8370));
    gl->Set(context, JS_STR("FLOAT_VEC2"), JS_INT(0x8B50));
    gl->Set(context, JS_STR("FLOAT_VEC3"), JS_INT(0x8B51));
    gl->Set(context, JS_STR("FLOAT_VEC4"), JS_INT(0x8B52));
    gl->Set(context, JS_STR("INT_VEC2"), JS_INT(0x8B53));
    gl->Set(context, JS_STR("INT_VEC3"), JS_INT(0x8B54));
    gl->Set(context, JS_STR("INT_VEC4"), JS_INT(0x8B55));
    gl->Set(context, JS_STR("BOOL"), JS_INT(0x8B56));
    gl->Set(context, JS_STR("BOOL_VEC2"), JS_INT(0x8B57));
    gl->Set(context, JS_STR("BOOL_VEC3"), JS_INT(0x8B58));
    gl->Set(context, JS_STR("BOOL_VEC4"), JS_INT(0x8B59));
    gl->Set(context, JS_STR("FLOAT_MAT2"), JS_INT(0x8B5A));
    gl->Set(context, JS_STR("FLOAT_MAT3"), JS_INT(0x8B5B));
    gl->Set(context, JS_STR("FLOAT_MAT4"), JS_INT(0x8B5C));
    gl->Set(context, JS_STR("SAMPLER_2D"), JS_INT(0x8B5E));
    gl->Set(context, JS_STR("SAMPLER_CUBE"), JS_INT(0x8B60));
    gl->Set(context, JS_STR("VERTEX_ATTRIB_ARRAY_ENABLED"), JS_INT(0x8622));
    gl->Set(context, JS_STR("VERTEX_ATTRIB_ARRAY_SIZE"), JS_INT(0x8623));
    gl->Set(context, JS_STR("VERTEX_ATTRIB_ARRAY_STRIDE"), JS_INT(0x8624));
    gl->Set(context, JS_STR("VERTEX_ATTRIB_ARRAY_TYPE"), JS_INT(0x8625));
    gl->Set(context, JS_STR("VERTEX_ATTRIB_ARRAY_NORMALIZED"), JS_INT(0x886A));
    gl->Set(context, JS_STR("VERTEX_ATTRIB_ARRAY_POINTER"), JS_INT(0x8645));
    gl->Set(context, JS_STR("VERTEX_ATTRIB_ARRAY_BUFFER_BINDING"), JS_INT(0x889F));
    gl->Set(context, JS_STR("IMPLEMENTATION_COLOR_READ_TYPE"), JS_INT(0x8B9A));
    gl->Set(context, JS_STR("IMPLEMENTATION_COLOR_READ_FORMAT"), JS_INT(0x8B9B));
    gl->Set(context, JS_STR("COMPILE_STATUS"), JS_INT(0x8B81));
    gl->Set(context, JS_STR("INFO_LOG_LENGTH"), JS_INT(0x8B84));
    gl->Set(context, JS_STR("SHADER_SOURCE_LENGTH"), JS_INT(0x8B88));
    gl->Set(context, JS_STR("SHADER_COMPILER"), JS_INT(0x8DFA));
    gl->Set(context, JS_STR("SHADER_BINARY_FORMATS"), JS_INT(0x8DF8));
    gl->Set(context, JS_STR("NUM_SHADER_BINARY_FORMATS"), JS_INT(0x8DF9));
    gl->Set(context, JS_STR("LOW_FLOAT"), JS_INT(0x8DF0));
    gl->Set(context, JS_STR("MEDIUM_FLOAT"), JS_INT(0x8DF1));
    gl->Set(context, JS_STR("HIGH_FLOAT"), JS_INT(0x8DF2));
    gl->Set(context, JS_STR("LOW_INT"), JS_INT(0x8DF3));
    gl->Set(context, JS_STR("MEDIUM_INT"), JS_INT(0x8DF4));
    gl->Set(context, JS_STR("HIGH_INT"), JS_INT(0x8DF5));
    gl->Set(context, JS_STR("FRAMEBUFFER"), JS_INT(0x8D40));
    gl->Set(context, JS_STR("RENDERBUFFER"), JS_INT(0x8D41));
    gl->Set(context, JS_STR("RGBA4"), JS_INT(0x8056));
    gl->Set(context, JS_STR("RGB5_A1"), JS_INT(0x8057));
    gl->Set(context, JS_STR("RGB565"), JS_INT(0x8D62));
    gl->Set(context, JS_STR("DEPTH_COMPONENT16"), JS_INT(0x81A5));
    gl->Set(context, JS_STR("STENCIL_INDEX"), JS_INT(0x1901));
    gl->Set(context, JS_STR("STENCIL_INDEX8"), JS_INT(0x8D48));
    gl->Set(context, JS_STR("RENDERBUFFER_WIDTH"), JS_INT(0x8D42));
    gl->Set(context, JS_STR("RENDERBUFFER_HEIGHT"), JS_INT(0x8D43));
    gl->Set(context, JS_STR("RENDERBUFFER_INTERNAL_FORMAT"), JS_INT(0x8D44));
    gl->Set(context, JS_STR("RENDERBUFFER_RED_SIZE"), JS_INT(0x8D50));
    gl->Set(context, JS_STR("RENDERBUFFER_GREEN_SIZE"), JS_INT(0x8D51));
    gl->Set(context, JS_STR("RENDERBUFFER_BLUE_SIZE"), JS_INT(0x8D52));
    gl->Set(context, JS_STR("RENDERBUFFER_ALPHA_SIZE"), JS_INT(0x8D53));
    gl->Set(context, JS_STR("RENDERBUFFER_DEPTH_SIZE"), JS_INT(0x8D54));
    gl->Set(context, JS_STR("RENDERBUFFER_STENCIL_SIZE"), JS_INT(0x8D55));
    gl->Set(context, JS_STR("FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE"), JS_INT(0x8CD0));
    gl->Set(context, JS_STR("FRAMEBUFFER_ATTACHMENT_OBJECT_NAME"), JS_INT(0x8CD1));
    gl->Set(context, JS_STR("FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL"), JS_INT(0x8CD2));
    gl->Set(context, JS_STR("FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE"), JS_INT(0x8CD3));
    gl->Set(context, JS_STR("DEPTH_ATTACHMENT"), JS_INT(0x8D00));
    gl->Set(context, JS_STR("STENCIL_ATTACHMENT"), JS_INT(0x8D20));
    gl->Set(context, JS_STR("NONE"), JS_INT(0));
    gl->Set(context, JS_STR("FRAMEBUFFER_COMPLETE"), JS_INT(0x8CD5));
    gl->Set(context, JS_STR("FRAMEBUFFER_INCOMPLETE_ATTACHMENT"), JS_INT(0x8CD6));
    gl->Set(context, JS_STR("FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"), JS_INT(0x8CD7));
    gl->Set(context, JS_STR("FRAMEBUFFER_INCOMPLETE_DIMENSIONS"), JS_INT(0x8CD9));
    gl->Set(context, JS_STR("FRAMEBUFFER_UNSUPPORTED"), JS_INT(0x8CDD));
    gl->Set(context, JS_STR("FRAMEBUFFER_BINDING"), JS_INT(0x8CA6));
    gl->Set(context, JS_STR("RENDERBUFFER_BINDING"), JS_INT(0x8CA7));
    gl->Set(context, JS_STR("MAX_RENDERBUFFER_SIZE"), JS_INT(0x84E8));
    gl->Set(context, JS_STR("INVALID_FRAMEBUFFER_OPERATION"), JS_INT(0x0506));
    gl->Set(context, JS_STR("DRAW_FRAMEBUFFER"), JS_INT(GL_DRAW_FRAMEBUFFER));
    gl->Set(context, JS_STR("READ_FRAMEBUFFER"), JS_INT(GL_READ_FRAMEBUFFER));

    gl->Set(context, JS_STR("RGBA16F"), JS_NUM(GL_RGBA16F));
    gl->Set(context, JS_STR("RGBA32F"), JS_NUM(GL_RGBA32F));
    gl->Set(context, JS_STR("RGB16F"), JS_NUM(GL_RGB16F));
    gl->Set(context, JS_STR("RGB32F"), JS_NUM(GL_RGB32F));
    gl->Set(context, JS_STR("UNIFORM_BUFFER"), JS_NUM(GL_UNIFORM_BUFFER));
    gl->Set(context, JS_STR("HALF_FLOAT"), JS_NUM(GL_HALF_FLOAT));
    gl->Set(context, JS_STR("TEXTURE_EXTERNAL_OES"), JS_NUM(0x8D65));
    gl->Set(context, JS_STR("UNSIGNED_INT_24_8"), JS_NUM(GL_UNSIGNED_INT_24_8));
    gl->Set(context, JS_STR("MAX_TEXTURE_MAX_ANISOTROPY_EXT"), JS_NUM(0x84FF));
    gl->Set(context, JS_STR("TEXTURE_MAX_ANISOTROPY_EXT"), JS_NUM(0x84FE));

    gl->Set(context, JS_STR("COLOR_ATTACHMENT0"), JS_INT(GL_COLOR_ATTACHMENT0));
    gl->Set(context, JS_STR("COLOR_ATTACHMENT1"), JS_INT(GL_COLOR_ATTACHMENT1));
    gl->Set(context, JS_STR("COLOR_ATTACHMENT2"), JS_INT(GL_COLOR_ATTACHMENT2));
    gl->Set(context, JS_STR("COLOR_ATTACHMENT3"), JS_INT(GL_COLOR_ATTACHMENT3));
    gl->Set(context, JS_STR("COLOR_ATTACHMENT4"), JS_INT(GL_COLOR_ATTACHMENT4));
    gl->Set(context, JS_STR("COLOR_ATTACHMENT5"), JS_INT(GL_COLOR_ATTACHMENT5));
    gl->Set(context, JS_STR("COLOR_ATTACHMENT6"), JS_INT(GL_COLOR_ATTACHMENT6));
    gl->Set(context, JS_STR("COLOR_ATTACHMENT7"), JS_INT(GL_COLOR_ATTACHMENT7));
    gl->Set(context, JS_STR("COLOR_ATTACHMENT8"), JS_INT(GL_COLOR_ATTACHMENT8));
    gl->Set(context, JS_STR("COLOR_ATTACHMENT9"), JS_INT(GL_COLOR_ATTACHMENT9));
    gl->Set(context, JS_STR("COLOR_ATTACHMENT10"), JS_INT(GL_COLOR_ATTACHMENT10));
    gl->Set(context, JS_STR("COLOR_ATTACHMENT11"), JS_INT(GL_COLOR_ATTACHMENT11));
    gl->Set(context, JS_STR("COLOR_ATTACHMENT12"), JS_INT(GL_COLOR_ATTACHMENT12));
    gl->Set(context, JS_STR("COLOR_ATTACHMENT13"), JS_INT(GL_COLOR_ATTACHMENT13));
    gl->Set(context, JS_STR("COLOR_ATTACHMENT14"), JS_INT(GL_COLOR_ATTACHMENT14));
    gl->Set(context, JS_STR("COLOR_ATTACHMENT15"), JS_INT(GL_COLOR_ATTACHMENT15));
    gl->Set(context, JS_STR("DEPTH_COMPONENT24"), JS_INT(GL_DEPTH_COMPONENT24));
    gl->Set(context, JS_STR("DEPTH_STENCIL_ATTACHMENT"), JS_INT(GL_DEPTH_STENCIL_ATTACHMENT));
    gl->Set(context, JS_STR("DEPTH_STENCIL"), JS_INT(DEPTH_STENCIL));
    gl->Set(context, JS_STR("MAX_TEXTURE_MAX_ANISOTROPY_EXT"), JS_INT(0x84FF));
    gl->Set(context, JS_STR("TEXTURE_MAX_ANISOTROPY_EXT"), JS_INT(0x84FE));

    gl->Set(context, JS_STR("_checkErrors"), FUNC(AURA_CheckErrors));
    gl->Set(context, JS_STR("_bindPromiseRejection"), FUNC(AURA_BindPromiseRejection));

    gl->Set(context, JS_STR("activeTexture"), FUNC(v8Bind_ActiveTexture));
    gl->Set(context, JS_STR("attachShader"), FUNC(v8Bind_AttachShader));
    gl->Set(context, JS_STR("bindAttribLocation"), FUNC(v8Bind_BindAttribLocation));
    gl->Set(context, JS_STR("bindBuffer"), FUNC(v8Bind_BindBuffer));
    gl->Set(context, JS_STR("bindFramebuffer"), FUNC(v8Bind_BindFramebuffer));
    gl->Set(context, JS_STR("bindRenderbuffer"), FUNC(v8Bind_BindRenderbuffer));
    gl->Set(context, JS_STR("bindTexture"), FUNC(v8Bind_BindTexture));
    gl->Set(context, JS_STR("blendColor"), FUNC(v8Bind_BlendColor));
    gl->Set(context, JS_STR("blendEquation"), FUNC(v8Bind_BlendEquation));
    gl->Set(context, JS_STR("blendEquationSeparate"), FUNC(v8Bind_BlendEquationSeparate));
    gl->Set(context, JS_STR("blendFunc"), FUNC(v8Bind_BlendFunc));
    gl->Set(context, JS_STR("blendFuncSeparate"), FUNC(v8Bind_BlendFuncSeparate));
    gl->Set(context, JS_STR("bufferData"), FUNC(v8Bind_BufferData));
    gl->Set(context, JS_STR("bufferSubData"), FUNC(v8Bind_BufferSubData));
    gl->Set(context, JS_STR("checkFramebufferStatus"), FUNC(v8Bind_CheckFramebufferStatus));
    gl->Set(context, JS_STR("clear"), FUNC(v8Bind_Clear));
    gl->Set(context, JS_STR("clearColor"), FUNC(v8Bind_ClearColor));
    gl->Set(context, JS_STR("clearDepthf"), FUNC(v8Bind_ClearDepthf));
    gl->Set(context, JS_STR("clearStencil"), FUNC(v8Bind_ClearStencil));
    gl->Set(context, JS_STR("colorMask"), FUNC(v8Bind_ColorMask));
    gl->Set(context, JS_STR("compileShader"), FUNC(v8Bind_CompileShader));
    gl->Set(context, JS_STR("compressedTexImage2D"), FUNC(v8Bind_CompressedTexImage2D));
    gl->Set(context, JS_STR("compressedTexSubImage2D"), FUNC(v8Bind_CompressedTexSubImage2D));
    gl->Set(context, JS_STR("copyTexImage2D"), FUNC(v8Bind_CopyTexImage2D));
    gl->Set(context, JS_STR("copyTexSubImage2D"), FUNC(v8Bind_CopyTexSubImage2D));
    gl->Set(context, JS_STR("createProgram"), FUNC(v8Bind_CreateProgram));
    gl->Set(context, JS_STR("createShader"), FUNC(v8Bind_CreateShader));
    gl->Set(context, JS_STR("cullFace"), FUNC(v8Bind_CullFace));
    gl->Set(context, JS_STR("deleteBuffers"), FUNC(v8Bind_DeleteBuffers));
    gl->Set(context, JS_STR("deleteFramebuffers"), FUNC(v8Bind_DeleteFramebuffers));
    gl->Set(context, JS_STR("deleteProgram"), FUNC(v8Bind_DeleteProgram));
    gl->Set(context, JS_STR("deleteRenderbuffers"), FUNC(v8Bind_DeleteRenderbuffers));
    gl->Set(context, JS_STR("deleteShader"), FUNC(v8Bind_DeleteShader));
    gl->Set(context, JS_STR("deleteTextures"), FUNC(v8Bind_DeleteTextures));
    gl->Set(context, JS_STR("depthFunc"), FUNC(v8Bind_DepthFunc));
    gl->Set(context, JS_STR("depthMask"), FUNC(v8Bind_DepthMask));
    gl->Set(context, JS_STR("depthRangef"), FUNC(v8Bind_DepthRangef));
    gl->Set(context, JS_STR("detachShader"), FUNC(v8Bind_DetachShader));
    gl->Set(context, JS_STR("disable"), FUNC(v8Bind_Disable));
    gl->Set(context, JS_STR("disableVertexAttribArray"), FUNC(v8Bind_DisableVertexAttribArray));
    gl->Set(context, JS_STR("drawArrays"), FUNC(v8Bind_DrawArrays));
    gl->Set(context, JS_STR("drawElements"), FUNC(v8Bind_DrawElements));
    gl->Set(context, JS_STR("enable"), FUNC(v8Bind_Enable));
    gl->Set(context, JS_STR("enableVertexAttribArray"), FUNC(v8Bind_EnableVertexAttribArray));
    gl->Set(context, JS_STR("finish"), FUNC(v8Bind_Finish));
    gl->Set(context, JS_STR("flush"), FUNC(v8Bind_Flush));
    gl->Set(context, JS_STR("framebufferRenderbuffer"), FUNC(v8Bind_FramebufferRenderbuffer));
    gl->Set(context, JS_STR("framebufferTexture2D"), FUNC(v8Bind_FramebufferTexture2D));
    gl->Set(context, JS_STR("frontFace"), FUNC(v8Bind_FrontFace));
    gl->Set(context, JS_STR("genBuffers"), FUNC(v8Bind_GenBuffers));
    gl->Set(context, JS_STR("generateMipmap"), FUNC(v8Bind_GenerateMipmap));
    gl->Set(context, JS_STR("genFramebuffers"), FUNC(v8Bind_GenFramebuffers));
    gl->Set(context, JS_STR("genRenderbuffers"), FUNC(v8Bind_GenRenderbuffers));
    gl->Set(context, JS_STR("genTextures"), FUNC(v8Bind_GenTextures));
    gl->Set(context, JS_STR("getActiveAttrib"), FUNC(v8Bind_GetActiveAttrib));
    gl->Set(context, JS_STR("getActiveUniform"), FUNC(v8Bind_GetActiveUniform));
    gl->Set(context, JS_STR("getAttachedShaders"), FUNC(v8Bind_GetAttachedShaders));
    gl->Set(context, JS_STR("getAttribLocation"), FUNC(v8Bind_GetAttribLocation));
    gl->Set(context, JS_STR("getBooleanv"), FUNC(v8Bind_GetBooleanv));
    gl->Set(context, JS_STR("getBufferParameteriv"), FUNC(v8Bind_GetBufferParameteriv));
    gl->Set(context, JS_STR("getError"), FUNC(v8Bind_GetError));
    gl->Set(context, JS_STR("getFloatv"), FUNC(v8Bind_GetFloatv));
    gl->Set(context, JS_STR("getFramebufferAttachmentParameteriv"), FUNC(v8Bind_GetFramebufferAttachmentParameteriv));
    gl->Set(context, JS_STR("getIntegerv"), FUNC(v8Bind_GetIntegerv));
    gl->Set(context, JS_STR("getProgramiv"), FUNC(v8Bind_GetProgramiv));
    gl->Set(context, JS_STR("getProgramInfoLog"), FUNC(v8Bind_GetProgramInfoLog));
    gl->Set(context, JS_STR("getRenderbufferParameteriv"), FUNC(v8Bind_GetRenderbufferParameteriv));
    gl->Set(context, JS_STR("getShaderiv"), FUNC(v8Bind_GetShaderiv));
    gl->Set(context, JS_STR("getShaderInfoLog"), FUNC(v8Bind_GetShaderInfoLog));
    gl->Set(context, JS_STR("getShaderPrecisionFormat"), FUNC(v8Bind_GetShaderPrecisionFormat));
    gl->Set(context, JS_STR("getShaderSource"), FUNC(v8Bind_GetShaderSource));
    gl->Set(context, JS_STR("getString"), FUNC(v8Bind_GetString));
    gl->Set(context, JS_STR("getTexParameterfv"), FUNC(v8Bind_GetTexParameterfv));
    gl->Set(context, JS_STR("getTexParameteriv"), FUNC(v8Bind_GetTexParameteriv));
    gl->Set(context, JS_STR("getUniformfv"), FUNC(v8Bind_GetUniformfv));
    gl->Set(context, JS_STR("getUniformiv"), FUNC(v8Bind_GetUniformiv));
    gl->Set(context, JS_STR("getUniformLocation"), FUNC(v8Bind_GetUniformLocation));
    gl->Set(context, JS_STR("getVertexAttribfv"), FUNC(v8Bind_GetVertexAttribfv));
    gl->Set(context, JS_STR("getVertexAttribiv"), FUNC(v8Bind_GetVertexAttribiv));
    gl->Set(context, JS_STR("getVertexAttribPointerv"), FUNC(v8Bind_GetVertexAttribPointerv));
    gl->Set(context, JS_STR("hint"), FUNC(v8Bind_Hint));
    gl->Set(context, JS_STR("isBuffer"), FUNC(v8Bind_IsBuffer));
    gl->Set(context, JS_STR("isEnabled"), FUNC(v8Bind_IsEnabled));
    gl->Set(context, JS_STR("isFramebuffer"), FUNC(v8Bind_IsFramebuffer));
    gl->Set(context, JS_STR("isProgram"), FUNC(v8Bind_IsProgram));
    gl->Set(context, JS_STR("isRenderbuffer"), FUNC(v8Bind_IsRenderbuffer));
    gl->Set(context, JS_STR("isShader"), FUNC(v8Bind_IsShader));
    gl->Set(context, JS_STR("isTexture"), FUNC(v8Bind_IsTexture));
    gl->Set(context, JS_STR("lineWidth"), FUNC(v8Bind_LineWidth));
    gl->Set(context, JS_STR("linkProgram"), FUNC(v8Bind_LinkProgram));
    gl->Set(context, JS_STR("pixelStorei"), FUNC(v8Bind_PixelStorei));
    gl->Set(context, JS_STR("polygonOffset"), FUNC(v8Bind_PolygonOffset));
    gl->Set(context, JS_STR("readPixels"), FUNC(v8Bind_ReadPixels));
    gl->Set(context, JS_STR("releaseShaderCompiler"), FUNC(v8Bind_ReleaseShaderCompiler));
    gl->Set(context, JS_STR("renderbufferStorage"), FUNC(v8Bind_RenderbufferStorage));
    gl->Set(context, JS_STR("sampleCoverage"), FUNC(v8Bind_SampleCoverage));
    gl->Set(context, JS_STR("scissor"), FUNC(v8Bind_Scissor));
    gl->Set(context, JS_STR("shaderBinary"), FUNC(v8Bind_ShaderBinary));
    gl->Set(context, JS_STR("shaderSource"), FUNC(v8Bind_ShaderSource));
    gl->Set(context, JS_STR("stencilFunc"), FUNC(v8Bind_StencilFunc));
    gl->Set(context, JS_STR("stencilFuncSeparate"), FUNC(v8Bind_StencilFuncSeparate));
    gl->Set(context, JS_STR("stencilMask"), FUNC(v8Bind_StencilMask));
    gl->Set(context, JS_STR("stencilMaskSeparate"), FUNC(v8Bind_StencilMaskSeparate));
    gl->Set(context, JS_STR("stencilOp"), FUNC(v8Bind_StencilOp));
    gl->Set(context, JS_STR("stencilOpSeparate"), FUNC(v8Bind_StencilOpSeparate));
    gl->Set(context, JS_STR("texParameterf"), FUNC(v8Bind_TexParameterf));
    gl->Set(context, JS_STR("texParameterfv"), FUNC(v8Bind_TexParameterfv));
    gl->Set(context, JS_STR("texParameteri"), FUNC(v8Bind_TexParameteri));
    gl->Set(context, JS_STR("texParameteriv"), FUNC(v8Bind_TexParameteriv));
    gl->Set(context, JS_STR("texSubImage2D"), FUNC(v8Bind_TexSubImage2D));
    gl->Set(context, JS_STR("uniform1f"), FUNC(v8Bind_Uniform1f));
    gl->Set(context, JS_STR("uniform1fv"), FUNC(v8Bind_Uniform1fv));
    gl->Set(context, JS_STR("uniform1i"), FUNC(v8Bind_Uniform1i));
    gl->Set(context, JS_STR("uniform1iv"), FUNC(v8Bind_Uniform1iv));
    gl->Set(context, JS_STR("uniform2f"), FUNC(v8Bind_Uniform2f));
    gl->Set(context, JS_STR("uniform2fv"), FUNC(v8Bind_Uniform2fv));
    gl->Set(context, JS_STR("uniform2i"), FUNC(v8Bind_Uniform2i));
    gl->Set(context, JS_STR("uniform2iv"), FUNC(v8Bind_Uniform2iv));
    gl->Set(context, JS_STR("uniform3f"), FUNC(v8Bind_Uniform3f));
    gl->Set(context, JS_STR("uniform3fv"), FUNC(v8Bind_Uniform3fv));
    gl->Set(context, JS_STR("uniform3i"), FUNC(v8Bind_Uniform3i));
    gl->Set(context, JS_STR("uniform3iv"), FUNC(v8Bind_Uniform3iv));
    gl->Set(context, JS_STR("uniform4f"), FUNC(v8Bind_Uniform4f));
    gl->Set(context, JS_STR("uniform4fv"), FUNC(v8Bind_Uniform4fv));
    gl->Set(context, JS_STR("uniform4i"), FUNC(v8Bind_Uniform4i));
    gl->Set(context, JS_STR("uniform4iv"), FUNC(v8Bind_Uniform4iv));
    gl->Set(context, JS_STR("uniformMatrix2fv"), FUNC(v8Bind_UniformMatrix2fv));
    gl->Set(context, JS_STR("uniformMatrix3fv"), FUNC(v8Bind_UniformMatrix3fv));
    gl->Set(context, JS_STR("uniformMatrix4fv"), FUNC(v8Bind_UniformMatrix4fv));
    gl->Set(context, JS_STR("useProgram"), FUNC(v8Bind_UseProgram));
    gl->Set(context, JS_STR("validateProgram"), FUNC(v8Bind_ValidateProgram));
    gl->Set(context, JS_STR("vertexAttrib1f"), FUNC(v8Bind_VertexAttrib1f));
    gl->Set(context, JS_STR("vertexAttrib1fv"), FUNC(v8Bind_VertexAttrib1fv));
    gl->Set(context, JS_STR("vertexAttrib2f"), FUNC(v8Bind_VertexAttrib2f));
    gl->Set(context, JS_STR("vertexAttrib2fv"), FUNC(v8Bind_VertexAttrib2fv));
    gl->Set(context, JS_STR("vertexAttrib3f"), FUNC(v8Bind_VertexAttrib3f));
    gl->Set(context, JS_STR("vertexAttrib3fv"), FUNC(v8Bind_VertexAttrib3fv));
    gl->Set(context, JS_STR("vertexAttrib4f"), FUNC(v8Bind_VertexAttrib4f));
    gl->Set(context, JS_STR("vertexAttrib4fv"), FUNC(v8Bind_VertexAttrib4fv));
    gl->Set(context, JS_STR("vertexAttribPointer"), FUNC(v8Bind_VertexAttribPointer));
    gl->Set(context, JS_STR("viewport"), FUNC(v8Bind_Viewport));
    gl->Set(context, JS_STR("getParameter"), FUNC(v8Bind_GetParameter));
    gl->Set(context, JS_STR("getExtension"), FUNC(v8Bind_GetExtension));
    gl->Set(context, JS_STR("_texImage2DNull"), FUNC(AURA_TexImage2DNull));
    gl->Set(context, JS_STR("_texImage2DArray"), FUNC(AURA_TexImage2DArray));
    gl->Set(context, JS_STR("_getExtensions"), FUNC(AURA_GetExtensions));
    gl->Set(context, JS_STR("drawElementsInstanced"), FUNC(AURA_DrawElementsInstanced));
    gl->Set(context, JS_STR("drawArraysInstanced"), FUNC(AURA_DrawArraysInstanced));
    gl->Set(context, JS_STR("vertexAttribDivisor"), FUNC(AURA_VertexAttribDivisor));
    gl->Set(context, JS_STR("getUniformBlockIndex"), FUNC(AURA_GetUniformBlockIndex));
    gl->Set(context, JS_STR("uniformBlockBinding"), FUNC(AURA_UniformBlockBinding));
    gl->Set(context, JS_STR("bindBufferBase"), FUNC(AURA_BindBufferBase));
    gl->Set(context, JS_STR("createVertexArray"), FUNC(AURA_CreateVertexArray));
    gl->Set(context, JS_STR("bindVertexArray"), FUNC(AURA_BindVertexArray));
    gl->Set(context, JS_STR("deleteVertexArray"), FUNC(AURA_DeleteVertexArray));
    gl->Set(context, JS_STR("drawBuffers"), FUNC(AURA_DrawBuffers));
    gl->Set(context, JS_STR("blitFramebuffer"), FUNC(AURA_BlitFramebuffer));
}


JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1initAura
    (JNIEnv *env, jobject, jlong v8RuntimePtr) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );

  Local<Object> gl = Object::New(isolate);
  context->Global()->Set(v8::String::NewFromUtf8(isolate, "_gl"), gl);

  initializeAura(runtime, context, gl);
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1setWeak
  (JNIEnv * env, jobject, jlong v8RuntimePtr, jlong objectHandle) {
    Isolate* isolate = SETUP(env, v8RuntimePtr, );
    WeakReferenceDescriptor* wrd = new WeakReferenceDescriptor();
    wrd->v8RuntimePtr = v8RuntimePtr;
    wrd->objectHandle = objectHandle;
    reinterpret_cast<Persistent<Object>*>(objectHandle)->SetWeak(wrd, [](v8::WeakCallbackInfo<WeakReferenceDescriptor> const& data) {
      WeakReferenceDescriptor* wrd = data.GetParameter();
      JNIEnv * env;
      getJNIEnv(env);
      jobject v8 = reinterpret_cast<V8Runtime*>(wrd->v8RuntimePtr)->v8;
      env->CallVoidMethod(v8, v8WeakReferenceReleased, wrd->objectHandle);
      delete(wrd);
    }, WeakCallbackType::kFinalizer);
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1clearWeak
  (JNIEnv * env, jobject, jlong v8RuntimePtr, jlong objectHandle) {
    Isolate* isolate = SETUP(env, v8RuntimePtr, )
    reinterpret_cast<Persistent<Object>*>(objectHandle)->ClearWeak();
}

JNIEXPORT jboolean JNICALL Java_com_eclipsesource_v8_V8__1isWeak
  (JNIEnv * env, jobject, jlong v8RuntimePtr, jlong objectHandle) {
    Isolate* isolate = SETUP(env, v8RuntimePtr, 0)
    return reinterpret_cast<Persistent<Object>*>(objectHandle)->IsWeak();
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1registerJavaMethod
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring functionName, jboolean voidMethod) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0)
  FunctionCallback callback = voidCallback;
  if (!voidMethod) {
    callback = objectCallback;
  }
  Handle<Object> object = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  Local<String> v8FunctionName = createV8String(env, isolate, functionName);
  isolate->IdleNotificationDeadline(1);
  MethodDescriptor* md= new MethodDescriptor();
  Local<External> ext = External::New(isolate, md);
  Persistent<External> pext(isolate, ext);
  pext.SetWeak(md, [](v8::WeakCallbackInfo<MethodDescriptor> const& data) {
    MethodDescriptor* md = data.GetParameter();
    jobject v8 = reinterpret_cast<V8Runtime*>(md->v8RuntimePtr)->v8;
    JNIEnv * env;
    getJNIEnv(env);
    env->CallVoidMethod(v8, v8DisposeMethodID, md->methodID);
    delete(md);
  }, WeakCallbackType::kParameter);

  md->methodID = reinterpret_cast<jlong>(md);
  md->v8RuntimePtr = v8RuntimePtr;
  MaybeLocal<Function> func = Function::New(context, callback, ext);
  if (!func.IsEmpty()) {
    Maybe<bool> unusedResult = object->Set(context, v8FunctionName, func.ToLocalChecked());
    unusedResult.Check();
  }
  return md->methodID;
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1releaseMethodDescriptor
  (JNIEnv *, jobject, jlong, jlong methodDescriptorPtr) {
  MethodDescriptor* md = reinterpret_cast<MethodDescriptor*>(methodDescriptorPtr);
  delete(md);
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1setPrototype
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jlong prototypeHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, )
  Handle<Object> object = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  Handle<Object> prototype = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(prototypeHandle));
  object->SetPrototype(context, prototype);
}

JNIEXPORT jboolean JNICALL Java_com_eclipsesource_v8_V8__1equals
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jlong thatHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, false);
  Handle<Object> object = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  Handle<Object> that = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  if (objectHandle == 0) {
    object = context->Global();
  }
  if (thatHandle == 0) {
  	that = context->Global();
  }
  return object->Equals(context, that).FromMaybe(false);
}

JNIEXPORT jstring JNICALL Java_com_eclipsesource_v8_V8__1toString
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Object> object = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  String::Value unicodeString(isolate, object);

  return env->NewString(*unicodeString, unicodeString.length());
}

JNIEXPORT jboolean JNICALL Java_com_eclipsesource_v8_V8__1strictEquals
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jlong thatHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, false);
  Handle<Object> object = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  Handle<Object> that = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(thatHandle));
  if (objectHandle == reinterpret_cast<jlong>(runtime->globalObject)) {
    object = context->Global();
  }
  if (thatHandle == reinterpret_cast<jlong>(runtime->globalObject)) {
  	that = context->Global();
  }
  return object->StrictEquals(that);
}

JNIEXPORT jboolean JNICALL Java_com_eclipsesource_v8_V8__1sameValue
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jlong thatHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, false);
  Handle<Object> object = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  Handle<Object> that = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  if (objectHandle == reinterpret_cast<jlong>(runtime->globalObject)) {
    object = context->Global();
  }
  if (thatHandle == reinterpret_cast<jlong>(runtime->globalObject)) {
  	that = context->Global();
  }
  return object->SameValue(that);
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1identityHash
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, false);
  Handle<Object> object = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  if (objectHandle == reinterpret_cast<jlong>(runtime->globalObject)) {
    object = context->Global();
  }
  return object->GetIdentityHash();
}

Isolate* getIsolate(JNIEnv *env, jlong v8RuntimePtr) {
  if (v8RuntimePtr == 0) {
    throwError(env, "V8 isolate not found.");
    return nullptr;
  }
  V8Runtime* runtime = reinterpret_cast<V8Runtime*>(v8RuntimePtr);
  return runtime->isolate;
}

void throwResultUndefinedException(JNIEnv *env, const char *message) {
  (env)->ThrowNew(v8ResultsUndefinedCls, message);
}

void throwParseException(JNIEnv *env, const char* fileName, int lineNumber, String::Value *message,
  String::Value *sourceLine, int startColumn, int endColumn) {
  jstring jfileName = env->NewStringUTF(fileName);
  jstring jmessage = env->NewString(**message, message->length());
  jstring jsourceLine = env->NewString(**sourceLine, sourceLine->length());
  jthrowable result = (jthrowable)env->NewObject(v8ScriptCompilationCls, v8ScriptCompilationInitMethodID, jfileName, lineNumber, jmessage, jsourceLine, startColumn, endColumn);
  env->DeleteLocalRef(jfileName);
  env->DeleteLocalRef(jmessage);
  env->DeleteLocalRef(jsourceLine);
  (env)->Throw(result);
}

void throwExecutionException(JNIEnv *env, const char* fileName, int lineNumber, String::Value *message,
  String::Value* sourceLine, int startColumn, int endColumn, const char* stackTrace, jlong v8RuntimePtr) {
  jstring jfileName = env->NewStringUTF(fileName);
  jstring jmessage = env->NewString(**message, message->length());
  jstring jsourceLine = env->NewString(**sourceLine, sourceLine->length());
  jstring jstackTrace = nullptr;
  if (stackTrace != nullptr) {
    jstackTrace = env->NewStringUTF(stackTrace);
  }
  jthrowable wrappedException = nullptr;
  if (env->ExceptionCheck()) {
    wrappedException = env->ExceptionOccurred();
    env->ExceptionClear();
  }
  if (reinterpret_cast<V8Runtime*>(v8RuntimePtr)->pendingException != nullptr) {
    wrappedException = reinterpret_cast<V8Runtime*>(v8RuntimePtr)->pendingException;
    reinterpret_cast<V8Runtime*>(v8RuntimePtr)->pendingException = nullptr;
  }
  if ( wrappedException != nullptr && !env->IsInstanceOf( wrappedException, throwableCls) ) {
    std::cout << "Wrapped Exception is not a Throwable" << std::endl;
    wrappedException = nullptr;
  }
  jthrowable result = static_cast<jthrowable>(env->NewObject(v8ScriptExecutionException, v8ScriptExecutionExceptionInitMethodID, jfileName, lineNumber, jmessage, jsourceLine, startColumn, endColumn, jstackTrace, wrappedException));
  env->DeleteLocalRef(jfileName);
  env->DeleteLocalRef(jmessage);
  env->DeleteLocalRef(jsourceLine);
  (env)->Throw(result);
}

void throwParseException(JNIEnv *env, const Local<Context>& context, Isolate* isolate, TryCatch* tryCatch) {
 String::Value exception(isolate, tryCatch->Exception());
 Handle<Message> message = tryCatch->Message();
 if (message.IsEmpty()) {
   throwV8RuntimeException(env, &exception);
 }
 else {
   String::Utf8Value filename(isolate, message->GetScriptResourceName());
   int lineNumber = message->GetLineNumber(context).FromJust();
   String::Value sourceline(isolate, message->GetSourceLine(context).ToLocalChecked());
   int start = message->GetStartColumn();
   int end = message->GetEndColumn();
   const char* filenameString = ToCString(filename);
   throwParseException(env, filenameString, lineNumber, &exception, &sourceline, start, end);
 }
}

void throwExecutionException(JNIEnv *env, const Local<Context>& context, Isolate* isolate, TryCatch* tryCatch, jlong v8RuntimePtr) {
 String::Value exception(isolate, tryCatch->Exception());
 Handle<Message> message = tryCatch->Message();
 if (message.IsEmpty()) {
   throwV8RuntimeException(env, &exception);
 }
 else {
   String::Utf8Value filename(isolate, message->GetScriptResourceName());
   int lineNumber = message->GetLineNumber(context).FromMaybe(-1);
   String::Value sourceline(isolate, message->GetSourceLine(context).ToLocalChecked());
   int start = message->GetStartColumn();
   int end = message->GetEndColumn();
   const char* filenameString = ToCString(filename);
   MaybeLocal<Value> v8StackTrace = tryCatch->StackTrace(context);
   if (!v8StackTrace.IsEmpty()) {
       const char* stackTrace;
       String::Utf8Value stack_trace(isolate, v8StackTrace.ToLocalChecked());
       if (stack_trace.length() > 0) {
         stackTrace = ToCString(stack_trace);
       }
       throwExecutionException(env, filenameString, lineNumber, &exception, &sourceline, start, end, stackTrace, v8RuntimePtr);
   } else {
       throwExecutionException(env, filenameString, lineNumber, &exception, &sourceline, start, end, nullptr, v8RuntimePtr);
   }
 }
}

void throwV8RuntimeException(JNIEnv *env, String::Value *message) {
  jstring exceptionString = env->NewString(**message, message->length());
  jthrowable exception = (jthrowable)env->NewObject(v8RuntimeExceptionCls, v8RuntimeExceptionInitMethodID, exceptionString);
  (env)->Throw(exception);
  env->DeleteLocalRef(exceptionString);
}

void throwError(JNIEnv *env, const char *message) {
  (env)->ThrowNew(errorCls, message);
}

jobject getResult(JNIEnv *env, const Local<Context>& context, jobject &v8, jlong v8RuntimePtr, Handle<Value> &result, jint expectedType) {
  v8::Isolate* isolate = reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate;
  if (result->IsUndefined() && expectedType == com_eclipsesource_v8_V8_V8_ARRAY) {
    jobject objectResult = env->NewObject(undefinedV8ArrayCls, undefinedV8ArrayInitMethodID, v8);
    return objectResult;
  }
  else if (result->IsUndefined() && (expectedType == com_eclipsesource_v8_V8_V8_OBJECT || expectedType == com_eclipsesource_v8_V8_NULL)) {
    jobject objectResult = env->NewObject(undefinedV8ObjectCls, undefinedV8ObjectInitMethodID, v8);
    return objectResult;
  }
  else if (result->IsInt32()) {
    return env->NewObject(integerCls, integerInitMethodID, result->Int32Value(context).FromJust());
  }
  else if (result->IsNumber()) {
    return env->NewObject(doubleCls, doubleInitMethodID, result->NumberValue(context).FromJust());
  }
  else if (result->IsBoolean()) {
    return env->NewObject(booleanCls, booleanInitMethodID, result->BooleanValue(isolate));
  }
  else if (result->IsString()) {
    String::Value unicodeString(isolate, result->ToString((context)).ToLocalChecked());

    return env->NewString(*unicodeString, unicodeString.length());
  }
  else if (result->IsFunction()) {
    jobject objectResult = env->NewObject(v8FunctionCls, v8FunctionInitMethodID, v8);
    jlong resultHandle = getHandle(env, objectResult);

    v8::Isolate* isolate = reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate;

    reinterpret_cast<Persistent<Object>*>(resultHandle)->Reset(isolate, result->ToObject(context).ToLocalChecked());

    return objectResult;
  }
  else if (result->IsArray()) {
    jobject objectResult = env->NewObject(v8ArrayCls, v8ArrayInitMethodID, v8);
    jlong resultHandle = getHandle(env, objectResult);

    v8::Isolate* isolate = reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate;

    reinterpret_cast<Persistent<Object>*>(resultHandle)->Reset(isolate, result->ToObject(context).ToLocalChecked());

    return objectResult;
  }
  else if (result->IsTypedArray()) {
      jobject objectResult = env->NewObject(v8TypedArrayCls, v8TypedArrayInitMethodID, v8);
      jlong resultHandle = getHandle(env, objectResult);

      v8::Isolate* isolate = reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate;

      reinterpret_cast<Persistent<Object>*>(resultHandle)->Reset(isolate, result->ToObject(context).ToLocalChecked());

      return objectResult;
  }
  else if (result->IsArrayBuffer()) {
    ArrayBuffer* arrayBuffer = ArrayBuffer::Cast(*result);
    if ( arrayBuffer->ByteLength() == 0 || arrayBuffer->GetContents().Data() == nullptr ) {
      jobject objectResult = env->NewObject(v8ArrayBufferCls, v8ArrayBufferInitMethodID, v8, NULL);
      jlong resultHandle = getHandle(env, objectResult);
      v8::Isolate* isolate = reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate;
      reinterpret_cast<Persistent<Object>*>(resultHandle)->Reset(isolate, result->ToObject(context).ToLocalChecked());
      return objectResult;
    }
    jobject byteBuffer = env->NewDirectByteBuffer(arrayBuffer->GetContents().Data(), static_cast<jlong>(arrayBuffer->ByteLength()));
    jobject objectResult = env->NewObject(v8ArrayBufferCls, v8ArrayBufferInitMethodID, v8, byteBuffer);
    jlong resultHandle = getHandle(env, objectResult);

    v8::Isolate* isolate = reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate;

    reinterpret_cast<Persistent<Object>*>(resultHandle)->Reset(isolate, result->ToObject(context).ToLocalChecked());

    return objectResult;
  }
  else if (result->IsObject()) {
    jobject objectResult = env->NewObject(v8ObjectCls, v8ObjectInitMethodID, v8);
    jlong resultHandle = getHandle(env, objectResult);

    v8::Isolate* isolate = reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate;

    reinterpret_cast<Persistent<Object>*>(resultHandle)->Reset(isolate, result->ToObject(context).ToLocalChecked());

    return objectResult;
  }

  return nullptr;
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1getBuildID
  (JNIEnv *, jobject) {
  return 2;
}
