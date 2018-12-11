/*******************************************************************************
* Copyright (c) 2014 EclipseSource and others.
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*
* Contributors:
*    EclipseSource - initial API and implementation
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

#ifdef NODE_COMPATIBLE
  #include <deps/uv/include/uv.h>
  #include <node.h>
#endif

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

#ifdef NODE_COMPATIBLE
  node::Environment* nodeEnvironment;
  node::IsolateData* isolateData;
  uv_loop_t* uvLoop;
  bool running;
#endif

};

v8::Platform* v8Platform;

const char* ToCString(const String::Utf8Value& value) {
  return *value ? *value : "<string conversion failed>";
}

JavaVM* jvm = NULL;
jclass v8cls = NULL;
jclass v8ObjectCls = NULL;
jclass v8ArrayCls = NULL;
jclass v8TypedArrayCls = NULL;
jclass v8ArrayBufferCls = NULL;
jclass v8FunctionCls = NULL;
jclass undefinedV8ObjectCls = NULL;
jclass undefinedV8ArrayCls = NULL;
jclass v8ResultsUndefinedCls = NULL;
jclass v8ScriptCompilationCls = NULL;
jclass v8ScriptExecutionException = NULL;
jclass v8RuntimeExceptionCls = NULL;
jclass throwableCls = NULL;
jclass stringCls = NULL;
jclass integerCls = NULL;
jclass doubleCls = NULL;
jclass booleanCls = NULL;
jclass errorCls = NULL;
jclass unsupportedOperationExceptionCls = NULL;
jmethodID v8ArrayInitMethodID = NULL;
jmethodID v8TypedArrayInitMethodID = NULL;
jmethodID v8ArrayBufferInitMethodID = NULL;
jmethodID v8ArrayGetHandleMethodID = NULL;
jmethodID v8CallVoidMethodID = NULL;
jmethodID v8ObjectReleaseMethodID = NULL;
jmethodID v8DisposeMethodID = NULL;
jmethodID v8WeakReferenceReleased = NULL;
jmethodID v8ArrayReleaseMethodID = NULL;
jmethodID v8ObjectIsUndefinedMethodID = NULL;
jmethodID v8ObjectGetHandleMethodID = NULL;
jmethodID throwableGetMessageMethodID = NULL;
jmethodID integerIntValueMethodID = NULL;
jmethodID booleanBoolValueMethodID = NULL;
jmethodID doubleDoubleValueMethodID = NULL;
jmethodID v8CallObjectJavaMethodMethodID = NULL;
jmethodID v8ScriptCompilationInitMethodID = NULL;
jmethodID v8ScriptExecutionExceptionInitMethodID = NULL;
jmethodID undefinedV8ArrayInitMethodID = NULL;
jmethodID undefinedV8ObjectInitMethodID = NULL;
jmethodID integerInitMethodID = NULL;
jmethodID doubleInitMethodID = NULL;
jmethodID booleanInitMethodID = NULL;
jmethodID v8FunctionInitMethodID = NULL;
jmethodID v8ObjectInitMethodID = NULL;
jmethodID v8RuntimeExceptionInitMethodID = NULL;

v8::Isolate* isolate_;

void throwParseException(JNIEnv *env, Isolate* isolate, TryCatch* tryCatch);
void throwExecutionException(JNIEnv *env, Isolate* isolate, TryCatch* tryCatch, jlong v8RuntimePtr);
void throwError(JNIEnv *env, const char *message);
void throwV8RuntimeException(JNIEnv *env,  String::Value *message);
void throwResultUndefinedException(JNIEnv *env, const char *message);
Isolate* getIsolate(JNIEnv *env, jlong handle);
int getType(Handle<Value> v8Value);
jobject getResult(JNIEnv *env, jobject &v8, jlong v8RuntimePtr, Handle<Value> &result, jint expectedType);
Local<String> createV8String(JNIEnv *env, Isolate *isolate, jstring &string);

#define SETUP(env, v8RuntimePtr, errorReturnResult) getIsolate(env, v8RuntimePtr);\
    if ( isolate == NULL ) {\
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

void getJNIEnv(JNIEnv*& env) {
  int getEnvStat = jvm->GetEnv((void **)&env, JNI_VERSION_1_6);
  if (getEnvStat == JNI_EDETACHED) {
#ifdef __ANDROID_API__
    if (jvm->AttachCurrentThread(&env, NULL) != 0) {
#else
    if (jvm->AttachCurrentThread((void **)&env, NULL) != 0) {
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

jlong getHandle(JNIEnv* env, jobject object) {
  return env->CallLongMethod(object, v8ObjectGetHandleMethodID);
}

JNIEXPORT jstring JNICALL Java_com_eclipsesource_v8_V8__1getVersion (JNIEnv *env, jclass) {
  const char* utfString = v8::V8::GetVersion();
  return env->NewStringUTF(utfString);
}

Local<String> createV8String(JNIEnv *env, Isolate *isolate, jstring &string) {
  const uint16_t* unicodeString = env->GetStringChars(string, NULL);
  int length = env->GetStringLength(string);
  Local<String> result = String::NewFromTwoByte(isolate, unicodeString, String::NewStringType::kNormalString, length);
  env->ReleaseStringChars(string, unicodeString);
  return result;
}

Handle<Value> getValueWithKey(JNIEnv* env, Isolate* isolate, jlong &v8RuntimePtr, jlong &objectHandle, jstring &key) {
  Handle<Object> object = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  Local<String> v8Key = createV8String(env, isolate, key);
  return object->Get(v8Key);
}

void addValueWithKey(JNIEnv* env, Isolate* isolate, jlong &v8RuntimePtr, jlong &objectHandle, jstring &key, Handle<Value> value) {
  Handle<Object> object = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  const uint16_t* unicodeString_key = env->GetStringChars(key, NULL);
  int length = env->GetStringLength(key);
  Local<String> v8Key = String::NewFromTwoByte(isolate, unicodeString_key, String::NewStringType::kNormalString, length);
  object->Set(v8Key, value);
  env->ReleaseStringChars(key, unicodeString_key);
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
    if (env == NULL) {
        return onLoad_err;
    }
    v8::V8::InitializeICU();
    v8Platform = v8::platform::CreateDefaultPlatform();
    v8::V8::InitializePlatform(v8Platform);
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
        char const* str = env->GetStringUTFChars(v8flags, NULL);
        v8::V8::SetFlagsFromString(str, env->GetStringUTFLength(v8flags));
        env->ReleaseStringUTFChars(v8flags, str);
    }
    v8::V8::Initialize();
}

ShellArrayBufferAllocator array_buffer_allocator;

#ifdef NODE_COMPATIBLE
extern "C" {
    void _register_async_wrap(void);
    void _register_cares_wrap(void);
    void _register_fs_event_wrap(void);
    void _register_js_stream(void);
    void _register_buffer(void);
    void _register_config(void);
    void _register_contextify(void);
    void _register_crypto(void);
    void _register_fs(void);
    void _register_http_parser(void);
    void _register_icu(void);
    void _register_os(void);
    void _register_url(void);
    void _register_util(void);
    void _register_v8(void);
    void _register_zlib(void);
    void _register_pipe_wrap(void);
    void _register_process_wrap(void);
    void _register_signal_wrap(void);
    void _register_spawn_sync(void);
    void _register_stream_wrap(void);
    void _register_tcp_wrap(void);
    void _register_timer_wrap(void);
    void _register_tls_wrap(void);
    void _register_tty_wrap(void);
    void _register_udp_wrap(void);
    void _register_uv(void);
  }
#endif


JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1startNodeJS
  (JNIEnv * jniEnv, jclass, jlong v8RuntimePtr, jstring fileName) {
#ifdef NODE_COMPATIBLE
  Isolate* isolate = SETUP(jniEnv, v8RuntimePtr, );
  setvbuf(stderr, NULL, _IOLBF, 1024);
  const char* utfFileName = jniEnv->GetStringUTFChars(fileName, NULL);
  const char *argv[] = {"j2v8", utfFileName, NULL};
  int argc = sizeof(argv) / sizeof(char*) - 1;
  V8Runtime* rt = reinterpret_cast<V8Runtime*>(v8RuntimePtr);
  if (v8RuntimePtr == 1) {
  #if defined(_MSC_VER)
    // This is deadcode, but it ensures that libj2v8 'touches' all the
    // node modules. If the node modules are not 'touched' then the
    // linker will strip them out
    // @node-builtins-force-link
    _register_async_wrap();
    _register_cares_wrap();
    _register_fs_event_wrap();
    _register_js_stream();
    _register_buffer();
    _register_config();
    _register_contextify();
    _register_crypto();
    _register_fs();
    _register_http_parser();
    _register_icu();
    _register_os();
    _register_url();
    _register_util();
    _register_v8();
    _register_zlib();
    _register_pipe_wrap();
    _register_process_wrap();
    _register_signal_wrap();
    _register_spawn_sync();
    _register_stream_wrap();
    _register_tcp_wrap();
    _register_timer_wrap();
    _register_tls_wrap();
    _register_tty_wrap();
    _register_udp_wrap();
    _register_uv();
  #endif
  }
  rt->uvLoop = uv_default_loop();
  rt->isolateData = node::CreateIsolateData(isolate, rt->uvLoop);
  node::Environment* env = node::CreateEnvironment(rt->isolateData, context, argc, argv, 0, 0);
  node::LoadEnvironment(env);
  rt->nodeEnvironment = env;

  rt->running = true;
#endif
#ifndef NODE_COMPATIBLE
  (jniEnv)->ThrowNew(unsupportedOperationExceptionCls, "StartNodeJS Not Supported.");
#endif
}

JNIEXPORT jboolean JNICALL Java_com_eclipsesource_v8_V8__1pumpMessageLoop
  (JNIEnv * env, jclass, jlong v8RuntimePtr) {
#ifdef NODE_COMPATIBLE
  Isolate* isolate = SETUP(env, v8RuntimePtr, false);
  V8Runtime* rt = reinterpret_cast<V8Runtime*>(v8RuntimePtr);
  node::Environment* environment = rt->nodeEnvironment;
  SealHandleScope seal(isolate);
  v8::platform::PumpMessageLoop(v8Platform, isolate);
  rt->running = uv_run(rt->uvLoop, UV_RUN_ONCE);
  if (rt->running == false) {
    v8::platform::PumpMessageLoop(v8Platform, isolate);
    node::EmitBeforeExit(environment);
    // Emit `beforeExit` if the loop became alive either after emitting
    // event, or after running some callbacks.
    rt->running = uv_loop_alive(rt->uvLoop);
    if (uv_run(rt->uvLoop, UV_RUN_NOWAIT) != 0) {
      rt->running = true;
    }
  }
  return rt->running;
#endif
#ifndef NODE_COMPATIBLE
  (env)->ThrowNew(unsupportedOperationExceptionCls, "pumpMessageLoop Not Supported.");
  return false;
#endif
}

JNIEXPORT jboolean JNICALL Java_com_eclipsesource_v8_V8__1isRunning
  (JNIEnv *env, jclass, jlong v8RuntimePtr) {
 #ifdef NODE_COMPATIBLE
   Isolate* isolate = SETUP(env, v8RuntimePtr, false);
   V8Runtime* rt = reinterpret_cast<V8Runtime*>(v8RuntimePtr);
   return rt->running;
 #endif
 #ifndef NODE_COMPATIBLE
   (env)->ThrowNew(unsupportedOperationExceptionCls, "isRunning Not Supported.");
   return false;
 #endif
}

JNIEXPORT jboolean JNICALL Java_com_eclipsesource_v8_V8__1isNodeCompatible
  (JNIEnv *, jclass) {
 #ifdef NODE_COMPATIBLE
   return true;
 #else
   return false;
 #endif
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
  runtime->pendingException = NULL;
  HandleScope handle_scope(runtime->isolate);
  Handle<ObjectTemplate> globalObject = ObjectTemplate::New();
  Handle<Context> context_;
  if (globalAlias == NULL) {
    Handle<Context> context = Context::New(runtime->isolate, NULL, globalObject);
    context_ = context;
    runtime->context_.Reset(runtime->isolate, context);
    runtime->globalObject = new Persistent<Object>;
    runtime->globalObject->Reset(runtime->isolate, context->Global()->GetPrototype()->ToObject(runtime->isolate));
  }
  else {
    Local<String> utfAlias = createV8String(env, runtime->isolate, globalAlias);
    globalObject->SetAccessor(utfAlias, jsWindowObjectAccessor);
    Handle<Context> context = Context::New(runtime->isolate, NULL, globalObject);
    runtime->context_.Reset(runtime->isolate, context);
    runtime->globalObject = new Persistent<Object>;
    runtime->globalObject->Reset(runtime->isolate, context->Global()->GetPrototype()->ToObject(runtime->isolate));
  }

  delete(runtime->locker);
  return reinterpret_cast<jlong>(runtime);
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1acquireLock
  (JNIEnv *env, jobject, jlong v8RuntimePtr) {
  V8Runtime* runtime = reinterpret_cast<V8Runtime*>(v8RuntimePtr);
  if(runtime->isolate->InContext()) {
    jstring exceptionString = env->NewStringUTF("Cannot acquire lock while in a V8 Context");
    jthrowable exception = (jthrowable)env->NewObject(v8RuntimeExceptionCls, v8RuntimeExceptionInitMethodID, exceptionString);
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
    jthrowable exception = (jthrowable)env->NewObject(v8RuntimeExceptionCls, v8RuntimeExceptionInitMethodID, exceptionString);
    (env)->Throw(exception);
    env->DeleteLocalRef(exceptionString);
    return;
  }
  delete(runtime->locker);
  runtime->locker = NULL;
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1lowMemoryNotification
  (JNIEnv *env, jobject, jlong v8RuntimePtr) {
  V8Runtime* runtime = reinterpret_cast<V8Runtime*>(v8RuntimePtr);
  runtime->isolate->LowMemoryNotification();
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initEmptyContainer
(JNIEnv *env, jobject, jlong v8RuntimePtr) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Persistent<Object>* container = new Persistent<Object>;
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8Object
(JNIEnv *env, jobject, jlong v8RuntimePtr) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Local<Object> obj = Object::New(isolate);
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, obj);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1getGlobalObject
  (JNIEnv *env, jobject, jlong v8RuntimePtr) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  // Local<Object> obj = Object::New(isolate);
  return reinterpret_cast<jlong>(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->globalObject);
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1createTwin
  (JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jlong twinObjectHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  Handle<Object> obj = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  reinterpret_cast<Persistent<Object>*>(twinObjectHandle)->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, obj);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8Array
(JNIEnv *env, jobject, jlong v8RuntimePtr) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Local<Array> array = Array::New(isolate);
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, array);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8Int8Array
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong bufferHandle, jint offset, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<ArrayBuffer> arrayBuffer = Local<ArrayBuffer>::New(isolate, *reinterpret_cast<Persistent<ArrayBuffer>*>(bufferHandle));
  Local<Int8Array> array = Int8Array::New(arrayBuffer, offset, length);
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, array);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8UInt8Array
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong bufferHandle, jint offset, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<ArrayBuffer> arrayBuffer = Local<ArrayBuffer>::New(isolate, *reinterpret_cast<Persistent<ArrayBuffer>*>(bufferHandle));
  Local<Uint8Array> array = Uint8Array::New(arrayBuffer, offset, length);
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, array);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8UInt8ClampedArray
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong bufferHandle, jint offset, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<ArrayBuffer> arrayBuffer = Local<ArrayBuffer>::New(isolate, *reinterpret_cast<Persistent<ArrayBuffer>*>(bufferHandle));
  Local<Uint8ClampedArray> array = Uint8ClampedArray::New(arrayBuffer, offset, length);
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, array);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8Int32Array
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong bufferHandle, jint offset, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<ArrayBuffer> arrayBuffer = Local<ArrayBuffer>::New(isolate, *reinterpret_cast<Persistent<ArrayBuffer>*>(bufferHandle));
  Local<Int32Array> array = Int32Array::New(arrayBuffer, offset, length);
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, array);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8UInt32Array
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong bufferHandle, jint offset, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<ArrayBuffer> arrayBuffer = Local<ArrayBuffer>::New(isolate, *reinterpret_cast<Persistent<ArrayBuffer>*>(bufferHandle));
  Local<Uint32Array> array = Uint32Array::New(arrayBuffer, offset, length);
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, array);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8UInt16Array
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong bufferHandle, jint offset, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<ArrayBuffer> arrayBuffer = Local<ArrayBuffer>::New(isolate, *reinterpret_cast<Persistent<ArrayBuffer>*>(bufferHandle));
  Local<Uint16Array> array = Uint16Array::New(arrayBuffer, offset, length);
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, array);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8Int16Array
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong bufferHandle, jint offset, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<ArrayBuffer> arrayBuffer = Local<ArrayBuffer>::New(isolate, *reinterpret_cast<Persistent<ArrayBuffer>*>(bufferHandle));
  Local<Int16Array> array = Int16Array::New(arrayBuffer, offset, length);
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, array);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8Float32Array
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong bufferHandle, jint offset, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<ArrayBuffer> arrayBuffer = Local<ArrayBuffer>::New(isolate, *reinterpret_cast<Persistent<ArrayBuffer>*>(bufferHandle));
  Local<Float32Array> array = Float32Array::New(arrayBuffer, offset, length);
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, array);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8Float64Array
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong bufferHandle, jint offset, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<ArrayBuffer> arrayBuffer = Local<ArrayBuffer>::New(isolate, *reinterpret_cast<Persistent<ArrayBuffer>*>(bufferHandle));
  Local<Float64Array> array = Float64Array::New(arrayBuffer, offset, length);
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, array);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8ArrayBuffer__JI
(JNIEnv *env, jobject, jlong v8RuntimePtr, jint capacity) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Local<ArrayBuffer> arrayBuffer = ArrayBuffer::New(isolate, capacity);
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, arrayBuffer);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1initNewV8ArrayBuffer__JLjava_nio_ByteBuffer_2I
(JNIEnv *env, jobject, jlong v8RuntimePtr, jobject byteBuffer, jint capacity) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Local<ArrayBuffer> arrayBuffer = ArrayBuffer::New(isolate, env->GetDirectBufferAddress(byteBuffer), capacity);
  Persistent<Object>* container = new Persistent<Object>;
  container->Reset(reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate, arrayBuffer);
  return reinterpret_cast<jlong>(container);
}

JNIEXPORT jobject JNICALL Java_com_eclipsesource_v8_V8__1createV8ArrayBufferBackingStore
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jint capacity) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<ArrayBuffer> arrayBuffer = Local<ArrayBuffer>::New(isolate, *reinterpret_cast<Persistent<ArrayBuffer>*>(objectHandle));
  void* dataPtr = arrayBuffer->GetContents().Data();
  jobject byteBuffer = env->NewDirectByteBuffer(arrayBuffer->GetContents().Data(), capacity);
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
	V8::TerminateExecution(isolate);
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
  delete(reinterpret_cast<V8Runtime*>(v8RuntimePtr));
}

JNIEXPORT jboolean JNICALL Java_com_eclipsesource_v8_V8__1contains
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, false);
  Handle<Object> object = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  Local<String> v8Key = createV8String(env, isolate, key);
  return object->Has(v8Key);
}

JNIEXPORT jobjectArray JNICALL Java_com_eclipsesource_v8_V8__1getKeys
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL);
  Handle<Object> object = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  Local<Array> properties = object->GetOwnPropertyNames();
  int size = properties->Length();
  jobjectArray keys = (env)->NewObjectArray(size, stringCls, NULL);
  for (int i = 0; i < size; i++) {
    String::Value unicodeString(properties->Get(i)->ToString(isolate));
    jobject key = (env)->NewString(*unicodeString, unicodeString.length());
    (env)->SetObjectArrayElement(keys, i, key);
    (env)->DeleteLocalRef(key);
  }
  return keys;
}

ScriptOrigin* createScriptOrigin(JNIEnv * env, Isolate* isolate, jstring &jscriptName, jint jlineNumber = 0) {
  Local<String> scriptName = createV8String(env, isolate, jscriptName);
  return new ScriptOrigin(scriptName, Integer::New(isolate, jlineNumber));
}

bool compileScript(Isolate *isolate, jstring &jscript, JNIEnv *env, jstring jscriptName, jint &jlineNumber, Local<Script> &script, TryCatch* tryCatch) {
  Local<String> source = createV8String(env, isolate, jscript);
  ScriptOrigin* scriptOriginPtr = NULL;
  if (jscriptName != NULL) {
    scriptOriginPtr = createScriptOrigin(env, isolate, jscriptName, jlineNumber);
  }
  script = Script::Compile(source, scriptOriginPtr);
  if (scriptOriginPtr != NULL) {
    delete(scriptOriginPtr);
  }
  if (tryCatch->HasCaught()) {
    throwParseException(env, isolate, tryCatch);
    return false;
  }
  return true;
}

bool runScript(Isolate* isolate, JNIEnv *env, Local<Script> *script, TryCatch* tryCatch, jlong v8RuntimePtr) {
  (*script)->Run();
  if (tryCatch->HasCaught()) {
    throwExecutionException(env, isolate, tryCatch, v8RuntimePtr);
    return false;
  }
  return true;
}

bool runScript(Isolate* isolate, JNIEnv *env, Local<Script> *script, TryCatch* tryCatch, Local<Value> &result, jlong v8RuntimePtr) {
  result = (*script)->Run();
  if (tryCatch->HasCaught()) {
    throwExecutionException(env, isolate, tryCatch, v8RuntimePtr);
    return false;
  }
  return true;
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1executeVoidScript
(JNIEnv * env, jobject v8, jlong v8RuntimePtr, jstring jjstring, jstring jscriptName = NULL, jint jlineNumber = 0) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  TryCatch tryCatch(isolate);
  Local<Script> script;
  if (!compileScript(isolate, jjstring, env, jscriptName, jlineNumber, script, &tryCatch))
    return;
  runScript(isolate, env, &script, &tryCatch, v8RuntimePtr);
}

JNIEXPORT jdouble JNICALL Java_com_eclipsesource_v8_V8__1executeDoubleScript
(JNIEnv * env, jobject v8, jlong v8RuntimePtr, jstring jjstring, jstring jscriptName = NULL, jint jlineNumber = 0) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  TryCatch tryCatch(isolate);
  Local<Script> script;
  Local<Value> result;
  if (!compileScript(isolate, jjstring, env, jscriptName, jlineNumber, script, &tryCatch))
    return 0;
  if (!runScript(isolate, env, &script, &tryCatch, result, v8RuntimePtr))
    return 0;
  ASSERT_IS_NUMBER(result);
  return result->NumberValue();
}

JNIEXPORT jboolean JNICALL Java_com_eclipsesource_v8_V8__1executeBooleanScript
(JNIEnv *env, jobject v8, jlong v8RuntimePtr, jstring jjstring, jstring jscriptName = NULL, jint jlineNumber = 0) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, false);
  TryCatch tryCatch(isolate);
  Local<Script> script;
  Local<Value> result;
  if (!compileScript(isolate, jjstring, env, jscriptName, jlineNumber, script, &tryCatch))
    return false;
  if (!runScript(isolate, env, &script, &tryCatch, result, v8RuntimePtr))
    return false;
  ASSERT_IS_BOOLEAN(result);
  return result->BooleanValue();
}

JNIEXPORT jstring JNICALL Java_com_eclipsesource_v8_V8__1executeStringScript
(JNIEnv *env, jobject v8, jlong v8RuntimePtr, jstring jjstring, jstring jscriptName = NULL, jint jlineNumber = 0) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL);
  TryCatch tryCatch(isolate);
  Local<Script> script;
  Local<Value> result;
  if (!compileScript(isolate, jjstring, env, jscriptName, jlineNumber, script, &tryCatch))
    return NULL;
  if (!runScript(isolate, env, &script, &tryCatch, result, v8RuntimePtr))
    return NULL;
  ASSERT_IS_STRING(result);
  String::Value unicodeString(result->ToString(isolate));

  return env->NewString(*unicodeString, unicodeString.length());
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1executeIntegerScript
(JNIEnv * env, jobject v8, jlong v8RuntimePtr, jstring jjstring, jstring jscriptName = NULL, jint jlineNumber = 0) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  TryCatch tryCatch(isolate);
  Local<Script> script;
  Local<Value> result;
  if (!compileScript(isolate, jjstring, env, jscriptName, jlineNumber, script, &tryCatch))
    return 0;
  if (!runScript(isolate, env, &script, &tryCatch, result, v8RuntimePtr))
    return 0;
  ASSERT_IS_NUMBER(result);
  return result->Int32Value();
}

JNIEXPORT jobject JNICALL Java_com_eclipsesource_v8_V8__1executeScript
(JNIEnv *env, jobject v8, jlong v8RuntimePtr, jint expectedType, jstring jjstring, jstring jscriptName = NULL, jint jlineNumber = 0) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL);
  TryCatch tryCatch(isolate);
  Local<Script> script;
  Local<Value> result;
  if (!compileScript(isolate, jjstring, env, jscriptName, jlineNumber, script, &tryCatch)) { return NULL; }
  if (!runScript(isolate, env, &script, &tryCatch, result, v8RuntimePtr)) { return NULL; }
  return getResult(env, v8, v8RuntimePtr, result, expectedType);
}

bool invokeFunction(JNIEnv *env, Isolate* isolate, jlong &v8RuntimePtr, jlong &receiverHandle, jlong &functionHandle, jlong &parameterHandle, Handle<Value> &result) {
  int size = 0;
  Handle<Value>* args = NULL;
  if (parameterHandle != 0) {
    Handle<Object> parameters = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(parameterHandle));
    size = Array::Cast(*parameters)->Length();
    args = new Handle<Value>[size];
    for (int i = 0; i < size; i++) {
      args[i] = parameters->Get(i);
    }
  }
  Handle<Object> object = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(functionHandle));
  Handle<Object> receiver = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(receiverHandle));
  Handle<Function> func = Handle<Function>::Cast(object);
  TryCatch tryCatch(isolate);
  result = func->Call(receiver, size, args);
  if (args != NULL) {
    delete(args);
  }
  if (tryCatch.HasCaught()) {
    throwExecutionException(env, isolate, &tryCatch, v8RuntimePtr);
    return false;
  }
  return true;
}

bool invokeFunction(JNIEnv *env, Isolate* isolate, jlong &v8RuntimePtr, jlong &objectHandle, jstring &jfunctionName, jlong &parameterHandle, Handle<Value> &result) {
  Local<String> functionName = createV8String(env, isolate, jfunctionName);
  Handle<Object> parentObject = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  int size = 0;
  Handle<Value>* args = NULL;
  if (parameterHandle != 0) {
    Handle<Object> parameters = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(parameterHandle));
    size = Array::Cast(*parameters)->Length();
    args = new Handle<Value>[size];
    for (int i = 0; i < size; i++) {
      args[i] = parameters->Get(i);
    }
  }
  Handle<Value> value = parentObject->Get(functionName);
  Handle<Function> func = Handle<Function>::Cast(value);
  TryCatch tryCatch(isolate);
  result = func->Call(parentObject, size, args);
  if (args != NULL) {
    delete(args);
  }
  if (tryCatch.HasCaught()) {
    throwExecutionException(env, isolate, &tryCatch, v8RuntimePtr);
    return false;
  }
  return true;
}

JNIEXPORT jobject JNICALL Java_com_eclipsesource_v8_V8__1executeFunction__JJJJ
  (JNIEnv *env, jobject v8, jlong v8RuntimePtr, jlong receiverHandle, jlong functionHandle, jlong parameterHandle) {
    Isolate* isolate = SETUP(env, v8RuntimePtr, NULL);
  Handle<Value> result;
  if (!invokeFunction(env, isolate, v8RuntimePtr, receiverHandle, functionHandle, parameterHandle, result))
    return NULL;
  return getResult(env, v8, v8RuntimePtr, result, com_eclipsesource_v8_V8_UNKNOWN);
}

JNIEXPORT jobject JNICALL Java_com_eclipsesource_v8_V8__1executeFunction__JIJLjava_lang_String_2J
(JNIEnv *env, jobject v8, jlong v8RuntimePtr, jint expectedType, jlong objectHandle, jstring jfunctionName, jlong parameterHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL);
  Handle<Value> result;
  if (!invokeFunction(env, isolate, v8RuntimePtr, objectHandle, jfunctionName, parameterHandle, result))
    return NULL;
  return getResult(env, v8, v8RuntimePtr, result, expectedType);
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1executeIntegerFunction
(JNIEnv *env, jobject v8, jlong v8RuntimePtr, jlong objectHandle, jstring jfunctionName, jlong parameterHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Value> result;
  if (!invokeFunction(env, isolate, v8RuntimePtr, objectHandle, jfunctionName, parameterHandle, result))
    return 0;
  ASSERT_IS_NUMBER(result);
  return result->Int32Value();
}

JNIEXPORT jdouble JNICALL Java_com_eclipsesource_v8_V8__1executeDoubleFunction
(JNIEnv *env, jobject v8, jlong v8RuntimePtr, jlong objectHandle, jstring jfunctionName, jlong parameterHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Value> result;
  if (!invokeFunction(env, isolate, v8RuntimePtr, objectHandle, jfunctionName, parameterHandle, result))
    return 0;
  ASSERT_IS_NUMBER(result);
  return result->NumberValue();
}

JNIEXPORT jboolean JNICALL Java_com_eclipsesource_v8_V8__1executeBooleanFunction
(JNIEnv *env, jobject v8, jlong v8RuntimePtr, jlong objectHandle, jstring jfunctionName, jlong parameterHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, false);
  Handle<Value> result;
  if (!invokeFunction(env, isolate, v8RuntimePtr, objectHandle, jfunctionName, parameterHandle, result))
    return false;
  ASSERT_IS_BOOLEAN(result);
  return result->BooleanValue();
}

JNIEXPORT jstring JNICALL Java_com_eclipsesource_v8_V8__1executeStringFunction
(JNIEnv *env, jobject v8, jlong v8RuntimePtr, jlong objectHandle, jstring jfunctionName, jlong parameterHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL);
  Handle<Value> result;
  if (!invokeFunction(env, isolate, v8RuntimePtr, objectHandle, jfunctionName, parameterHandle, result))
    return NULL;
  ASSERT_IS_STRING(result);
  String::Value unicodeString(result->ToString(isolate));

  return env->NewString(*unicodeString, unicodeString.length());
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1executeVoidFunction
(JNIEnv *env, jobject v8, jlong v8RuntimePtr, jlong objectHandle, jstring jfunctionName, jlong parameterHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  Handle<Value> result;
  invokeFunction(env, isolate, v8RuntimePtr, objectHandle, jfunctionName, parameterHandle, result);
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1addUndefined
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  addValueWithKey(env, isolate, v8RuntimePtr, objectHandle, key, Undefined(isolate));
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1addNull
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  addValueWithKey(env, isolate, v8RuntimePtr, objectHandle, key, Null(isolate));
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1add__JJLjava_lang_String_2I
(JNIEnv * env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key, jint value) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  addValueWithKey(env, isolate, v8RuntimePtr, objectHandle, key, Int32::New(isolate, value));
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1add__JJLjava_lang_String_2D
(JNIEnv * env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key, jdouble value) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  addValueWithKey(env, isolate, v8RuntimePtr, objectHandle, key, Number::New(isolate, value));
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1add__JJLjava_lang_String_2Ljava_lang_String_2
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key, jstring value) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  Handle<Value> v8Value = createV8String(env, isolate, value);
  addValueWithKey(env, isolate, v8RuntimePtr, objectHandle, key, v8Value);
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1add__JJLjava_lang_String_2Z
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key, jboolean value) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  addValueWithKey(env, isolate, v8RuntimePtr, objectHandle, key, Boolean::New(isolate, value));
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1addObject
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key, jlong valueHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  Handle<Value> value = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(valueHandle));
  addValueWithKey(env, isolate, v8RuntimePtr, objectHandle, key, value);
}

JNIEXPORT jobject JNICALL Java_com_eclipsesource_v8_V8__1get
(JNIEnv *env, jobject v8, jlong v8RuntimePtr, jint expectedType, jlong objectHandle, jstring key) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL);
  Handle<Value> result = getValueWithKey(env, isolate, v8RuntimePtr, objectHandle, key);
  return getResult(env, v8, v8RuntimePtr, result, expectedType);
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1getInteger
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Value> v8Value = getValueWithKey(env, isolate, v8RuntimePtr, objectHandle, key);
  ASSERT_IS_NUMBER(v8Value);
  return v8Value->Int32Value();
}

JNIEXPORT jdouble JNICALL Java_com_eclipsesource_v8_V8__1getDouble
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Value> v8Value = getValueWithKey(env, isolate, v8RuntimePtr, objectHandle, key);
  ASSERT_IS_NUMBER(v8Value);
  return v8Value->NumberValue();
}

JNIEXPORT jstring JNICALL Java_com_eclipsesource_v8_V8__1getString
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Value> v8Value = getValueWithKey(env, isolate, v8RuntimePtr, objectHandle, key);
  ASSERT_IS_STRING(v8Value);
  String::Value unicode(v8Value->ToString(isolate));

  return env->NewString(*unicode, unicode.length());
}

JNIEXPORT jboolean JNICALL Java_com_eclipsesource_v8_V8__1getBoolean
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, false);
  Handle<Value> v8Value = getValueWithKey(env, isolate, v8RuntimePtr, objectHandle, key);
  ASSERT_IS_BOOLEAN(v8Value);
  return v8Value->BooleanValue();
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1getType__JJLjava_lang_String_2
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring key) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Value> v8Value = getValueWithKey(env, isolate, v8RuntimePtr, objectHandle, key);
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
    int type = getType(array->Get(index));
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
  Handle<Value> v8Value = array->Get(index);
  ASSERT_IS_NUMBER(v8Value);
  return v8Value->Int32Value();
}

int fillIntArray(JNIEnv *env, Handle<Object> &array, int start, int length, jintArray &result) {
  jint * fill = new jint[length];
  for (int i = start; i < start + length; i++) {
    Handle<Value> v8Value = array->Get(i);
    ASSERT_IS_NUMBER(v8Value);
    fill[i - start] = v8Value->Int32Value();
  }
  (env)->SetIntArrayRegion(result, 0, length, fill);
  delete[] fill;
  return length;
}

int fillDoubleArray(JNIEnv *env, Handle<Object> &array, int start, int length, jdoubleArray &result) {
  jdouble * fill = new jdouble[length];
  for (int i = start; i < start + length; i++) {
    Handle<Value> v8Value = array->Get(i);
    ASSERT_IS_NUMBER(v8Value);
    fill[i - start] = v8Value->NumberValue();
  }
  (env)->SetDoubleArrayRegion(result, 0, length, fill);
  delete[] fill;
  return length;
}

int fillByteArray(JNIEnv *env, Handle<Object> &array, int start, int length, jbyteArray &result) {
  jbyte * fill = new jbyte[length];
  for (int i = start; i < start + length; i++) {
    Handle<Value> v8Value = array->Get(i);
    ASSERT_IS_NUMBER(v8Value);
    fill[i - start] = (jbyte)v8Value->Int32Value();
  }
  (env)->SetByteArrayRegion(result, 0, length, fill);
  delete[] fill;
  return length;
}

int fillBooleanArray(JNIEnv *env, Handle<Object> &array, int start, int length, jbooleanArray &result) {
  jboolean * fill = new jboolean[length];
  for (int i = start; i < start + length; i++) {
    Handle<Value> v8Value = array->Get(i);
    ASSERT_IS_BOOLEAN(v8Value);
    fill[i - start] = v8Value->BooleanValue();
  }
  (env)->SetBooleanArrayRegion(result, 0, length, fill);
  delete[] fill;
  return length;
}

int fillStringArray(JNIEnv *env, Isolate* isolate, Handle<Object> &array, int start, int length, jobjectArray &result) {
  for (int i = start; i < start + length; i++) {
    Handle<Value> v8Value = array->Get(i);
    ASSERT_IS_STRING(v8Value);
    String::Value unicodeString(v8Value->ToString(isolate));
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

  return fillIntArray(env, array, start, length, result);
}

JNIEXPORT jintArray JNICALL Java_com_eclipsesource_v8_V8__1arrayGetIntegers__JJII
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint start, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  jintArray result = env->NewIntArray(length);
  fillIntArray(env, array, start, length, result);

  return result;
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1arrayGetDoubles__JJII_3D
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint start, jint length, jdoubleArray result) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  return fillDoubleArray(env, array, start, length, result);
}

JNIEXPORT jdoubleArray JNICALL Java_com_eclipsesource_v8_V8__1arrayGetDoubles__JJII
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint start, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  jdoubleArray result = env->NewDoubleArray(length);
  fillDoubleArray(env, array, start, length, result);
  return result;
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1arrayGetBooleans__JJII_3Z
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint start, jint length, jbooleanArray result) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  return fillBooleanArray(env, array, start, length, result);
}

JNIEXPORT jbyteArray JNICALL Java_com_eclipsesource_v8_V8__1arrayGetBytes__JJII
  (JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint start, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  jbyteArray result = env->NewByteArray(length);
  fillByteArray(env, array, start, length, result);
  return result;
}

JNIEXPORT jbooleanArray JNICALL Java_com_eclipsesource_v8_V8__1arrayGetBooleans__JJII
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint start, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  jbooleanArray result = env->NewBooleanArray(length);
  fillBooleanArray(env, array, start, length, result);
  return result;
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1arrayGetBytes__JJII_3B
  (JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint start, jint length, jbyteArray result) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  return fillByteArray(env, array, start, length, result);
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1arrayGetStrings__JJII_3Ljava_lang_String_2
(JNIEnv * env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint start, jint length, jobjectArray result) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));

  return fillStringArray(env, isolate, array, start, length, result);
}

JNIEXPORT jobjectArray JNICALL Java_com_eclipsesource_v8_V8__1arrayGetStrings__JJII
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint start, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  jobjectArray result = env->NewObjectArray(length, stringCls, NULL);
  fillStringArray(env, isolate, array, start, length, result);

  return result;
}

JNIEXPORT jboolean JNICALL Java_com_eclipsesource_v8_V8__1arrayGetBoolean
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint index) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, false);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  Handle<Value> v8Value = array->Get(index);
  ASSERT_IS_BOOLEAN(v8Value);
  return v8Value->BooleanValue();
}

JNIEXPORT jbyte JNICALL Java_com_eclipsesource_v8_V8__1arrayGetByte
  (JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint index) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, false);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  Handle<Value> v8Value = array->Get(index);
  ASSERT_IS_NUMBER(v8Value);
  return v8Value->Int32Value();
}

JNIEXPORT jdouble JNICALL Java_com_eclipsesource_v8_V8__1arrayGetDouble
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint index) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  Handle<Value> v8Value = array->Get(index);
  ASSERT_IS_NUMBER(v8Value);
  return v8Value->NumberValue();
}

JNIEXPORT jstring JNICALL Java_com_eclipsesource_v8_V8__1arrayGetString
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint index) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  Handle<Value> v8Value = array->Get(index);
  ASSERT_IS_STRING(v8Value);
  String::Value unicodeString(v8Value->ToString(isolate));

  return env->NewString(*unicodeString, unicodeString.length());
}

JNIEXPORT jobject JNICALL Java_com_eclipsesource_v8_V8__1arrayGet
(JNIEnv *env, jobject v8, jlong v8RuntimePtr, jint expectedType, jlong arrayHandle, jint index) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, NULL);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  Handle<Value> result = array->Get(index);
  return getResult(env, v8, v8RuntimePtr, result, expectedType);
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1addArrayNullItem
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  if ( array->IsTypedArray() ) {
     Local<String> string = String::NewFromUtf8(isolate, "Cannot push to a Typed Array.");
     v8::String::Value strValue(string);
     throwV8RuntimeException(env, &strValue);
     return;
  }
  int index = Array::Cast(*array)->Length();
  array->Set(index, Null(isolate));
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1addArrayUndefinedItem
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  if ( array->IsTypedArray() ) {
     Local<String> string = String::NewFromUtf8(isolate, "Cannot push to a Typed Array.");
     v8::String::Value strValue(string);
     throwV8RuntimeException(env, &strValue);
     return;
  }
  int index = Array::Cast(*array)->Length();
  array->Set(index, Undefined(isolate));
}


JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1addArrayIntItem
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint value) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  if ( array->IsTypedArray() ) {
     Local<String> string = String::NewFromUtf8(isolate, "Cannot push to a Typed Array.");
     v8::String::Value strValue(string);
     throwV8RuntimeException(env, &strValue);
     return;
  }
  Local<Value> v8Value = Int32::New(isolate, value);
  int index = Array::Cast(*array)->Length();
  array->Set(index, v8Value);
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1addArrayDoubleItem
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jdouble value) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  if ( array->IsTypedArray() ) {
     Local<String> string = String::NewFromUtf8(isolate, "Cannot push to a Typed Array.");
     v8::String::Value strValue(string);
     throwV8RuntimeException(env, &strValue);
     return;
  }
  Local<Value> v8Value = Number::New(isolate, value);
  int index = Array::Cast(*array)->Length();
  array->Set(index, v8Value);
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1addArrayBooleanItem
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jboolean value) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  if ( array->IsTypedArray() ) {
     Local<String> string = String::NewFromUtf8(isolate, "Cannot push to a Typed Array.");
     v8::String::Value strValue(string);
     throwV8RuntimeException(env, &strValue);
     return;
  }
  Local<Value> v8Value = Boolean::New(isolate, value);
  int index = Array::Cast(*array)->Length();
  array->Set(index, v8Value);
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1addArrayStringItem
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jstring value) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  if ( array->IsTypedArray() ) {
     Local<String> string = String::NewFromUtf8(isolate, "Cannot push to a Typed Array.");
     v8::String::Value strValue(string);
     throwV8RuntimeException(env, &strValue);
     return;
  }
  int index = Array::Cast(*array)->Length();
  Local<String> v8Value = createV8String(env, isolate, value);
  array->Set(index, v8Value);
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1addArrayObjectItem
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jlong valueHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  if ( array->IsTypedArray() ) {
     Local<String> string = String::NewFromUtf8(isolate, "Cannot push to a Typed Array.");
     v8::String::Value strValue(string);
     throwV8RuntimeException(env, &strValue);
     return;
  }
  int index = Array::Cast(*array)->Length();
  Local<Value> v8Value = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(valueHandle));
  array->Set(index, v8Value);
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
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  Handle<Value> v8Value = array->Get(index);
  int type = getType(v8Value);
  if (type < 0) {
    throwResultUndefinedException(env, "");
  }
  return type;
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1getType__JJ
  (JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Value> v8Value = Local<Value>::New(isolate, *reinterpret_cast<Persistent<Value>*>(objectHandle));
  return getType(v8Value);
}

JNIEXPORT jint JNICALL Java_com_eclipsesource_v8_V8__1getType__JJII
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong arrayHandle, jint start, jint length) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Object> array = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(arrayHandle));
  int result = -1;
  for (int i = start; i < start + length; i++) {
    Handle<Value> v8Value = array->Get(i);
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

jobject createParameterArray(JNIEnv* env, jlong v8RuntimePtr, jobject v8, int size, const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = getIsolate(env, v8RuntimePtr);
  jobject result = env->NewObject(v8ArrayCls, v8ArrayInitMethodID, v8);
  jlong parameterHandle = env->CallLongMethod(result, v8ArrayGetHandleMethodID);
  Handle<Object> parameters = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(parameterHandle));
  for (int i = 0; i < size; i++) {
    parameters->Set(i, args[i]);
  }
  return result;
}

void voidCallback(const FunctionCallbackInfo<Value>& args) {
  int size = args.Length();
  Local<External> data = Local<External>::Cast(args.Data());
  void *methodDescriptorPtr = data->Value();
  MethodDescriptor* md = static_cast<MethodDescriptor*>(methodDescriptorPtr);
  jobject v8 = reinterpret_cast<V8Runtime*>(md->v8RuntimePtr)->v8;
  Isolate* isolate = reinterpret_cast<V8Runtime*>(md->v8RuntimePtr)->isolate;
  Isolate::Scope isolateScope(isolate);
  JNIEnv * env;
  getJNIEnv(env);
  jobject parameters = createParameterArray(env, md->v8RuntimePtr, v8, size, args);
  Handle<Value> receiver = args.This();
  jobject jreceiver = getResult(env, v8, md->v8RuntimePtr, receiver, com_eclipsesource_v8_V8_UNKNOWN);
  env->CallVoidMethod(v8, v8CallVoidMethodID, md->methodID, jreceiver, parameters);
  if (env->ExceptionCheck()) {
    Isolate* isolate = getIsolate(env, md->v8RuntimePtr);
    reinterpret_cast<V8Runtime*>(md->v8RuntimePtr)->pendingException = env->ExceptionOccurred();
    env->ExceptionClear();
    jstring exceptionMessage = (jstring)env->CallObjectMethod(reinterpret_cast<V8Runtime*>(md->v8RuntimePtr)->pendingException, throwableGetMessageMethodID);
    if (exceptionMessage != NULL) {
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
  jobject v8 = reinterpret_cast<V8Runtime*>(md->v8RuntimePtr)->v8;
  Isolate* isolate = reinterpret_cast<V8Runtime*>(md->v8RuntimePtr)->isolate;
  Isolate::Scope isolateScope(isolate);
  JNIEnv * env;
  getJNIEnv(env);
  jobject parameters = createParameterArray(env, md->v8RuntimePtr, v8, size, args);
  Handle<Value> receiver = args.This();
  jobject jreceiver = getResult(env, v8, md->v8RuntimePtr, receiver, com_eclipsesource_v8_V8_UNKNOWN);
  jobject resultObject = env->CallObjectMethod(v8, v8CallObjectJavaMethodMethodID, md->methodID, jreceiver, parameters);
  if (env->ExceptionCheck()) {
    resultObject = NULL;
    Isolate* isolate = getIsolate(env, md->v8RuntimePtr);
    reinterpret_cast<V8Runtime*>(md->v8RuntimePtr)->pendingException = env->ExceptionOccurred();
    env->ExceptionClear();
    jstring exceptionMessage = (jstring)env->CallObjectMethod(reinterpret_cast<V8Runtime*>(md->v8RuntimePtr)->pendingException, throwableGetMessageMethodID);
    if (exceptionMessage != NULL) {
      Local<String> v8String = createV8String(env, isolate, exceptionMessage);
      isolate->ThrowException(v8String);
    }
    else {
      isolate->ThrowException(String::NewFromUtf8(isolate, "Unhandled Java Exception"));
    }
  }
  else if (resultObject == NULL) {
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
  if (resultObject != NULL) {
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
  isolate->IdleNotification(1000);
  pext.SetWeak(md, [](v8::WeakCallbackInfo<MethodDescriptor> const& data) {
    MethodDescriptor* md = data.GetParameter();
    jobject v8 = reinterpret_cast<V8Runtime*>(md->v8RuntimePtr)->v8;
    JNIEnv * env;
    getJNIEnv(env);
    env->CallVoidMethod(v8, v8DisposeMethodID, md->methodID);
    delete(md);
  }, WeakCallbackType::kParameter);

  Local<Function> function = Function::New(isolate, objectCallback, ext);
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
bool CHECK_GL_ERRORS = true;

#define GL_ERROR_THROW(x) \
if (CHECK_GL_ERRORS) { \
  GLenum err = glGetError(); \
  if (err != GL_NO_ERROR) { \
      x->ThrowException(v8::Exception::Error(String::NewFromUtf8(x, GLErrorToString(err))));\
  } \
} \

void v8Bind_ActiveTexture (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glActiveTexture((GLenum)arg0->Int32Value());
    GL_ERROR_THROW(isolate);
}

void v8Bind_AttachShader (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glAttachShader((GLuint)arg0->Int32Value(),(GLuint)arg1->Int32Value());
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

    glBindAttribLocation((GLuint)arg0->Int32Value(),(GLuint)arg1->Int32Value(),*String::Utf8Value(arg2));
    GL_ERROR_THROW(isolate);
}

void v8Bind_BindBuffer (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glBindBuffer((GLenum)arg0->Int32Value(),(GLuint)arg1->Int32Value());
    GL_ERROR_THROW(isolate);
}

void v8Bind_BindFramebuffer (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glBindFramebuffer((GLenum)arg0->Int32Value(),(GLuint)arg1->Int32Value());
    GL_ERROR_THROW(isolate);
}

void v8Bind_BindRenderbuffer (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glBindRenderbuffer((GLenum)arg0->Int32Value(),(GLuint)arg1->Int32Value());
    GL_ERROR_THROW(isolate);
}

void v8Bind_BindTexture (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLenum type = (GLenum)arg0->Int32Value();
    GLuint handle = (GLuint)arg1->Uint32Value();

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

    glBlendColor((GLclampf)arg0->NumberValue(),(GLclampf)arg1->NumberValue(),(GLclampf)arg2->NumberValue(),(GLclampf)arg3->NumberValue());
    GL_ERROR_THROW(isolate);
}

void v8Bind_BlendEquation (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glBlendEquation((GLenum)arg0->Int32Value());
    GL_ERROR_THROW(isolate);
}

void v8Bind_BlendEquationSeparate (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glBlendEquationSeparate((GLenum)arg0->Int32Value(),(GLenum)arg1->Int32Value());
    GL_ERROR_THROW(isolate);
}

void v8Bind_BlendFunc (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glBlendFunc((GLenum)arg0->Int32Value(),(GLenum)arg1->Int32Value());
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

    glBlendFuncSeparate((GLenum)arg0->Int32Value(),(GLenum)arg1->Int32Value(),(GLenum)arg2->Int32Value(),(GLenum)arg3->Int32Value());
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

    glBufferData((GLenum)arg0->Int32Value(),(GLsizeiptr)arg1->Int32Value(),ptr0,(GLenum)arg3->Int32Value());
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

    glBufferSubData((GLenum)arg0->Int32Value(),(GLintptr)arg1->Int32Value(),(GLsizeiptr)arg2->Int32Value(),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_CheckFramebufferStatus (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLenum ret=  glCheckFramebufferStatus((GLenum)arg0->Int32Value());
    GL_ERROR_THROW(isolate);
    args.GetReturnValue().Set( v8::Integer::New( isolate, ret ) );
}

void v8Bind_Clear (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glClear((GLbitfield)arg0->Int32Value());
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

    glClearColor((GLclampf)arg0->NumberValue(),(GLclampf)arg1->NumberValue(),(GLclampf)arg2->NumberValue(),(GLclampf)arg3->NumberValue());
    GL_ERROR_THROW(isolate);
}

void v8Bind_ClearDepthf (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsNumber()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glClearDepthf((GLclampf)arg0->NumberValue());
    GL_ERROR_THROW(isolate);
}

void v8Bind_ClearStencil (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glClearStencil((GLint)arg0->Int32Value());
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

    glColorMask((GLboolean)arg0->BooleanValue(),(GLboolean)arg1->BooleanValue(),(GLboolean)arg2->BooleanValue(),(GLboolean)arg3->BooleanValue());
    GL_ERROR_THROW(isolate);
}

void v8Bind_CompileShader (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glCompileShader((GLuint)arg0->Int32Value());
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

    glCompressedTexImage2D((GLenum)arg0->Int32Value(),(GLint)arg1->Int32Value(),(GLenum)arg2->Int32Value(),(GLsizei)arg3->Int32Value(),(GLsizei)arg4->Int32Value(),(GLint)arg5->Int32Value(),(GLsizei)arg6->Int32Value(),ptr0);
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

    glCompressedTexSubImage2D((GLenum)arg0->Int32Value(),(GLint)arg1->Int32Value(),(GLint)arg2->Int32Value(),(GLint)arg3->Int32Value(),(GLsizei)arg4->Int32Value(),(GLsizei)arg5->Int32Value(),(GLenum)arg6->Int32Value(),(GLsizei)arg7->Int32Value(),ptr0);
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

    glCopyTexImage2D((GLenum)arg0->Int32Value(),(GLint)arg1->Int32Value(),(GLenum)arg2->Int32Value(),(GLint)arg3->Int32Value(),(GLint)arg4->Int32Value(),(GLsizei)arg5->Int32Value(),(GLsizei)arg6->Int32Value(),(GLint)arg7->Int32Value());
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

    glCopyTexSubImage2D((GLenum)arg0->Int32Value(),(GLint)arg1->Int32Value(),(GLint)arg2->Int32Value(),(GLint)arg3->Int32Value(),(GLint)arg4->Int32Value(),(GLint)arg5->Int32Value(),(GLsizei)arg6->Int32Value(),(GLsizei)arg7->Int32Value());
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

    GLuint ret=  glCreateShader((GLenum)arg0->Int32Value());
    GL_ERROR_THROW(isolate);
    args.GetReturnValue().Set( v8::Integer::New( isolate, ret ) );
}

void v8Bind_CullFace (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glCullFace((GLenum)arg0->Int32Value());
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

    glDeleteBuffers((GLsizei)arg0->Int32Value(),ptr0);
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

    glDeleteFramebuffers((GLsizei)arg0->Int32Value(),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_DeleteProgram (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glDeleteProgram((GLuint)arg0->Int32Value());
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

    glDeleteRenderbuffers((GLsizei)arg0->Int32Value(),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_DeleteShader (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glDeleteShader((GLuint)arg0->Int32Value());
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

    glDeleteTextures((GLsizei)arg0->Int32Value(),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_DepthFunc (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glDepthFunc((GLenum)arg0->Int32Value());
    GL_ERROR_THROW(isolate);
}

void v8Bind_DepthMask (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsBoolean()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glDepthMask((GLboolean)arg0->BooleanValue());
    GL_ERROR_THROW(isolate);
}

void v8Bind_DepthRangef (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsNumber() || !arg1->IsNumber()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glDepthRangef((GLclampf)arg0->NumberValue(),(GLclampf)arg1->NumberValue());
    GL_ERROR_THROW(isolate);
}

void v8Bind_DetachShader (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glDetachShader((GLuint)arg0->Int32Value(),(GLuint)arg1->Int32Value());
    GL_ERROR_THROW(isolate);
}

void v8Bind_Disable (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glDisable((GLenum)arg0->Int32Value());
    GL_ERROR_THROW(isolate);
}

void v8Bind_DisableVertexAttribArray (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glDisableVertexAttribArray((GLuint)arg0->Int32Value());
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

    glDrawArrays((GLenum)arg0->Int32Value(),(GLint)arg1->Int32Value(),(GLsizei)arg2->Int32Value());
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

    glDrawElements((GLenum)arg0->Int32Value(),(GLsizei)arg1->Int32Value(),(GLenum)arg2->Int32Value(),(GLvoid*)arg3->Int32Value());
    GL_ERROR_THROW(isolate);
}

void v8Bind_Enable (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glEnable((GLenum)arg0->Int32Value());
    GL_ERROR_THROW(isolate);
}

void v8Bind_EnableVertexAttribArray (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glEnableVertexAttribArray((GLuint)arg0->Int32Value());
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

    glFramebufferRenderbuffer((GLenum)arg0->Int32Value(),(GLenum)arg1->Int32Value(),(GLenum)arg2->Int32Value(),(GLuint)arg3->Int32Value());
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

    glFramebufferTexture2D((GLenum)arg0->Int32Value(),(GLenum)arg1->Int32Value(),(GLenum)arg2->Int32Value(),(GLuint)arg3->Int32Value(),(GLint)arg4->Int32Value());
    GL_ERROR_THROW(isolate);
}

void v8Bind_FrontFace (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glFrontFace((GLenum)arg0->Int32Value());
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

    glGenBuffers((GLsizei)arg0->Int32Value(),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_GenerateMipmap (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glGenerateMipmap((GLenum)arg0->Int32Value());
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

    glGenFramebuffers((GLsizei)arg0->Int32Value(),ptr0);
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

    glGenRenderbuffers((GLsizei)arg0->Int32Value(),ptr0);
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

    glGenTextures((GLsizei)arg0->Int32Value(),ptr0);
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
            (GLuint)arg0->Int32Value(),
            (GLuint)arg1->Int32Value(),
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
            String::NewFromUtf8(isolate, "name"),
            String::NewFromUtf8(isolate, name)
    );
    obj->Set(
            String::NewFromUtf8(isolate, "size"),
            Number::New(isolate, retsize)
    );
    obj->Set(
            String::NewFromUtf8(isolate, "type"),
            Number::New(isolate, type)
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
            (GLuint)arg0->Int32Value(),
            (GLuint)arg1->Int32Value(),
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
            String::NewFromUtf8(isolate, "name"),
            String::NewFromUtf8(isolate, name)
    );
    obj->Set(
            String::NewFromUtf8(isolate, "size"),
            Number::New(isolate, retsize)
    );
    obj->Set(
            String::NewFromUtf8(isolate, "type"),
            Number::New(isolate, type)
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

    glGetAttachedShaders((GLuint)arg0->Int32Value(),(GLsizei)arg1->Int32Value(),ptr0,ptr1);
    GL_ERROR_THROW(isolate);
}

void v8Bind_GetAttribLocation (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsString()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLint ret=  glGetAttribLocation((GLuint)arg0->Int32Value(),*String::Utf8Value(arg1));
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

    glGetBooleanv((GLenum)arg0->Int32Value(),ptr0);
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

    glGetBufferParameteriv((GLenum)arg0->Int32Value(),(GLenum)arg1->Int32Value(),ptr0);
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

    glGetFloatv((GLenum)arg0->Int32Value(),ptr0);
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

    glGetFramebufferAttachmentParameteriv((GLenum)arg0->Int32Value(),(GLenum)arg1->Int32Value(),(GLenum)arg2->Int32Value(),ptr0);
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

    GLenum p= (GLenum)arg0->Int32Value();
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

    glGetProgramiv((GLuint)arg0->Int32Value(),(GLenum)arg1->Int32Value(),ptr0);
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

    glGetProgramInfoLog((GLuint)arg0->Int32Value(),len, &retlen, buff);
    GL_ERROR_THROW(isolate);

    args.GetReturnValue().Set( String::NewFromUtf8( isolate, buff ) ) ;
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

    glGetRenderbufferParameteriv((GLenum)arg0->Int32Value(),(GLenum)arg1->Int32Value(),ptr0);
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

    glGetShaderiv((GLuint)arg0->Int32Value(),(GLenum)arg1->Int32Value(),ptr0);
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

    glGetShaderInfoLog((GLuint)arg0->Int32Value(),len, &retlen, buff);
    GL_ERROR_THROW(isolate);

    args.GetReturnValue().Set( String::NewFromUtf8( isolate, buff ) ) ;
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

    glGetShaderPrecisionFormat( (GLenum)arg0->Int32Value(),(GLenum)arg1->Int32Value(),precision,&decimal);
    GL_ERROR_THROW(isolate);

    v8::HandleScope hs(isolate);

    v8::Local<v8::Object> ret = v8::Object::New(isolate);
    ret->Set( String::NewFromUtf8(isolate,"rangeMin"), v8::Int32::New(isolate, precision[0]) );
    ret->Set( String::NewFromUtf8(isolate,"rangeMax"), v8::Int32::New(isolate, precision[1]) );
    ret->Set( String::NewFromUtf8(isolate,"precision"), v8::Int32::New(isolate, decimal) );

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

    glGetShaderSource((GLuint)arg0->Int32Value(),(GLsizei)arg1->Int32Value(),ptr0,*String::Utf8Value(arg3));
    GL_ERROR_THROW(isolate);
}

void v8Bind_GetString (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    const GLubyte* ret=  glGetString((GLenum)arg0->Int32Value());
    GL_ERROR_THROW(isolate);
    args.GetReturnValue().Set( String::NewFromUtf8( isolate, (const char*)ret ) );
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

    glGetTexParameterfv((GLenum)arg0->Int32Value(),(GLenum)arg1->Int32Value(),ptr0);
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

    glGetTexParameteriv((GLenum)arg0->Int32Value(),(GLenum)arg1->Int32Value(),ptr0);
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

    glGetUniformfv((GLuint)arg0->Int32Value(),(GLint)arg1->Int32Value(),ptr0);
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

    glGetUniformiv((GLuint)arg0->Int32Value(),(GLint)arg1->Int32Value(),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_GetUniformLocation (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsString()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLint ret=  glGetUniformLocation((GLuint)arg0->Int32Value(),*String::Utf8Value(arg1));
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

    glGetVertexAttribfv((GLuint)arg0->Int32Value(),(GLenum)arg1->Int32Value(),ptr0);
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

    glGetVertexAttribiv((GLuint)arg0->Int32Value(),(GLenum)arg1->Int32Value(),ptr0);
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

    glGetVertexAttribPointerv((GLuint)arg0->Int32Value(),(GLenum)arg1->Int32Value(),&ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_Hint (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glHint((GLenum)arg0->Int32Value(),(GLenum)arg1->Int32Value());
    GL_ERROR_THROW(isolate);
}

void v8Bind_IsBuffer (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLboolean ret=  glIsBuffer((GLuint)arg0->Int32Value());
    GL_ERROR_THROW(isolate);
    args.GetReturnValue().Set( v8::Boolean::New( isolate, ret ) );
}

void v8Bind_IsEnabled (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLboolean ret=  glIsEnabled((GLenum)arg0->Int32Value());
    GL_ERROR_THROW(isolate);
    args.GetReturnValue().Set( v8::Boolean::New( isolate, ret ) );
}

void v8Bind_IsFramebuffer (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLboolean ret=  glIsFramebuffer((GLuint)arg0->Int32Value());
    GL_ERROR_THROW(isolate);
    args.GetReturnValue().Set( v8::Boolean::New( isolate, ret ) );
}

void v8Bind_IsProgram (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLboolean ret=  glIsProgram((GLuint)arg0->Int32Value());
    GL_ERROR_THROW(isolate);
    args.GetReturnValue().Set( v8::Boolean::New( isolate, ret ) );
}

void v8Bind_IsRenderbuffer (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLboolean ret=  glIsRenderbuffer((GLuint)arg0->Int32Value());
    GL_ERROR_THROW(isolate);
    args.GetReturnValue().Set( v8::Boolean::New( isolate, ret ) );
}

void v8Bind_IsShader (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLboolean ret=  glIsShader((GLuint)arg0->Int32Value());
    GL_ERROR_THROW(isolate);
    args.GetReturnValue().Set( v8::Boolean::New( isolate, ret ) );
}

void v8Bind_IsTexture (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLboolean ret=  glIsTexture((GLuint)arg0->Int32Value());
    GL_ERROR_THROW(isolate);
    args.GetReturnValue().Set( v8::Boolean::New( isolate, ret ) );
}

void v8Bind_LineWidth (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsNumber()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glLineWidth((GLfloat)arg0->NumberValue());
    GL_ERROR_THROW(isolate);
}

void v8Bind_LinkProgram (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glLinkProgram((GLuint)arg0->Int32Value());
    GL_ERROR_THROW(isolate);
}

void v8Bind_PixelStorei (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLenum pname = args[0]->Int32Value();
    GLenum param = args[1]->Int32Value();

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

    glPolygonOffset((GLfloat)arg0->NumberValue(),(GLfloat)arg1->NumberValue());
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

    glReadPixels((GLint)arg0->Int32Value(),(GLint)arg1->Int32Value(),(GLsizei)arg2->Int32Value(),(GLsizei)arg3->Int32Value(),(GLenum)arg4->Int32Value(),(GLenum)arg5->Int32Value(),ptr0);
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

    glRenderbufferStorage((GLenum)arg0->Int32Value(),(GLenum)arg1->Int32Value(),(GLsizei)arg2->Int32Value(),(GLsizei)arg3->Int32Value());
    GL_ERROR_THROW(isolate);
}

void v8Bind_SampleCoverage (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsNumber() || !arg1->IsBoolean()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glSampleCoverage((GLclampf)arg0->NumberValue(),(GLboolean)arg1->BooleanValue());
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

    glScissor((GLint)arg0->Int32Value(),(GLint)arg1->Int32Value(),(GLsizei)arg2->Int32Value(),(GLsizei)arg3->Int32Value());
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

    glShaderBinary((GLsizei)arg0->Int32Value(),ptr0,(GLenum)arg2->Int32Value(),ptr1,(GLsizei)arg4->Int32Value());
    GL_ERROR_THROW(isolate);
}

void v8Bind_ShaderSource (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsString()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    String::Utf8Value srcCode(arg1->ToString());
    std::string code = std::string(*srcCode);

    const char *shader_source = code.c_str();
    glShaderSource((GLuint)arg0->Int32Value(), 1, &shader_source, NULL);
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

    glStencilFunc((GLenum)arg0->Int32Value(),(GLint)arg1->Int32Value(),(GLuint)arg2->Int32Value());
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

    glStencilFuncSeparate((GLenum)arg0->Int32Value(),(GLenum)arg1->Int32Value(),(GLint)arg2->Int32Value(),(GLuint)arg3->Int32Value());
    GL_ERROR_THROW(isolate);
}

void v8Bind_StencilMask (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glStencilMask((GLuint)arg0->Int32Value());
    GL_ERROR_THROW(isolate);
}

void v8Bind_StencilMaskSeparate (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glStencilMaskSeparate((GLenum)arg0->Int32Value(),(GLuint)arg1->Int32Value());
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

    glStencilOp((GLenum)arg0->Int32Value(),(GLenum)arg1->Int32Value(),(GLenum)arg2->Int32Value());
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

    glStencilOpSeparate((GLenum)arg0->Int32Value(),(GLenum)arg1->Int32Value(),(GLenum)arg2->Int32Value(),(GLenum)arg3->Int32Value());
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

    glTexParameterf((GLenum)arg0->Int32Value(),(GLenum)arg1->Int32Value(),(GLfloat)arg2->NumberValue());
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

    glTexParameterfv((GLenum)arg0->Int32Value(),(GLenum)arg1->Int32Value(),ptr0);
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

    glTexParameteri((GLenum)arg0->Int32Value(),(GLenum)arg1->Int32Value(),(GLint)arg2->Int32Value());
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

    glTexParameteriv((GLenum)arg0->Int32Value(),(GLenum)arg1->Int32Value(),ptr0);
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

    glTexSubImage2D((GLenum)arg0->Int32Value(),(GLint)arg1->Int32Value(),(GLint)arg2->Int32Value(),(GLint)arg3->Int32Value(),(GLsizei)arg4->Int32Value(),(GLsizei)arg5->Int32Value(),(GLenum)arg6->Int32Value(),(GLenum)arg7->Int32Value(),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_Uniform1f (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsNumber()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glUniform1f((GLint)arg0->Int32Value(),(GLfloat)arg1->NumberValue());
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

        glUniform1fv((GLint) arg0->Int32Value(), ta->ByteLength(), ptr0);
        GL_ERROR_THROW(isolate);
    } else if ( arg1->IsArray() ) {
        v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast( arg1 );
        glUniform1f(
                (GLint) arg0->Int32Value(),
                (GLfloat)arr->Get(0)->NumberValue()
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

    glUniform1i((GLint)arg0->Int32Value(),(GLint)arg1->Int32Value());
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

        glUniform1iv((GLint) arg0->Int32Value(), ta->ByteLength(), ptr0);
        GL_ERROR_THROW(isolate);
    } else if ( arg1->IsArray() ) {
        v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast( arg1 );
        glUniform1i(
                (GLint) arg0->Int32Value(),
                arr->Get(0)->Int32Value()
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

    glUniform2f((GLint)arg0->Int32Value(),(GLfloat)arg1->NumberValue(),(GLfloat)arg2->NumberValue());
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

        glUniform2fv((GLint) arg0->Int32Value(), ta->ByteLength(), ptr0);
        GL_ERROR_THROW(isolate);
    } else if ( arg1->IsArray() ) {
        v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast( arg1 );
        glUniform2f(
                (GLint) arg0->Int32Value(),
                (GLfloat)arr->Get(0)->NumberValue(),
                (GLfloat)arr->Get(1)->NumberValue()
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

    glUniform2i((GLint)arg0->Int32Value(),(GLint)arg1->Int32Value(),(GLint)arg2->Int32Value());
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

        glUniform2iv((GLint) arg0->Int32Value(), ta->ByteLength(), ptr0);
        GL_ERROR_THROW(isolate);
    } else if ( arg1->IsArray() ) {
        v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast( arg1 );
        glUniform2i(
                (GLint) arg0->Int32Value(),
                arr->Get(0)->Int32Value(),
                arr->Get(1)->Int32Value()
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

    glUniform3f((GLint)arg0->Int32Value(),(GLfloat)arg1->NumberValue(),(GLfloat)arg2->NumberValue(),(GLfloat)arg3->NumberValue());
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

        glUniform3fv((GLint) arg0->Int32Value(), ta->ByteLength(), ptr0);
        GL_ERROR_THROW(isolate);
    } else if ( arg1->IsArray() ) {
        v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast( arg1 );
        glUniform3f(
                (GLint) arg0->Int32Value(),
                (GLfloat)arr->Get(0)->NumberValue(),
                (GLfloat)arr->Get(1)->NumberValue(),
                (GLfloat)arr->Get(2)->NumberValue()
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

    glUniform3i((GLint)arg0->Int32Value(),(GLint)arg1->Int32Value(),(GLint)arg2->Int32Value(),(GLint)arg3->Int32Value());
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

        glUniform3iv((GLint) arg0->Int32Value(), ta->ByteLength(), ptr0);
        GL_ERROR_THROW(isolate);
    } else if ( arg1->IsArray() ) {
        v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast( arg1 );
        glUniform3i(
                (GLint) arg0->Int32Value(),
                arr->Get(0)->Int32Value(),
                arr->Get(1)->Int32Value(),
                arr->Get(2)->Int32Value()
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

    glUniform4f((GLint)arg0->Int32Value(),(GLfloat)arg1->NumberValue(),(GLfloat)arg2->NumberValue(),(GLfloat)arg3->NumberValue(),(GLfloat)arg4->NumberValue());
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

        glUniform4fv((GLint) arg0->Int32Value(), ta->ByteLength(), ptr0);
        GL_ERROR_THROW(isolate);
    } else if ( arg1->IsArray() ) {
        v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast( arg1 );
        glUniform4f(
                (GLint) arg0->Int32Value(),
                (GLfloat)arr->Get(0)->NumberValue(),
                (GLfloat)arr->Get(1)->NumberValue(),
                (GLfloat)arr->Get(2)->NumberValue(),
                (GLfloat)arr->Get(3)->NumberValue()
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

    glUniform4i((GLint)arg0->Int32Value(),(GLint)arg1->Int32Value(),(GLint)arg2->Int32Value(),(GLint)arg3->Int32Value(),(GLint)arg4->Int32Value());
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

        glUniform4iv((GLint) arg0->Int32Value(), ta->ByteLength(), ptr0);
        GL_ERROR_THROW(isolate);
    } else if ( arg1->IsArray() ) {
        v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast( arg1 );
        glUniform4i(
                (GLint) arg0->Int32Value(),
                arr->Get(0)->Int32Value(),
                arr->Get(1)->Int32Value(),
                arr->Get(2)->Int32Value(),
                arr->Get(3)->Int32Value()
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

    glUniformMatrix2fv((GLint)arg0->Int32Value(),1,(GLboolean)arg2->BooleanValue(),ptr0);
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

    glUniformMatrix3fv((GLint)arg0->Int32Value(),1,(GLboolean)arg2->BooleanValue(),ptr0);
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

    glUniformMatrix4fv((GLint)arg0->Int32Value(),1,(GLboolean)arg2->BooleanValue(),ptr0);
    GL_ERROR_THROW(isolate);
}

void v8Bind_UseProgram (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glUseProgram((GLuint)arg0->Int32Value());
    GL_ERROR_THROW(isolate);
}

void v8Bind_ValidateProgram (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glValidateProgram((GLuint)arg0->Int32Value());
    GL_ERROR_THROW(isolate);
}

void v8Bind_VertexAttrib1f (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    if(!arg0->IsInt32() || !arg1->IsNumber()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    glVertexAttrib1f((GLuint)arg0->Int32Value(),(GLfloat)arg1->NumberValue());
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

    glVertexAttrib1fv((GLuint)arg0->Int32Value(),ptr0);
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

    glVertexAttrib2f((GLuint)arg0->Int32Value(),(GLfloat)arg1->NumberValue(),(GLfloat)arg2->NumberValue());
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

    glVertexAttrib2fv((GLuint)arg0->Int32Value(),ptr0);
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

    glVertexAttrib3f((GLuint)arg0->Int32Value(),(GLfloat)arg1->NumberValue(),(GLfloat)arg2->NumberValue(),(GLfloat)arg3->NumberValue());
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

    glVertexAttrib3fv((GLuint)arg0->Int32Value(),ptr0);
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

    glVertexAttrib4f((GLuint)arg0->Int32Value(),(GLfloat)arg1->NumberValue(),(GLfloat)arg2->NumberValue(),(GLfloat)arg3->NumberValue(),(GLfloat)arg4->NumberValue());
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

    glVertexAttrib4fv((GLuint)arg0->Int32Value(),ptr0);
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

    glVertexAttribPointer((GLuint)arg0->Int32Value(),(GLint)arg1->Int32Value(),(GLenum)arg2->Int32Value(),(GLboolean)arg3->BooleanValue(),(GLsizei)arg4->Int32Value(),(GLvoid*)arg5->Int32Value());
    GL_ERROR_THROW(isolate);
}

void v8Bind_Viewport (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];
    v8::Local<v8::Value> arg3= args[3];

    glViewport((GLint)arg0->Int32Value(),(GLint)arg1->Int32Value(),(GLsizei)arg2->Int32Value(),(GLsizei)arg3->Int32Value());
    GL_ERROR_THROW(isolate);
}

void v8Bind_GetExtension (const v8::FunctionCallbackInfo<v8::Value>& args) {
    args.GetReturnValue().SetNull();
}

void v8Bind_GetParameter (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();

    v8::HandleScope hs(isolate);

    v8::Local<v8::Value> arg0= args[0];

    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }

    GLenum param = (GLenum)arg0->Int32Value();

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
            args.GetReturnValue().Set( String::NewFromUtf8(isolate, "WebGL 2.0 (Hyper+ActiveTheory)") );
        }

        case GL_RENDERER:
        case GL_SHADING_LANGUAGE_VERSION:
        case GL_VENDOR:
        case GL_EXTENSIONS:
        {
            // return a string
            char *params = (char *)glGetString(param);

            if (params != NULL) {
                args.GetReturnValue().Set( String::NewFromUtf8(isolate, params) );
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
            isolate->ThrowException( v8::Exception::Error( String::NewFromUtf8(isolate, "Unknown getParam value")) );
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

    glTexImage2D((GLenum) arg0->Int32Value(), (GLint) arg1->Int32Value(), (GLint) arg2->Int32Value(),
                 (GLsizei) arg3->Int32Value(), (GLsizei) arg4->Int32Value(),
                 (GLint) arg5->Int32Value(), (GLenum) arg6->Int32Value(), (GLenum) arg7->Int32Value(), NULL);

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

    int p2 = arg2->Int32Value();
    int p7 = arg7->Int32Value();

    const GLvoid* ptr0= ( arg8->IsTypedArray() ) ?
                        (const GLvoid*)v8::Local<v8::TypedArray>::Cast(arg8)->Buffer()->GetContents().Data() :
                        nullptr;

    glTexImage2D((GLenum)arg0->Int32Value(),(GLint)arg1->Int32Value(),(GLint)p2,(GLsizei)arg3->Int32Value(),(GLsizei)arg4->Int32Value(),(GLint)arg5->Int32Value(),(GLenum)arg6->Int32Value(),(GLenum)p7,ptr0);
    GL_ERROR_THROW(isolate);
}

void AURA_GetExtensions (const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    Isolate::Scope iscope( isolate );
    HandleScope hs( isolate );
    v8::Local<v8::Value> arg0= args[0];

    char *extensionString = (char*) glGetString(GL_EXTENSIONS);

    args.GetReturnValue().Set(String::NewFromUtf8(isolate, extensionString));
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

    void * vv = reinterpret_cast<void*>(arg3->Int32Value());
    glDrawElementsInstanced((GLenum)arg0->Int32Value(), (GLsizei)arg1->Int32Value(), (GLenum)arg2->Int32Value(), vv, (GLsizei)arg4->Int32Value());
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

    glDrawArraysInstanced((GLenum)arg0->Int32Value(), (GLint)arg1->Int32Value(), (GLsizei)arg2->Int32Value(), (GLsizei)arg3->Int32Value());
    GL_ERROR_THROW(isolate);
}

void AURA_VertexAttribDivisor (const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    Isolate::Scope iscope( isolate );
    HandleScope hs( isolate );
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];

    glVertexAttribDivisor((GLuint)arg0->Int32Value(), (GLuint)arg1->Int32Value());
    GL_ERROR_THROW(isolate);
}

void AURA_GetUniformBlockIndex (const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    Isolate::Scope iscope( isolate );
    HandleScope hs( isolate );
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];

    v8::Local<v8::Value> stringValue = arg1->ToString();
    String::Utf8Value str1(stringValue);
    std::string src = *str1;

    GLuint id = glGetUniformBlockIndex((GLuint)arg0->Int32Value(), src.c_str());
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

    glUniformBlockBinding((GLuint)arg0->Int32Value(), (GLuint)arg1->Int32Value(), (GLuint)arg2->Int32Value());
    GL_ERROR_THROW(isolate);
}

void AURA_BindBufferBase (const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    Isolate::Scope iscope( isolate );
    HandleScope hs( isolate );
    v8::Local<v8::Value> arg0= args[0];
    v8::Local<v8::Value> arg1= args[1];
    v8::Local<v8::Value> arg2= args[2];

    glBindBufferBase((GLenum)arg0->Int32Value(), (GLuint)arg1->Int32Value(), (GLuint)arg2->Int32Value());
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

    glBindVertexArray((GLuint)arg0->Int32Value());
    GL_ERROR_THROW(isolate);
}

void AURA_DeleteVertexArray (const v8::FunctionCallbackInfo<v8::Value>& args) {
    Isolate* isolate = args.GetIsolate();
    Isolate::Scope iscope( isolate );
    HandleScope hs( isolate );
    v8::Local<v8::Value> arg0= args[0];

    GLuint params = (GLuint)arg0->Int32Value();
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
        buffers[i] = buffersArray->Get(i)->Uint32Value();
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

    GLuint x0 = (GLenum)arg0->Int32Value();
    GLuint y0 = (GLenum)arg1->Int32Value();
    GLuint w0 = (GLenum)arg2->Int32Value();
    GLuint h0 = (GLenum)arg3->Int32Value();
    GLuint x1 = (GLenum)arg4->Int32Value();
    GLuint y1 = (GLenum)arg5->Int32Value();
    GLuint w1 = (GLenum)arg6->Int32Value();
    GLuint h1 = (GLenum)arg7->Int32Value();
    GLbitfield target = (GLenum)arg8->Int32Value();
    GLenum type = (GLenum)arg9->Int32Value();

    glBlitFramebuffer(x0, y0, w0, h0, x1, y1, w1, h1, target, type);
    GL_ERROR_THROW(isolate);
}

void initializeAura(V8Runtime* runtime, Local<Object> gl) {
    v8::Isolate* isolate = runtime->isolate;
    isolate_ = isolate;

    gl->Set( String::NewFromUtf8(isolate, "DEPTH_BUFFER_BIT"), Integer::New(isolate, 0x00000100) );
    gl->Set( String::NewFromUtf8(isolate, "STENCIL_BUFFER_BIT"), Integer::New(isolate, 0x00000400) );
    gl->Set( String::NewFromUtf8(isolate, "COLOR_BUFFER_BIT"), Integer::New(isolate, 0x00004000) );
    gl->Set( String::NewFromUtf8(isolate, "FALSE"), Integer::New(isolate, 0) );
    gl->Set( String::NewFromUtf8(isolate, "TRUE"), Integer::New(isolate, 1) );
    gl->Set( String::NewFromUtf8(isolate, "POINTS"), Integer::New(isolate, 0x0000) );
    gl->Set( String::NewFromUtf8(isolate, "LINES"), Integer::New(isolate, 0x0001) );
    gl->Set( String::NewFromUtf8(isolate, "LINE_LOOP"), Integer::New(isolate, 0x0002) );
    gl->Set( String::NewFromUtf8(isolate, "LINE_STRIP"), Integer::New(isolate, 0x0003) );
    gl->Set( String::NewFromUtf8(isolate, "TRIANGLES"), Integer::New(isolate, 0x0004) );
    gl->Set( String::NewFromUtf8(isolate, "TRIANGLE_STRIP"), Integer::New(isolate, 0x0005) );
    gl->Set( String::NewFromUtf8(isolate, "TRIANGLE_FAN"), Integer::New(isolate, 0x0006) );
    gl->Set( String::NewFromUtf8(isolate, "ZERO"), Integer::New(isolate, 0) );
    gl->Set( String::NewFromUtf8(isolate, "ONE"), Integer::New(isolate, 1) );
    gl->Set( String::NewFromUtf8(isolate, "SRC_COLOR"), Integer::New(isolate, 0x0300) );
    gl->Set( String::NewFromUtf8(isolate, "ONE_MINUS_SRC_COLOR"), Integer::New(isolate, 0x0301) );
    gl->Set( String::NewFromUtf8(isolate, "SRC_ALPHA"), Integer::New(isolate, 0x0302) );
    gl->Set( String::NewFromUtf8(isolate, "ONE_MINUS_SRC_ALPHA"), Integer::New(isolate, 0x0303) );
    gl->Set( String::NewFromUtf8(isolate, "DST_ALPHA"), Integer::New(isolate, 0x0304) );
    gl->Set( String::NewFromUtf8(isolate, "ONE_MINUS_DST_ALPHA"), Integer::New(isolate, 0x0305) );
    gl->Set( String::NewFromUtf8(isolate, "DST_COLOR"), Integer::New(isolate, 0x0306) );
    gl->Set( String::NewFromUtf8(isolate, "ONE_MINUS_DST_COLOR"), Integer::New(isolate, 0x0307) );
    gl->Set( String::NewFromUtf8(isolate, "SRC_ALPHA_SATURATE"), Integer::New(isolate, 0x0308) );
    gl->Set( String::NewFromUtf8(isolate, "FUNC_ADD"), Integer::New(isolate, 0x8006) );
    gl->Set( String::NewFromUtf8(isolate, "BLEND_EQUATION"), Integer::New(isolate, 0x8009) );
    gl->Set( String::NewFromUtf8(isolate, "BLEND_EQUATION_RGB"), Integer::New(isolate, 0x8009) );
    gl->Set( String::NewFromUtf8(isolate, "BLEND_EQUATION_ALPHA"), Integer::New(isolate, 0x883D) );
    gl->Set( String::NewFromUtf8(isolate, "FUNC_SUBTRACT"), Integer::New(isolate, 0x800A) );
    gl->Set( String::NewFromUtf8(isolate, "FUNC_REVERSE_SUBTRACT"), Integer::New(isolate, 0x800B) );
    gl->Set( String::NewFromUtf8(isolate, "BLEND_DST_RGB"), Integer::New(isolate, 0x80C8) );
    gl->Set( String::NewFromUtf8(isolate, "BLEND_SRC_RGB"), Integer::New(isolate, 0x80C9) );
    gl->Set( String::NewFromUtf8(isolate, "BLEND_DST_ALPHA"), Integer::New(isolate, 0x80CA) );
    gl->Set( String::NewFromUtf8(isolate, "BLEND_SRC_ALPHA"), Integer::New(isolate, 0x80CB) );
    gl->Set( String::NewFromUtf8(isolate, "CONSTANT_COLOR"), Integer::New(isolate, 0x8001) );
    gl->Set( String::NewFromUtf8(isolate, "ONE_MINUS_CONSTANT_COLOR"), Integer::New(isolate, 0x8002) );
    gl->Set( String::NewFromUtf8(isolate, "CONSTANT_ALPHA"), Integer::New(isolate, 0x8003) );
    gl->Set( String::NewFromUtf8(isolate, "ONE_MINUS_CONSTANT_ALPHA"), Integer::New(isolate, 0x8004) );
    gl->Set( String::NewFromUtf8(isolate, "BLEND_COLOR"), Integer::New(isolate, 0x8005) );
    gl->Set( String::NewFromUtf8(isolate, "ARRAY_BUFFER"), Integer::New(isolate, 0x8892) );
    gl->Set( String::NewFromUtf8(isolate, "ELEMENT_ARRAY_BUFFER"), Integer::New(isolate, 0x8893) );
    gl->Set( String::NewFromUtf8(isolate, "ARRAY_BUFFER_BINDING"), Integer::New(isolate, 0x8894) );
    gl->Set( String::NewFromUtf8(isolate, "ELEMENT_ARRAY_BUFFER_BINDING"), Integer::New(isolate, 0x8895) );
    gl->Set( String::NewFromUtf8(isolate, "STREAM_DRAW"), Integer::New(isolate, 0x88E0) );
    gl->Set( String::NewFromUtf8(isolate, "STATIC_DRAW"), Integer::New(isolate, 0x88E4) );
    gl->Set( String::NewFromUtf8(isolate, "DYNAMIC_DRAW"), Integer::New(isolate, 0x88E8) );
    gl->Set( String::NewFromUtf8(isolate, "BUFFER_SIZE"), Integer::New(isolate, 0x8764) );
    gl->Set( String::NewFromUtf8(isolate, "BUFFER_USAGE"), Integer::New(isolate, 0x8765) );
    gl->Set( String::NewFromUtf8(isolate, "CURRENT_VERTEX_ATTRIB"), Integer::New(isolate, 0x8626) );
    gl->Set( String::NewFromUtf8(isolate, "FRONT"), Integer::New(isolate, 0x0404) );
    gl->Set( String::NewFromUtf8(isolate, "BACK"), Integer::New(isolate, 0x0405) );
    gl->Set( String::NewFromUtf8(isolate, "FRONT_AND_BACK"), Integer::New(isolate, 0x0408) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE_2D"), Integer::New(isolate, 0x0DE1) );
    gl->Set( String::NewFromUtf8(isolate, "CULL_FACE"), Integer::New(isolate, 0x0B44) );
    gl->Set( String::NewFromUtf8(isolate, "BLEND"), Integer::New(isolate, 0x0BE2) );
    gl->Set( String::NewFromUtf8(isolate, "DITHER"), Integer::New(isolate, 0x0BD0) );
    gl->Set( String::NewFromUtf8(isolate, "STENCIL_TEST"), Integer::New(isolate, 0x0B90) );
    gl->Set( String::NewFromUtf8(isolate, "DEPTH_TEST"), Integer::New(isolate, 0x0B71) );
    gl->Set( String::NewFromUtf8(isolate, "SCISSOR_TEST"), Integer::New(isolate, 0x0C11) );
    gl->Set( String::NewFromUtf8(isolate, "POLYGON_OFFSET_FILL"), Integer::New(isolate, 0x8037) );
    gl->Set( String::NewFromUtf8(isolate, "SAMPLE_ALPHA_TO_COVERAGE"), Integer::New(isolate, 0x809E) );
    gl->Set( String::NewFromUtf8(isolate, "SAMPLE_COVERAGE"), Integer::New(isolate, 0x80A0) );
    gl->Set( String::NewFromUtf8(isolate, "NO_ERROR"), Integer::New(isolate, 0) );
    gl->Set( String::NewFromUtf8(isolate, "INVALID_ENUM"), Integer::New(isolate, 0x0500) );
    gl->Set( String::NewFromUtf8(isolate, "INVALID_VALUE"), Integer::New(isolate, 0x0501) );
    gl->Set( String::NewFromUtf8(isolate, "INVALID_OPERATION"), Integer::New(isolate, 0x0502) );
    gl->Set( String::NewFromUtf8(isolate, "OUT_OF_MEMORY"), Integer::New(isolate, 0x0505) );
    gl->Set( String::NewFromUtf8(isolate, "CW"), Integer::New(isolate, 0x0900) );
    gl->Set( String::NewFromUtf8(isolate, "CCW"), Integer::New(isolate, 0x0901) );
    gl->Set( String::NewFromUtf8(isolate, "LINE_WIDTH"), Integer::New(isolate, 0x0B21) );
    gl->Set( String::NewFromUtf8(isolate, "ALIASED_POINT_SIZE_RANGE"), Integer::New(isolate, 0x846D) );
    gl->Set( String::NewFromUtf8(isolate, "ALIASED_LINE_WIDTH_RANGE"), Integer::New(isolate, 0x846E) );
    gl->Set( String::NewFromUtf8(isolate, "CULL_FACE_MODE"), Integer::New(isolate, 0x0B45) );
    gl->Set( String::NewFromUtf8(isolate, "FRONT_FACE"), Integer::New(isolate, 0x0B46) );
    gl->Set( String::NewFromUtf8(isolate, "DEPTH_RANGE"), Integer::New(isolate, 0x0B70) );
    gl->Set( String::NewFromUtf8(isolate, "DEPTH_WRITEMASK"), Integer::New(isolate, 0x0B72) );
    gl->Set( String::NewFromUtf8(isolate, "DEPTH_CLEAR_VALUE"), Integer::New(isolate, 0x0B73) );
    gl->Set( String::NewFromUtf8(isolate, "DEPTH_FUNC"), Integer::New(isolate, 0x0B74) );
    gl->Set( String::NewFromUtf8(isolate, "STENCIL_CLEAR_VALUE"), Integer::New(isolate, 0x0B91) );
    gl->Set( String::NewFromUtf8(isolate, "STENCIL_FUNC"), Integer::New(isolate, 0x0B92) );
    gl->Set( String::NewFromUtf8(isolate, "STENCIL_FAIL"), Integer::New(isolate, 0x0B94) );
    gl->Set( String::NewFromUtf8(isolate, "STENCIL_PASS_DEPTH_FAIL"), Integer::New(isolate, 0x0B95) );
    gl->Set( String::NewFromUtf8(isolate, "STENCIL_PASS_DEPTH_PASS"), Integer::New(isolate, 0x0B96) );
    gl->Set( String::NewFromUtf8(isolate, "STENCIL_REF"), Integer::New(isolate, 0x0B97) );
    gl->Set( String::NewFromUtf8(isolate, "STENCIL_VALUE_MASK"), Integer::New(isolate, 0x0B93) );
    gl->Set( String::NewFromUtf8(isolate, "STENCIL_WRITEMASK"), Integer::New(isolate, 0x0B98) );
    gl->Set( String::NewFromUtf8(isolate, "STENCIL_BACK_FUNC"), Integer::New(isolate, 0x8800) );
    gl->Set( String::NewFromUtf8(isolate, "STENCIL_BACK_FAIL"), Integer::New(isolate, 0x8801) );
    gl->Set( String::NewFromUtf8(isolate, "STENCIL_BACK_PASS_DEPTH_FAIL"), Integer::New(isolate, 0x8802) );
    gl->Set( String::NewFromUtf8(isolate, "STENCIL_BACK_PASS_DEPTH_PASS"), Integer::New(isolate, 0x8803) );
    gl->Set( String::NewFromUtf8(isolate, "STENCIL_BACK_REF"), Integer::New(isolate, 0x8CA3) );
    gl->Set( String::NewFromUtf8(isolate, "STENCIL_BACK_VALUE_MASK"), Integer::New(isolate, 0x8CA4) );
    gl->Set( String::NewFromUtf8(isolate, "STENCIL_BACK_WRITEMASK"), Integer::New(isolate, 0x8CA5) );
    gl->Set( String::NewFromUtf8(isolate, "VIEWPORT"), Integer::New(isolate, 0x0BA2) );
    gl->Set( String::NewFromUtf8(isolate, "SCISSOR_BOX"), Integer::New(isolate, 0x0C10) );
    gl->Set( String::NewFromUtf8(isolate, "COLOR_CLEAR_VALUE"), Integer::New(isolate, 0x0C22) );
    gl->Set( String::NewFromUtf8(isolate, "COLOR_WRITEMASK"), Integer::New(isolate, 0x0C23) );
    gl->Set( String::NewFromUtf8(isolate, "UNPACK_ALIGNMENT"), Integer::New(isolate, GL_UNPACK_ALIGNMENT) );
    gl->Set( String::NewFromUtf8(isolate, "UNPACK_FLIP_Y_WEBGL"), Integer::New(isolate, 0x9240) );
    gl->Set( String::NewFromUtf8(isolate, "UNPACK_PREMULTIPLY_ALPHA_WEBGL"), Integer::New(isolate, 0x9241) );
    gl->Set( String::NewFromUtf8(isolate, "PACK_ALIGNMENT"), Integer::New(isolate, 0x0D05) );
    gl->Set( String::NewFromUtf8(isolate, "MAX_TEXTURE_SIZE"), Integer::New(isolate, 0x0D33) );
    gl->Set( String::NewFromUtf8(isolate, "MAX_VIEWPORT_DIMS"), Integer::New(isolate, 0x0D3A) );
    gl->Set( String::NewFromUtf8(isolate, "SUBPIXEL_BITS"), Integer::New(isolate, 0x0D50) );
    gl->Set( String::NewFromUtf8(isolate, "RED_BITS"), Integer::New(isolate, 0x0D52) );
    gl->Set( String::NewFromUtf8(isolate, "GREEN_BITS"), Integer::New(isolate, 0x0D53) );
    gl->Set( String::NewFromUtf8(isolate, "BLUE_BITS"), Integer::New(isolate, 0x0D54) );
    gl->Set( String::NewFromUtf8(isolate, "ALPHA_BITS"), Integer::New(isolate, 0x0D55) );
    gl->Set( String::NewFromUtf8(isolate, "DEPTH_BITS"), Integer::New(isolate, 0x0D56) );
    gl->Set( String::NewFromUtf8(isolate, "STENCIL_BITS"), Integer::New(isolate, 0x0D57) );
    gl->Set( String::NewFromUtf8(isolate, "POLYGON_OFFSET_UNITS"), Integer::New(isolate, 0x2A00) );
    gl->Set( String::NewFromUtf8(isolate, "POLYGON_OFFSET_FACTOR"), Integer::New(isolate, 0x8038) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE_BINDING_2D"), Integer::New(isolate, 0x8069) );
    gl->Set( String::NewFromUtf8(isolate, "SAMPLE_BUFFERS"), Integer::New(isolate, 0x80A8) );
    gl->Set( String::NewFromUtf8(isolate, "SAMPLES"), Integer::New(isolate, 0x80A9) );
    gl->Set( String::NewFromUtf8(isolate, "SAMPLE_COVERAGE_VALUE"), Integer::New(isolate, 0x80AA) );
    gl->Set( String::NewFromUtf8(isolate, "SAMPLE_COVERAGE_INVERT"), Integer::New(isolate, 0x80AB) );
    gl->Set( String::NewFromUtf8(isolate, "NUM_COMPRESSED_TEXTURE_FORMATS"), Integer::New(isolate, 0x86A2) );
    gl->Set( String::NewFromUtf8(isolate, "COMPRESSED_TEXTURE_FORMATS"), Integer::New(isolate, 0x86A3) );
    gl->Set( String::NewFromUtf8(isolate, "DONT_CARE"), Integer::New(isolate, 0x1100) );
    gl->Set( String::NewFromUtf8(isolate, "FASTEST"), Integer::New(isolate, 0x1101) );
    gl->Set( String::NewFromUtf8(isolate, "NICEST"), Integer::New(isolate, 0x1102) );
    gl->Set( String::NewFromUtf8(isolate, "GENERATE_MIPMAP_HINT"), Integer::New(isolate, 0x8192) );
    gl->Set( String::NewFromUtf8(isolate, "BYTE"), Integer::New(isolate, 0x1400) );
    gl->Set( String::NewFromUtf8(isolate, "UNSIGNED_BYTE"), Integer::New(isolate, 0x1401) );
    gl->Set( String::NewFromUtf8(isolate, "SHORT"), Integer::New(isolate, 0x1402) );
    gl->Set( String::NewFromUtf8(isolate, "UNSIGNED_SHORT"), Integer::New(isolate, 0x1403) );
    gl->Set( String::NewFromUtf8(isolate, "INT"), Integer::New(isolate, 0x1404) );
    gl->Set( String::NewFromUtf8(isolate, "UNSIGNED_INT"), Integer::New(isolate, 0x1405) );
    gl->Set( String::NewFromUtf8(isolate, "FLOAT"), Integer::New(isolate, 0x1406) );
    gl->Set( String::NewFromUtf8(isolate, "FIXED"), Integer::New(isolate, 0x140C) );
    gl->Set( String::NewFromUtf8(isolate, "DEPTH_COMPONENT"), Integer::New(isolate, 0x1902) );
    gl->Set( String::NewFromUtf8(isolate, "ALPHA"), Integer::New(isolate, 0x1906) );
    gl->Set( String::NewFromUtf8(isolate, "RGB"), Integer::New(isolate, 0x1907) );
    gl->Set( String::NewFromUtf8(isolate, "RGBA"), Integer::New(isolate, 0x1908) );
    gl->Set( String::NewFromUtf8(isolate, "LUMINANCE"), Integer::New(isolate, 0x1909) );
    gl->Set( String::NewFromUtf8(isolate, "LUMINANCE_ALPHA"), Integer::New(isolate, 0x190A) );
    gl->Set( String::NewFromUtf8(isolate, "UNSIGNED_SHORT_4_4_4_4"), Integer::New(isolate, 0x8033) );
    gl->Set( String::NewFromUtf8(isolate, "UNSIGNED_SHORT_5_5_5_1"), Integer::New(isolate, 0x8034) );
    gl->Set( String::NewFromUtf8(isolate, "UNSIGNED_SHORT_5_6_5"), Integer::New(isolate, 0x8363) );
    gl->Set( String::NewFromUtf8(isolate, "FRAGMENT_SHADER"), Integer::New(isolate, 0x8B30) );
    gl->Set( String::NewFromUtf8(isolate, "VERTEX_SHADER"), Integer::New(isolate, 0x8B31) );
    gl->Set( String::NewFromUtf8(isolate, "MAX_VERTEX_ATTRIBS"), Integer::New(isolate, 0x8869) );
    gl->Set( String::NewFromUtf8(isolate, "MAX_VERTEX_UNIFORM_VECTORS"), Integer::New(isolate, 0x8DFB) );
    gl->Set( String::NewFromUtf8(isolate, "MAX_VARYING_VECTORS"), Integer::New(isolate, 0x8DFC) );
    gl->Set( String::NewFromUtf8(isolate, "MAX_COMBINED_TEXTURE_IMAGE_UNITS"), Integer::New(isolate, 0x8B4D) );
    gl->Set( String::NewFromUtf8(isolate, "MAX_VERTEX_TEXTURE_IMAGE_UNITS"), Integer::New(isolate, 0x8B4C) );
    gl->Set( String::NewFromUtf8(isolate, "MAX_TEXTURE_IMAGE_UNITS"), Integer::New(isolate, 0x8872) );
    gl->Set( String::NewFromUtf8(isolate, "MAX_FRAGMENT_UNIFORM_VECTORS"), Integer::New(isolate, 0x8DFD) );
    gl->Set( String::NewFromUtf8(isolate, "SHADER_TYPE"), Integer::New(isolate, 0x8B4F) );
    gl->Set( String::NewFromUtf8(isolate, "DELETE_STATUS"), Integer::New(isolate, 0x8B80) );
    gl->Set( String::NewFromUtf8(isolate, "LINK_STATUS"), Integer::New(isolate, 0x8B82) );
    gl->Set( String::NewFromUtf8(isolate, "VALIDATE_STATUS"), Integer::New(isolate, 0x8B83) );
    gl->Set( String::NewFromUtf8(isolate, "ATTACHED_SHADERS"), Integer::New(isolate, 0x8B85) );
    gl->Set( String::NewFromUtf8(isolate, "ACTIVE_UNIFORMS"), Integer::New(isolate, 0x8B86) );
    gl->Set( String::NewFromUtf8(isolate, "ACTIVE_UNIFORM_MAX_LENGTH"), Integer::New(isolate, 0x8B87) );
    gl->Set( String::NewFromUtf8(isolate, "ACTIVE_ATTRIBUTES"), Integer::New(isolate, 0x8B89) );
    gl->Set( String::NewFromUtf8(isolate, "ACTIVE_ATTRIBUTE_MAX_LENGTH"), Integer::New(isolate, 0x8B8A) );
    gl->Set( String::NewFromUtf8(isolate, "SHADING_LANGUAGE_VERSION"), Integer::New(isolate, 0x8B8C) );
    gl->Set( String::NewFromUtf8(isolate, "CURRENT_PROGRAM"), Integer::New(isolate, 0x8B8D) );
    gl->Set( String::NewFromUtf8(isolate, "NEVER"), Integer::New(isolate, 0x0200) );
    gl->Set( String::NewFromUtf8(isolate, "LESS"), Integer::New(isolate, 0x0201) );
    gl->Set( String::NewFromUtf8(isolate, "EQUAL"), Integer::New(isolate, 0x0202) );
    gl->Set( String::NewFromUtf8(isolate, "LEQUAL"), Integer::New(isolate, 0x0203) );
    gl->Set( String::NewFromUtf8(isolate, "GREATER"), Integer::New(isolate, 0x0204) );
    gl->Set( String::NewFromUtf8(isolate, "NOTEQUAL"), Integer::New(isolate, 0x0205) );
    gl->Set( String::NewFromUtf8(isolate, "GEQUAL"), Integer::New(isolate, 0x0206) );
    gl->Set( String::NewFromUtf8(isolate, "ALWAYS"), Integer::New(isolate, 0x0207) );
    gl->Set( String::NewFromUtf8(isolate, "KEEP"), Integer::New(isolate, 0x1E00) );
    gl->Set( String::NewFromUtf8(isolate, "REPLACE"), Integer::New(isolate, 0x1E01) );
    gl->Set( String::NewFromUtf8(isolate, "INCR"), Integer::New(isolate, 0x1E02) );
    gl->Set( String::NewFromUtf8(isolate, "DECR"), Integer::New(isolate, 0x1E03) );
    gl->Set( String::NewFromUtf8(isolate, "INVERT"), Integer::New(isolate, 0x150A) );
    gl->Set( String::NewFromUtf8(isolate, "INCR_WRAP"), Integer::New(isolate, 0x8507) );
    gl->Set( String::NewFromUtf8(isolate, "DECR_WRAP"), Integer::New(isolate, 0x8508) );
    gl->Set( String::NewFromUtf8(isolate, "VENDOR"), Integer::New(isolate, 0x1F00) );
    gl->Set( String::NewFromUtf8(isolate, "RENDERER"), Integer::New(isolate, 0x1F01) );
    gl->Set( String::NewFromUtf8(isolate, "VERSION"), Integer::New(isolate, 0x1F02) );
    gl->Set( String::NewFromUtf8(isolate, "EXTENSIONS"), Integer::New(isolate, 0x1F03) );
    gl->Set( String::NewFromUtf8(isolate, "NEAREST"), Integer::New(isolate, 0x2600) );
    gl->Set( String::NewFromUtf8(isolate, "LINEAR"), Integer::New(isolate, 0x2601) );
    gl->Set( String::NewFromUtf8(isolate, "NEAREST_MIPMAP_NEAREST"), Integer::New(isolate, 0x2700) );
    gl->Set( String::NewFromUtf8(isolate, "LINEAR_MIPMAP_NEAREST"), Integer::New(isolate, 0x2701) );
    gl->Set( String::NewFromUtf8(isolate, "NEAREST_MIPMAP_LINEAR"), Integer::New(isolate, 0x2702) );
    gl->Set( String::NewFromUtf8(isolate, "LINEAR_MIPMAP_LINEAR"), Integer::New(isolate, 0x2703) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE_MAG_FILTER"), Integer::New(isolate, 0x2800) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE_MIN_FILTER"), Integer::New(isolate, 0x2801) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE_WRAP_S"), Integer::New(isolate, 0x2802) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE_WRAP_T"), Integer::New(isolate, 0x2803) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE"), Integer::New(isolate, 0x1702) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE_CUBE_MAP"), Integer::New(isolate, 0x8513) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE_BINDING_CUBE_MAP"), Integer::New(isolate, 0x8514) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE_CUBE_MAP_POSITIVE_X"), Integer::New(isolate, 0x8515) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE_CUBE_MAP_NEGATIVE_X"), Integer::New(isolate, 0x8516) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE_CUBE_MAP_POSITIVE_Y"), Integer::New(isolate, 0x8517) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE_CUBE_MAP_NEGATIVE_Y"), Integer::New(isolate, 0x8518) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE_CUBE_MAP_POSITIVE_Z"), Integer::New(isolate, 0x8519) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE_CUBE_MAP_NEGATIVE_Z"), Integer::New(isolate, 0x851A) );
    gl->Set( String::NewFromUtf8(isolate, "MAX_CUBE_MAP_TEXTURE_SIZE"), Integer::New(isolate, 0x851C) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE0"), Integer::New(isolate, 0x84C0) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE1"), Integer::New(isolate, 0x84C1) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE2"), Integer::New(isolate, 0x84C2) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE3"), Integer::New(isolate, 0x84C3) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE4"), Integer::New(isolate, 0x84C4) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE5"), Integer::New(isolate, 0x84C5) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE6"), Integer::New(isolate, 0x84C6) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE7"), Integer::New(isolate, 0x84C7) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE8"), Integer::New(isolate, 0x84C8) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE9"), Integer::New(isolate, 0x84C9) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE10"), Integer::New(isolate, 0x84CA) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE11"), Integer::New(isolate, 0x84CB) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE12"), Integer::New(isolate, 0x84CC) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE13"), Integer::New(isolate, 0x84CD) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE14"), Integer::New(isolate, 0x84CE) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE15"), Integer::New(isolate, 0x84CF) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE16"), Integer::New(isolate, 0x84D0) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE17"), Integer::New(isolate, 0x84D1) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE18"), Integer::New(isolate, 0x84D2) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE19"), Integer::New(isolate, 0x84D3) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE20"), Integer::New(isolate, 0x84D4) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE21"), Integer::New(isolate, 0x84D5) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE22"), Integer::New(isolate, 0x84D6) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE23"), Integer::New(isolate, 0x84D7) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE24"), Integer::New(isolate, 0x84D8) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE25"), Integer::New(isolate, 0x84D9) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE26"), Integer::New(isolate, 0x84DA) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE27"), Integer::New(isolate, 0x84DB) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE28"), Integer::New(isolate, 0x84DC) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE29"), Integer::New(isolate, 0x84DD) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE30"), Integer::New(isolate, 0x84DE) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE31"), Integer::New(isolate, 0x84DF) );
    gl->Set( String::NewFromUtf8(isolate, "ACTIVE_TEXTURE"), Integer::New(isolate, 0x84E0) );
    gl->Set( String::NewFromUtf8(isolate, "REPEAT"), Integer::New(isolate, 0x2901) );
    gl->Set( String::NewFromUtf8(isolate, "CLAMP_TO_EDGE"), Integer::New(isolate, 0x812F) );
    gl->Set( String::NewFromUtf8(isolate, "MIRRORED_REPEAT"), Integer::New(isolate, 0x8370) );
    gl->Set( String::NewFromUtf8(isolate, "FLOAT_VEC2"), Integer::New(isolate, 0x8B50) );
    gl->Set( String::NewFromUtf8(isolate, "FLOAT_VEC3"), Integer::New(isolate, 0x8B51) );
    gl->Set( String::NewFromUtf8(isolate, "FLOAT_VEC4"), Integer::New(isolate, 0x8B52) );
    gl->Set( String::NewFromUtf8(isolate, "INT_VEC2"), Integer::New(isolate, 0x8B53) );
    gl->Set( String::NewFromUtf8(isolate, "INT_VEC3"), Integer::New(isolate, 0x8B54) );
    gl->Set( String::NewFromUtf8(isolate, "INT_VEC4"), Integer::New(isolate, 0x8B55) );
    gl->Set( String::NewFromUtf8(isolate, "BOOL"), Integer::New(isolate, 0x8B56) );
    gl->Set( String::NewFromUtf8(isolate, "BOOL_VEC2"), Integer::New(isolate, 0x8B57) );
    gl->Set( String::NewFromUtf8(isolate, "BOOL_VEC3"), Integer::New(isolate, 0x8B58) );
    gl->Set( String::NewFromUtf8(isolate, "BOOL_VEC4"), Integer::New(isolate, 0x8B59) );
    gl->Set( String::NewFromUtf8(isolate, "FLOAT_MAT2"), Integer::New(isolate, 0x8B5A) );
    gl->Set( String::NewFromUtf8(isolate, "FLOAT_MAT3"), Integer::New(isolate, 0x8B5B) );
    gl->Set( String::NewFromUtf8(isolate, "FLOAT_MAT4"), Integer::New(isolate, 0x8B5C) );
    gl->Set( String::NewFromUtf8(isolate, "SAMPLER_2D"), Integer::New(isolate, 0x8B5E) );
    gl->Set( String::NewFromUtf8(isolate, "SAMPLER_CUBE"), Integer::New(isolate, 0x8B60) );
    gl->Set( String::NewFromUtf8(isolate, "VERTEX_ATTRIB_ARRAY_ENABLED"), Integer::New(isolate, 0x8622) );
    gl->Set( String::NewFromUtf8(isolate, "VERTEX_ATTRIB_ARRAY_SIZE"), Integer::New(isolate, 0x8623) );
    gl->Set( String::NewFromUtf8(isolate, "VERTEX_ATTRIB_ARRAY_STRIDE"), Integer::New(isolate, 0x8624) );
    gl->Set( String::NewFromUtf8(isolate, "VERTEX_ATTRIB_ARRAY_TYPE"), Integer::New(isolate, 0x8625) );
    gl->Set( String::NewFromUtf8(isolate, "VERTEX_ATTRIB_ARRAY_NORMALIZED"), Integer::New(isolate, 0x886A) );
    gl->Set( String::NewFromUtf8(isolate, "VERTEX_ATTRIB_ARRAY_POINTER"), Integer::New(isolate, 0x8645) );
    gl->Set( String::NewFromUtf8(isolate, "VERTEX_ATTRIB_ARRAY_BUFFER_BINDING"), Integer::New(isolate, 0x889F) );
    gl->Set( String::NewFromUtf8(isolate, "IMPLEMENTATION_COLOR_READ_TYPE"), Integer::New(isolate, 0x8B9A) );
    gl->Set( String::NewFromUtf8(isolate, "IMPLEMENTATION_COLOR_READ_FORMAT"), Integer::New(isolate, 0x8B9B) );
    gl->Set( String::NewFromUtf8(isolate, "COMPILE_STATUS"), Integer::New(isolate, 0x8B81) );
    gl->Set( String::NewFromUtf8(isolate, "INFO_LOG_LENGTH"), Integer::New(isolate, 0x8B84) );
    gl->Set( String::NewFromUtf8(isolate, "SHADER_SOURCE_LENGTH"), Integer::New(isolate, 0x8B88) );
    gl->Set( String::NewFromUtf8(isolate, "SHADER_COMPILER"), Integer::New(isolate, 0x8DFA) );
    gl->Set( String::NewFromUtf8(isolate, "SHADER_BINARY_FORMATS"), Integer::New(isolate, 0x8DF8) );
    gl->Set( String::NewFromUtf8(isolate, "NUM_SHADER_BINARY_FORMATS"), Integer::New(isolate, 0x8DF9) );
    gl->Set( String::NewFromUtf8(isolate, "LOW_FLOAT"), Integer::New(isolate, 0x8DF0) );
    gl->Set( String::NewFromUtf8(isolate, "MEDIUM_FLOAT"), Integer::New(isolate, 0x8DF1) );
    gl->Set( String::NewFromUtf8(isolate, "HIGH_FLOAT"), Integer::New(isolate, 0x8DF2) );
    gl->Set( String::NewFromUtf8(isolate, "LOW_INT"), Integer::New(isolate, 0x8DF3) );
    gl->Set( String::NewFromUtf8(isolate, "MEDIUM_INT"), Integer::New(isolate, 0x8DF4) );
    gl->Set( String::NewFromUtf8(isolate, "HIGH_INT"), Integer::New(isolate, 0x8DF5) );
    gl->Set( String::NewFromUtf8(isolate, "FRAMEBUFFER"), Integer::New(isolate, 0x8D40) );
    gl->Set( String::NewFromUtf8(isolate, "RENDERBUFFER"), Integer::New(isolate, 0x8D41) );
    gl->Set( String::NewFromUtf8(isolate, "RGBA4"), Integer::New(isolate, 0x8056) );
    gl->Set( String::NewFromUtf8(isolate, "RGB5_A1"), Integer::New(isolate, 0x8057) );
    gl->Set( String::NewFromUtf8(isolate, "RGB565"), Integer::New(isolate, 0x8D62) );
    gl->Set( String::NewFromUtf8(isolate, "DEPTH_COMPONENT16"), Integer::New(isolate, 0x81A5) );
    gl->Set( String::NewFromUtf8(isolate, "STENCIL_INDEX"), Integer::New(isolate, 0x1901) );
    gl->Set( String::NewFromUtf8(isolate, "STENCIL_INDEX8"), Integer::New(isolate, 0x8D48) );
    gl->Set( String::NewFromUtf8(isolate, "RENDERBUFFER_WIDTH"), Integer::New(isolate, 0x8D42) );
    gl->Set( String::NewFromUtf8(isolate, "RENDERBUFFER_HEIGHT"), Integer::New(isolate, 0x8D43) );
    gl->Set( String::NewFromUtf8(isolate, "RENDERBUFFER_INTERNAL_FORMAT"), Integer::New(isolate, 0x8D44) );
    gl->Set( String::NewFromUtf8(isolate, "RENDERBUFFER_RED_SIZE"), Integer::New(isolate, 0x8D50) );
    gl->Set( String::NewFromUtf8(isolate, "RENDERBUFFER_GREEN_SIZE"), Integer::New(isolate, 0x8D51) );
    gl->Set( String::NewFromUtf8(isolate, "RENDERBUFFER_BLUE_SIZE"), Integer::New(isolate, 0x8D52) );
    gl->Set( String::NewFromUtf8(isolate, "RENDERBUFFER_ALPHA_SIZE"), Integer::New(isolate, 0x8D53) );
    gl->Set( String::NewFromUtf8(isolate, "RENDERBUFFER_DEPTH_SIZE"), Integer::New(isolate, 0x8D54) );
    gl->Set( String::NewFromUtf8(isolate, "RENDERBUFFER_STENCIL_SIZE"), Integer::New(isolate, 0x8D55) );
    gl->Set( String::NewFromUtf8(isolate, "FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE"), Integer::New(isolate, 0x8CD0) );
    gl->Set( String::NewFromUtf8(isolate, "FRAMEBUFFER_ATTACHMENT_OBJECT_NAME"), Integer::New(isolate, 0x8CD1) );
    gl->Set( String::NewFromUtf8(isolate, "FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL"), Integer::New(isolate, 0x8CD2) );
    gl->Set( String::NewFromUtf8(isolate, "FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE"), Integer::New(isolate, 0x8CD3) );
    gl->Set( String::NewFromUtf8(isolate, "DEPTH_ATTACHMENT"), Integer::New(isolate, 0x8D00) );
    gl->Set( String::NewFromUtf8(isolate, "STENCIL_ATTACHMENT"), Integer::New(isolate, 0x8D20) );
    gl->Set( String::NewFromUtf8(isolate, "NONE"), Integer::New(isolate, 0) );
    gl->Set( String::NewFromUtf8(isolate, "FRAMEBUFFER_COMPLETE"), Integer::New(isolate, 0x8CD5) );
    gl->Set( String::NewFromUtf8(isolate, "FRAMEBUFFER_INCOMPLETE_ATTACHMENT"), Integer::New(isolate, 0x8CD6) );
    gl->Set( String::NewFromUtf8(isolate, "FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT"), Integer::New(isolate, 0x8CD7) );
    gl->Set( String::NewFromUtf8(isolate, "FRAMEBUFFER_INCOMPLETE_DIMENSIONS"), Integer::New(isolate, 0x8CD9) );
    gl->Set( String::NewFromUtf8(isolate, "FRAMEBUFFER_UNSUPPORTED"), Integer::New(isolate, 0x8CDD) );
    gl->Set( String::NewFromUtf8(isolate, "FRAMEBUFFER_BINDING"), Integer::New(isolate, 0x8CA6) );
    gl->Set( String::NewFromUtf8(isolate, "RENDERBUFFER_BINDING"), Integer::New(isolate, 0x8CA7) );
    gl->Set( String::NewFromUtf8(isolate, "MAX_RENDERBUFFER_SIZE"), Integer::New(isolate, 0x84E8) );
    gl->Set( String::NewFromUtf8(isolate, "INVALID_FRAMEBUFFER_OPERATION"), Integer::New(isolate, 0x0506) );
    gl->Set( String::NewFromUtf8(isolate, "DRAW_FRAMEBUFFER"), Integer::New(isolate, GL_DRAW_FRAMEBUFFER) );
    gl->Set( String::NewFromUtf8(isolate, "READ_FRAMEBUFFER"), Integer::New(isolate, GL_READ_FRAMEBUFFER) );

    gl->Set( String::NewFromUtf8(isolate, "RGBA16F"), Number::New(isolate, GL_RGBA16F) );
    gl->Set( String::NewFromUtf8(isolate, "RGBA32F"), Number::New(isolate, GL_RGBA32F) );
    gl->Set( String::NewFromUtf8(isolate, "RGB16F"), Number::New(isolate, GL_RGB16F) );
    gl->Set( String::NewFromUtf8(isolate, "RGB32F"), Number::New(isolate, GL_RGB32F) );
    gl->Set( String::NewFromUtf8(isolate, "UNIFORM_BUFFER"), Number::New(isolate, GL_UNIFORM_BUFFER) );
    gl->Set( String::NewFromUtf8(isolate, "HALF_FLOAT"), Number::New(isolate, GL_HALF_FLOAT) );
    gl->Set( String::NewFromUtf8(isolate, "TEXTURE_EXTERNAL_OES"), Number::New(isolate, 0x8D65) );

    gl->Set( String::NewFromUtf8(isolate, "COLOR_ATTACHMENT0"), Integer::New(isolate, GL_COLOR_ATTACHMENT0) );
    gl->Set( String::NewFromUtf8(isolate, "COLOR_ATTACHMENT1"), Integer::New(isolate, GL_COLOR_ATTACHMENT1) );
    gl->Set( String::NewFromUtf8(isolate, "COLOR_ATTACHMENT2"), Integer::New(isolate, GL_COLOR_ATTACHMENT2) );
    gl->Set( String::NewFromUtf8(isolate, "COLOR_ATTACHMENT3"), Integer::New(isolate, GL_COLOR_ATTACHMENT3) );
    gl->Set( String::NewFromUtf8(isolate, "COLOR_ATTACHMENT4"), Integer::New(isolate, GL_COLOR_ATTACHMENT4) );
    gl->Set( String::NewFromUtf8(isolate, "COLOR_ATTACHMENT5"), Integer::New(isolate, GL_COLOR_ATTACHMENT5) );
    gl->Set( String::NewFromUtf8(isolate, "COLOR_ATTACHMENT6"), Integer::New(isolate, GL_COLOR_ATTACHMENT6) );
    gl->Set( String::NewFromUtf8(isolate, "COLOR_ATTACHMENT7"), Integer::New(isolate, GL_COLOR_ATTACHMENT7) );
    gl->Set( String::NewFromUtf8(isolate, "COLOR_ATTACHMENT8"), Integer::New(isolate, GL_COLOR_ATTACHMENT8) );
    gl->Set( String::NewFromUtf8(isolate, "COLOR_ATTACHMENT9"), Integer::New(isolate, GL_COLOR_ATTACHMENT9) );
    gl->Set( String::NewFromUtf8(isolate, "COLOR_ATTACHMENT10"), Integer::New(isolate, GL_COLOR_ATTACHMENT10) );
    gl->Set( String::NewFromUtf8(isolate, "COLOR_ATTACHMENT11"), Integer::New(isolate, GL_COLOR_ATTACHMENT11) );
    gl->Set( String::NewFromUtf8(isolate, "COLOR_ATTACHMENT12"), Integer::New(isolate, GL_COLOR_ATTACHMENT12) );
    gl->Set( String::NewFromUtf8(isolate, "COLOR_ATTACHMENT13"), Integer::New(isolate, GL_COLOR_ATTACHMENT13) );
    gl->Set( String::NewFromUtf8(isolate, "COLOR_ATTACHMENT14"), Integer::New(isolate, GL_COLOR_ATTACHMENT14) );
    gl->Set( String::NewFromUtf8(isolate, "COLOR_ATTACHMENT15"), Integer::New(isolate, GL_COLOR_ATTACHMENT15) );
    gl->Set( String::NewFromUtf8(isolate, "DEPTH_COMPONENT24"), Integer::New(isolate, GL_DEPTH_COMPONENT24) );

    gl->Set( String::NewFromUtf8(isolate, "activeTexture"), Function::New(isolate, v8Bind_ActiveTexture) );
    gl->Set( String::NewFromUtf8(isolate, "attachShader"), Function::New(isolate, v8Bind_AttachShader) );
    gl->Set( String::NewFromUtf8(isolate, "bindAttribLocation"), Function::New(isolate, v8Bind_BindAttribLocation) );
    gl->Set( String::NewFromUtf8(isolate, "bindBuffer"), Function::New(isolate, v8Bind_BindBuffer) );
    gl->Set( String::NewFromUtf8(isolate, "bindFramebuffer"), Function::New(isolate, v8Bind_BindFramebuffer) );
    gl->Set( String::NewFromUtf8(isolate, "bindRenderbuffer"), Function::New(isolate, v8Bind_BindRenderbuffer) );
    gl->Set( String::NewFromUtf8(isolate, "bindTexture"), Function::New(isolate, v8Bind_BindTexture) );
    gl->Set( String::NewFromUtf8(isolate, "blendColor"), Function::New(isolate, v8Bind_BlendColor) );
    gl->Set( String::NewFromUtf8(isolate, "blendEquation"), Function::New(isolate, v8Bind_BlendEquation) );
    gl->Set( String::NewFromUtf8(isolate, "blendEquationSeparate"), Function::New(isolate, v8Bind_BlendEquationSeparate) );
    gl->Set( String::NewFromUtf8(isolate, "blendFunc"), Function::New(isolate, v8Bind_BlendFunc) );
    gl->Set( String::NewFromUtf8(isolate, "blendFuncSeparate"), Function::New(isolate, v8Bind_BlendFuncSeparate) );
    gl->Set( String::NewFromUtf8(isolate, "bufferData"), Function::New(isolate, v8Bind_BufferData) );
    gl->Set( String::NewFromUtf8(isolate, "bufferSubData"), Function::New(isolate, v8Bind_BufferSubData) );
    gl->Set( String::NewFromUtf8(isolate, "checkFramebufferStatus"), Function::New(isolate, v8Bind_CheckFramebufferStatus) );
    gl->Set( String::NewFromUtf8(isolate, "clear"), Function::New(isolate, v8Bind_Clear) );
    gl->Set( String::NewFromUtf8(isolate, "clearColor"), Function::New(isolate, v8Bind_ClearColor) );
    gl->Set( String::NewFromUtf8(isolate, "clearDepthf"), Function::New(isolate, v8Bind_ClearDepthf) );
    gl->Set( String::NewFromUtf8(isolate, "clearStencil"), Function::New(isolate, v8Bind_ClearStencil) );
    gl->Set( String::NewFromUtf8(isolate, "colorMask"), Function::New(isolate, v8Bind_ColorMask) );
    gl->Set( String::NewFromUtf8(isolate, "compileShader"), Function::New(isolate, v8Bind_CompileShader) );
    gl->Set( String::NewFromUtf8(isolate, "compressedTexImage2D"), Function::New(isolate, v8Bind_CompressedTexImage2D) );
    gl->Set( String::NewFromUtf8(isolate, "compressedTexSubImage2D"), Function::New(isolate, v8Bind_CompressedTexSubImage2D) );
    gl->Set( String::NewFromUtf8(isolate, "copyTexImage2D"), Function::New(isolate, v8Bind_CopyTexImage2D) );
    gl->Set( String::NewFromUtf8(isolate, "copyTexSubImage2D"), Function::New(isolate, v8Bind_CopyTexSubImage2D) );
    gl->Set( String::NewFromUtf8(isolate, "createProgram"), Function::New(isolate, v8Bind_CreateProgram) );
    gl->Set( String::NewFromUtf8(isolate, "createShader"), Function::New(isolate, v8Bind_CreateShader) );
    gl->Set( String::NewFromUtf8(isolate, "cullFace"), Function::New(isolate, v8Bind_CullFace) );
    gl->Set( String::NewFromUtf8(isolate, "deleteBuffers"), Function::New(isolate, v8Bind_DeleteBuffers) );
    gl->Set( String::NewFromUtf8(isolate, "deleteFramebuffers"), Function::New(isolate, v8Bind_DeleteFramebuffers) );
    gl->Set( String::NewFromUtf8(isolate, "deleteProgram"), Function::New(isolate, v8Bind_DeleteProgram) );
    gl->Set( String::NewFromUtf8(isolate, "deleteRenderbuffers"), Function::New(isolate, v8Bind_DeleteRenderbuffers) );
    gl->Set( String::NewFromUtf8(isolate, "deleteShader"), Function::New(isolate, v8Bind_DeleteShader) );
    gl->Set( String::NewFromUtf8(isolate, "deleteTextures"), Function::New(isolate, v8Bind_DeleteTextures) );
    gl->Set( String::NewFromUtf8(isolate, "depthFunc"), Function::New(isolate, v8Bind_DepthFunc) );
    gl->Set( String::NewFromUtf8(isolate, "depthMask"), Function::New(isolate, v8Bind_DepthMask) );
    gl->Set( String::NewFromUtf8(isolate, "depthRangef"), Function::New(isolate, v8Bind_DepthRangef) );
    gl->Set( String::NewFromUtf8(isolate, "detachShader"), Function::New(isolate, v8Bind_DetachShader) );
    gl->Set( String::NewFromUtf8(isolate, "disable"), Function::New(isolate, v8Bind_Disable) );
    gl->Set( String::NewFromUtf8(isolate, "disableVertexAttribArray"), Function::New(isolate, v8Bind_DisableVertexAttribArray) );
    gl->Set( String::NewFromUtf8(isolate, "drawArrays"), Function::New(isolate, v8Bind_DrawArrays) );
    gl->Set( String::NewFromUtf8(isolate, "drawElements"), Function::New(isolate, v8Bind_DrawElements) );
    gl->Set( String::NewFromUtf8(isolate, "enable"), Function::New(isolate, v8Bind_Enable) );
    gl->Set( String::NewFromUtf8(isolate, "enableVertexAttribArray"), Function::New(isolate, v8Bind_EnableVertexAttribArray) );
    gl->Set( String::NewFromUtf8(isolate, "finish"), Function::New(isolate, v8Bind_Finish) );
    gl->Set( String::NewFromUtf8(isolate, "flush"), Function::New(isolate, v8Bind_Flush) );
    gl->Set( String::NewFromUtf8(isolate, "framebufferRenderbuffer"), Function::New(isolate, v8Bind_FramebufferRenderbuffer) );
    gl->Set( String::NewFromUtf8(isolate, "framebufferTexture2D"), Function::New(isolate, v8Bind_FramebufferTexture2D) );
    gl->Set( String::NewFromUtf8(isolate, "frontFace"), Function::New(isolate, v8Bind_FrontFace) );
    gl->Set( String::NewFromUtf8(isolate, "genBuffers"), Function::New(isolate, v8Bind_GenBuffers) );
    gl->Set( String::NewFromUtf8(isolate, "generateMipmap"), Function::New(isolate, v8Bind_GenerateMipmap) );
    gl->Set( String::NewFromUtf8(isolate, "genFramebuffers"), Function::New(isolate, v8Bind_GenFramebuffers) );
    gl->Set( String::NewFromUtf8(isolate, "genRenderbuffers"), Function::New(isolate, v8Bind_GenRenderbuffers) );
    gl->Set( String::NewFromUtf8(isolate, "genTextures"), Function::New(isolate, v8Bind_GenTextures) );
    gl->Set( String::NewFromUtf8(isolate, "getActiveAttrib"), Function::New(isolate, v8Bind_GetActiveAttrib) );
    gl->Set( String::NewFromUtf8(isolate, "getActiveUniform"), Function::New(isolate, v8Bind_GetActiveUniform) );
    gl->Set( String::NewFromUtf8(isolate, "getAttachedShaders"), Function::New(isolate, v8Bind_GetAttachedShaders) );
    gl->Set( String::NewFromUtf8(isolate, "getAttribLocation"), Function::New(isolate, v8Bind_GetAttribLocation) );
    gl->Set( String::NewFromUtf8(isolate, "getBooleanv"), Function::New(isolate, v8Bind_GetBooleanv) );
    gl->Set( String::NewFromUtf8(isolate, "getBufferParameteriv"), Function::New(isolate, v8Bind_GetBufferParameteriv) );
    gl->Set( String::NewFromUtf8(isolate, "getError"), Function::New(isolate, v8Bind_GetError) );
    gl->Set( String::NewFromUtf8(isolate, "getFloatv"), Function::New(isolate, v8Bind_GetFloatv) );
    gl->Set( String::NewFromUtf8(isolate, "getFramebufferAttachmentParameteriv"), Function::New(isolate, v8Bind_GetFramebufferAttachmentParameteriv) );
    gl->Set( String::NewFromUtf8(isolate, "getIntegerv"), Function::New(isolate, v8Bind_GetIntegerv) );
    gl->Set( String::NewFromUtf8(isolate, "getProgramiv"), Function::New(isolate, v8Bind_GetProgramiv) );
    gl->Set( String::NewFromUtf8(isolate, "getProgramInfoLog"), Function::New(isolate, v8Bind_GetProgramInfoLog) );
    gl->Set( String::NewFromUtf8(isolate, "getRenderbufferParameteriv"), Function::New(isolate, v8Bind_GetRenderbufferParameteriv) );
    gl->Set( String::NewFromUtf8(isolate, "getShaderiv"), Function::New(isolate, v8Bind_GetShaderiv) );
    gl->Set( String::NewFromUtf8(isolate, "getShaderInfoLog"), Function::New(isolate, v8Bind_GetShaderInfoLog) );
    gl->Set( String::NewFromUtf8(isolate, "getShaderPrecisionFormat"), Function::New(isolate, v8Bind_GetShaderPrecisionFormat) );
    gl->Set( String::NewFromUtf8(isolate, "getShaderSource"), Function::New(isolate, v8Bind_GetShaderSource) );
    gl->Set( String::NewFromUtf8(isolate, "getString"), Function::New(isolate, v8Bind_GetString) );
    gl->Set( String::NewFromUtf8(isolate, "getTexParameterfv"), Function::New(isolate, v8Bind_GetTexParameterfv) );
    gl->Set( String::NewFromUtf8(isolate, "getTexParameteriv"), Function::New(isolate, v8Bind_GetTexParameteriv) );
    gl->Set( String::NewFromUtf8(isolate, "getUniformfv"), Function::New(isolate, v8Bind_GetUniformfv) );
    gl->Set( String::NewFromUtf8(isolate, "getUniformiv"), Function::New(isolate, v8Bind_GetUniformiv) );
    gl->Set( String::NewFromUtf8(isolate, "getUniformLocation"), Function::New(isolate, v8Bind_GetUniformLocation) );
    gl->Set( String::NewFromUtf8(isolate, "getVertexAttribfv"), Function::New(isolate, v8Bind_GetVertexAttribfv) );
    gl->Set( String::NewFromUtf8(isolate, "getVertexAttribiv"), Function::New(isolate, v8Bind_GetVertexAttribiv) );
    gl->Set( String::NewFromUtf8(isolate, "getVertexAttribPointerv"), Function::New(isolate, v8Bind_GetVertexAttribPointerv) );
    gl->Set( String::NewFromUtf8(isolate, "hint"), Function::New(isolate, v8Bind_Hint) );
    gl->Set( String::NewFromUtf8(isolate, "isBuffer"), Function::New(isolate, v8Bind_IsBuffer) );
    gl->Set( String::NewFromUtf8(isolate, "isEnabled"), Function::New(isolate, v8Bind_IsEnabled) );
    gl->Set( String::NewFromUtf8(isolate, "isFramebuffer"), Function::New(isolate, v8Bind_IsFramebuffer) );
    gl->Set( String::NewFromUtf8(isolate, "isProgram"), Function::New(isolate, v8Bind_IsProgram) );
    gl->Set( String::NewFromUtf8(isolate, "isRenderbuffer"), Function::New(isolate, v8Bind_IsRenderbuffer) );
    gl->Set( String::NewFromUtf8(isolate, "isShader"), Function::New(isolate, v8Bind_IsShader) );
    gl->Set( String::NewFromUtf8(isolate, "isTexture"), Function::New(isolate, v8Bind_IsTexture) );
    gl->Set( String::NewFromUtf8(isolate, "lineWidth"), Function::New(isolate, v8Bind_LineWidth) );
    gl->Set( String::NewFromUtf8(isolate, "linkProgram"), Function::New(isolate, v8Bind_LinkProgram) );
    gl->Set( String::NewFromUtf8(isolate, "pixelStorei"), Function::New(isolate, v8Bind_PixelStorei) );
    gl->Set( String::NewFromUtf8(isolate, "polygonOffset"), Function::New(isolate, v8Bind_PolygonOffset) );
    gl->Set( String::NewFromUtf8(isolate, "readPixels"), Function::New(isolate, v8Bind_ReadPixels) );
    gl->Set( String::NewFromUtf8(isolate, "releaseShaderCompiler"), Function::New(isolate, v8Bind_ReleaseShaderCompiler) );
    gl->Set( String::NewFromUtf8(isolate, "renderbufferStorage"), Function::New(isolate, v8Bind_RenderbufferStorage) );
    gl->Set( String::NewFromUtf8(isolate, "sampleCoverage"), Function::New(isolate, v8Bind_SampleCoverage) );
    gl->Set( String::NewFromUtf8(isolate, "scissor"), Function::New(isolate, v8Bind_Scissor) );
    gl->Set( String::NewFromUtf8(isolate, "shaderBinary"), Function::New(isolate, v8Bind_ShaderBinary) );
    gl->Set( String::NewFromUtf8(isolate, "shaderSource"), Function::New(isolate, v8Bind_ShaderSource) );
    gl->Set( String::NewFromUtf8(isolate, "stencilFunc"), Function::New(isolate, v8Bind_StencilFunc) );
    gl->Set( String::NewFromUtf8(isolate, "stencilFuncSeparate"), Function::New(isolate, v8Bind_StencilFuncSeparate) );
    gl->Set( String::NewFromUtf8(isolate, "stencilMask"), Function::New(isolate, v8Bind_StencilMask) );
    gl->Set( String::NewFromUtf8(isolate, "stencilMaskSeparate"), Function::New(isolate, v8Bind_StencilMaskSeparate) );
    gl->Set( String::NewFromUtf8(isolate, "stencilOp"), Function::New(isolate, v8Bind_StencilOp) );
    gl->Set( String::NewFromUtf8(isolate, "stencilOpSeparate"), Function::New(isolate, v8Bind_StencilOpSeparate) );
    gl->Set( String::NewFromUtf8(isolate, "texParameterf"), Function::New(isolate, v8Bind_TexParameterf) );
    gl->Set( String::NewFromUtf8(isolate, "texParameterfv"), Function::New(isolate, v8Bind_TexParameterfv) );
    gl->Set( String::NewFromUtf8(isolate, "texParameteri"), Function::New(isolate, v8Bind_TexParameteri) );
    gl->Set( String::NewFromUtf8(isolate, "texParameteriv"), Function::New(isolate, v8Bind_TexParameteriv) );
    gl->Set( String::NewFromUtf8(isolate, "texSubImage2D"), Function::New(isolate, v8Bind_TexSubImage2D) );
    gl->Set( String::NewFromUtf8(isolate, "uniform1f"), Function::New(isolate, v8Bind_Uniform1f) );
    gl->Set( String::NewFromUtf8(isolate, "uniform1fv"), Function::New(isolate, v8Bind_Uniform1fv) );
    gl->Set( String::NewFromUtf8(isolate, "uniform1i"), Function::New(isolate, v8Bind_Uniform1i) );
    gl->Set( String::NewFromUtf8(isolate, "uniform1iv"), Function::New(isolate, v8Bind_Uniform1iv) );
    gl->Set( String::NewFromUtf8(isolate, "uniform2f"), Function::New(isolate, v8Bind_Uniform2f) );
    gl->Set( String::NewFromUtf8(isolate, "uniform2fv"), Function::New(isolate, v8Bind_Uniform2fv) );
    gl->Set( String::NewFromUtf8(isolate, "uniform2i"), Function::New(isolate, v8Bind_Uniform2i) );
    gl->Set( String::NewFromUtf8(isolate, "uniform2iv"), Function::New(isolate, v8Bind_Uniform2iv) );
    gl->Set( String::NewFromUtf8(isolate, "uniform3f"), Function::New(isolate, v8Bind_Uniform3f) );
    gl->Set( String::NewFromUtf8(isolate, "uniform3fv"), Function::New(isolate, v8Bind_Uniform3fv) );
    gl->Set( String::NewFromUtf8(isolate, "uniform3i"), Function::New(isolate, v8Bind_Uniform3i) );
    gl->Set( String::NewFromUtf8(isolate, "uniform3iv"), Function::New(isolate, v8Bind_Uniform3iv) );
    gl->Set( String::NewFromUtf8(isolate, "uniform4f"), Function::New(isolate, v8Bind_Uniform4f) );
    gl->Set( String::NewFromUtf8(isolate, "uniform4fv"), Function::New(isolate, v8Bind_Uniform4fv) );
    gl->Set( String::NewFromUtf8(isolate, "uniform4i"), Function::New(isolate, v8Bind_Uniform4i) );
    gl->Set( String::NewFromUtf8(isolate, "uniform4iv"), Function::New(isolate, v8Bind_Uniform4iv) );
    gl->Set( String::NewFromUtf8(isolate, "uniformMatrix2fv"), Function::New(isolate, v8Bind_UniformMatrix2fv) );
    gl->Set( String::NewFromUtf8(isolate, "uniformMatrix3fv"), Function::New(isolate, v8Bind_UniformMatrix3fv) );
    gl->Set( String::NewFromUtf8(isolate, "uniformMatrix4fv"), Function::New(isolate, v8Bind_UniformMatrix4fv) );
    gl->Set( String::NewFromUtf8(isolate, "useProgram"), Function::New(isolate, v8Bind_UseProgram) );
    gl->Set( String::NewFromUtf8(isolate, "validateProgram"), Function::New(isolate, v8Bind_ValidateProgram) );
    gl->Set( String::NewFromUtf8(isolate, "vertexAttrib1f"), Function::New(isolate, v8Bind_VertexAttrib1f) );
    gl->Set( String::NewFromUtf8(isolate, "vertexAttrib1fv"), Function::New(isolate, v8Bind_VertexAttrib1fv) );
    gl->Set( String::NewFromUtf8(isolate, "vertexAttrib2f"), Function::New(isolate, v8Bind_VertexAttrib2f) );
    gl->Set( String::NewFromUtf8(isolate, "vertexAttrib2fv"), Function::New(isolate, v8Bind_VertexAttrib2fv) );
    gl->Set( String::NewFromUtf8(isolate, "vertexAttrib3f"), Function::New(isolate, v8Bind_VertexAttrib3f) );
    gl->Set( String::NewFromUtf8(isolate, "vertexAttrib3fv"), Function::New(isolate, v8Bind_VertexAttrib3fv) );
    gl->Set( String::NewFromUtf8(isolate, "vertexAttrib4f"), Function::New(isolate, v8Bind_VertexAttrib4f) );
    gl->Set( String::NewFromUtf8(isolate, "vertexAttrib4fv"), Function::New(isolate, v8Bind_VertexAttrib4fv) );
    gl->Set( String::NewFromUtf8(isolate, "vertexAttribPointer"), Function::New(isolate, v8Bind_VertexAttribPointer) );
    gl->Set( String::NewFromUtf8(isolate, "viewport"), Function::New(isolate, v8Bind_Viewport) );

    gl->Set( String::NewFromUtf8(isolate, "getParameter"), Function::New(isolate, v8Bind_GetParameter) );
    gl->Set( String::NewFromUtf8(isolate, "getExtension"), Function::New(isolate, v8Bind_GetExtension) );

    gl->Set( String::NewFromUtf8(isolate, "_texImage2DNull"), Function::New(isolate, AURA_TexImage2DNull) );
    gl->Set( String::NewFromUtf8(isolate, "_texImage2DArray"), Function::New(isolate, AURA_TexImage2DArray) );
    gl->Set( String::NewFromUtf8(isolate, "_getExtensions"), Function::New(isolate, AURA_GetExtensions) );
    gl->Set( String::NewFromUtf8(isolate, "drawElementsInstanced"), Function::New(isolate, AURA_DrawElementsInstanced) );
    gl->Set( String::NewFromUtf8(isolate, "drawArraysInstanced"), Function::New(isolate, AURA_DrawArraysInstanced) );
    gl->Set( String::NewFromUtf8(isolate, "vertexAttribDivisor"), Function::New(isolate, AURA_VertexAttribDivisor) );
    gl->Set( String::NewFromUtf8(isolate, "getUniformBlockIndex"), Function::New(isolate, AURA_GetUniformBlockIndex) );
    gl->Set( String::NewFromUtf8(isolate, "uniformBlockBinding"), Function::New(isolate, AURA_UniformBlockBinding) );
    gl->Set( String::NewFromUtf8(isolate, "bindBufferBase"), Function::New(isolate, AURA_BindBufferBase) );
    gl->Set( String::NewFromUtf8(isolate, "createVertexArray"), Function::New(isolate, AURA_CreateVertexArray) );
    gl->Set( String::NewFromUtf8(isolate, "bindVertexArray"), Function::New(isolate, AURA_BindVertexArray) );
    gl->Set( String::NewFromUtf8(isolate, "deleteVertexArray"), Function::New(isolate, AURA_DeleteVertexArray) );
    gl->Set( String::NewFromUtf8(isolate, "drawBuffers"), Function::New(isolate, AURA_DrawBuffers) );
    gl->Set( String::NewFromUtf8(isolate, "blitFramebuffer"), Function::New(isolate, AURA_BlitFramebuffer) );

}


JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1initAura
(JNIEnv *env, jobject, jlong v8RuntimePtr) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);

  Local<Object> gl = Object::New(isolate);
  context->Global()->Set(v8::String::NewFromUtf8(isolate, "_gl"), gl);
    
  initializeAura(runtime, gl);
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

JNIEXPORT jboolean JNICALL Java_com_eclipsesource_v8_V8__1isWeak
  (JNIEnv * env, jobject, jlong v8RuntimePtr, jlong objectHandle) {
    Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
    return reinterpret_cast<Persistent<Object>*>(objectHandle)->IsWeak();
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1registerJavaMethod
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jstring functionName, jboolean voidMethod) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  FunctionCallback callback = voidCallback;
  if (!voidMethod) {
    callback = objectCallback;
  }
  Handle<Object> object = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  Local<String> v8FunctionName = createV8String(env, isolate, functionName);
  isolate->IdleNotification(1000);
  MethodDescriptor* md= new MethodDescriptor();
  Local<External> ext =  External::New(isolate, md);
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
  object->Set(v8FunctionName, Function::New(isolate, callback, ext));
  return md->methodID;
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1releaseMethodDescriptor
  (JNIEnv *, jobject, jlong, jlong methodDescriptorPtr) {
  MethodDescriptor* md = reinterpret_cast<MethodDescriptor*>(methodDescriptorPtr);
  delete(md);
}

JNIEXPORT void JNICALL Java_com_eclipsesource_v8_V8__1setPrototype
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle, jlong prototypeHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, );
  Handle<Object> object = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  Handle<Object> prototype = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(prototypeHandle));
  object->SetPrototype(prototype);
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
  return object->Equals(that);
}

JNIEXPORT jstring JNICALL Java_com_eclipsesource_v8_V8__1toString
(JNIEnv *env, jobject, jlong v8RuntimePtr, jlong objectHandle) {
  Isolate* isolate = SETUP(env, v8RuntimePtr, 0);
  Handle<Object> object = Local<Object>::New(isolate, *reinterpret_cast<Persistent<Object>*>(objectHandle));
  String::Value unicodeString(object->ToString(isolate));

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
    return NULL;
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
  jstring jstackTrace = NULL;
  if (stackTrace != NULL) {
    jstackTrace = env->NewStringUTF(stackTrace);
  }
  jthrowable wrappedException = NULL;
  if (env->ExceptionCheck()) {
    wrappedException = env->ExceptionOccurred();
    env->ExceptionClear();
  }
  if (reinterpret_cast<V8Runtime*>(v8RuntimePtr)->pendingException != NULL) {
    wrappedException = reinterpret_cast<V8Runtime*>(v8RuntimePtr)->pendingException;
    reinterpret_cast<V8Runtime*>(v8RuntimePtr)->pendingException = NULL;
  }
  if ( wrappedException != NULL && !env->IsInstanceOf( wrappedException, throwableCls) ) {
    std::cout << "Wrapped Exception is not a Throwable" << std::endl;
    wrappedException = NULL;
  }
  jthrowable result = (jthrowable)env->NewObject(v8ScriptExecutionException, v8ScriptExecutionExceptionInitMethodID, jfileName, lineNumber, jmessage, jsourceLine, startColumn, endColumn, jstackTrace, wrappedException);
  env->DeleteLocalRef(jfileName);
  env->DeleteLocalRef(jmessage);
  env->DeleteLocalRef(jsourceLine);
  (env)->Throw(result);
}

void throwParseException(JNIEnv *env, Isolate* isolate, TryCatch* tryCatch) {
  String::Value exception(tryCatch->Exception());
  Handle<Message> message = tryCatch->Message();
  if (message.IsEmpty()) {
    throwV8RuntimeException(env, &exception);
  }
  else {
    String::Utf8Value filename(message->GetScriptResourceName());
    int lineNumber = message->GetLineNumber();
    String::Value sourceline(message->GetSourceLine());
    int start = message->GetStartColumn();
    int end = message->GetEndColumn();
    const char* filenameString = ToCString(filename);
    throwParseException(env, filenameString, lineNumber, &exception, &sourceline, start, end);
  }
}

void throwExecutionException(JNIEnv *env, Isolate* isolate, TryCatch* tryCatch, jlong v8RuntimePtr) {
  String::Value exception(tryCatch->Exception());
  Handle<Message> message = tryCatch->Message();
  if (message.IsEmpty()) {
    throwV8RuntimeException(env, &exception);
  }
  else {
    String::Utf8Value filename(message->GetScriptResourceName());
    int lineNumber = message->GetLineNumber();
    String::Value sourceline(message->GetSourceLine());
    int start = message->GetStartColumn();
    int end = message->GetEndColumn();
    const char* filenameString = ToCString(filename);
    String::Utf8Value stack_trace(tryCatch->StackTrace());
    const char* stackTrace = NULL;
    if (stack_trace.length() > 0) {
      stackTrace = ToCString(stack_trace);
    }
    throwExecutionException(env, filenameString, lineNumber, &exception, &sourceline, start, end, stackTrace, v8RuntimePtr);
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

jobject getResult(JNIEnv *env, jobject &v8, jlong v8RuntimePtr, Handle<Value> &result, jint expectedType) {
  if (result->IsUndefined() && expectedType == com_eclipsesource_v8_V8_V8_ARRAY) {
    jobject objectResult = env->NewObject(undefinedV8ArrayCls, undefinedV8ArrayInitMethodID, v8);
    return objectResult;
  }
  else if (result->IsUndefined() && (expectedType == com_eclipsesource_v8_V8_V8_OBJECT || expectedType == com_eclipsesource_v8_V8_NULL)) {
    jobject objectResult = env->NewObject(undefinedV8ObjectCls, undefinedV8ObjectInitMethodID, v8);
    return objectResult;
  }
  else if (result->IsInt32()) {
    return env->NewObject(integerCls, integerInitMethodID, result->Int32Value());
  }
  else if (result->IsNumber()) {
    return env->NewObject(doubleCls, doubleInitMethodID, result->NumberValue());
  }
  else if (result->IsBoolean()) {
    return env->NewObject(booleanCls, booleanInitMethodID, result->BooleanValue());
  }
  else if (result->IsString()) {
    v8::Isolate* isolate = reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate;

    String::Value unicodeString(result->ToString(isolate));

    return env->NewString(*unicodeString, unicodeString.length());
  }
  else if (result->IsFunction()) {
    jobject objectResult = env->NewObject(v8FunctionCls, v8FunctionInitMethodID, v8);
    jlong resultHandle = getHandle(env, objectResult);

    v8::Isolate* isolate = reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate;

    reinterpret_cast<Persistent<Object>*>(resultHandle)->Reset(isolate, result->ToObject(isolate));

    return objectResult;
  }
  else if (result->IsArray()) {
    jobject objectResult = env->NewObject(v8ArrayCls, v8ArrayInitMethodID, v8);
    jlong resultHandle = getHandle(env, objectResult);

    v8::Isolate* isolate = reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate;

    reinterpret_cast<Persistent<Object>*>(resultHandle)->Reset(isolate, result->ToObject(isolate));

    return objectResult;
  }
  else if (result->IsTypedArray()) {
      jobject objectResult = env->NewObject(v8TypedArrayCls, v8TypedArrayInitMethodID, v8);
      jlong resultHandle = getHandle(env, objectResult);

      v8::Isolate* isolate = reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate;

      reinterpret_cast<Persistent<Object>*>(resultHandle)->Reset(isolate, result->ToObject(isolate));

      return objectResult;
  }
  else if (result->IsArrayBuffer()) {
    ArrayBuffer* arrayBuffer = ArrayBuffer::Cast(*result);
    if ( arrayBuffer->GetContents().Data() == NULL ) {
      jobject byteBuffer = env->NewDirectByteBuffer(arrayBuffer->GetContents().Data(), 0);
      jobject objectResult = env->NewObject(v8ArrayBufferCls, v8ArrayBufferInitMethodID, v8, byteBuffer);
      jlong resultHandle = getHandle(env, objectResult);
      v8::Isolate* isolate = reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate;
      reinterpret_cast<Persistent<Object>*>(resultHandle)->Reset(isolate, result->ToObject(isolate));
      return objectResult;
    }
    jobject byteBuffer = env->NewDirectByteBuffer(arrayBuffer->GetContents().Data(), arrayBuffer->ByteLength());
    jobject objectResult = env->NewObject(v8ArrayBufferCls, v8ArrayBufferInitMethodID, v8, byteBuffer);
    jlong resultHandle = getHandle(env, objectResult);

    v8::Isolate* isolate = reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate;

    reinterpret_cast<Persistent<Object>*>(resultHandle)->Reset(isolate, result->ToObject(isolate));

    return objectResult;
  }
  else if (result->IsObject()) {
    jobject objectResult = env->NewObject(v8ObjectCls, v8ObjectInitMethodID, v8);
    jlong resultHandle = getHandle(env, objectResult);

    v8::Isolate* isolate = reinterpret_cast<V8Runtime*>(v8RuntimePtr)->isolate;

    reinterpret_cast<Persistent<Object>*>(resultHandle)->Reset(isolate, result->ToObject(isolate));

    return objectResult;
  }

  return NULL;
}

JNIEXPORT jlong JNICALL Java_com_eclipsesource_v8_V8__1getBuildID
  (JNIEnv *, jobject) {
  return 2;
}
