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
// #define TO_OBJECT(val) Nan::To<v8::Object>(val).ToLocalChecked()
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

void v8Bind_LinkProgram (const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::Isolate* isolate = args.GetIsolate();
    v8::Local<v8::Value> arg0= args[0];
    if(!arg0->IsInt32()) {
        BAD_PARAMETER_TYPE(isolate);
    }



    glLinkProgram((GLuint)NATIVE_UINT(arg0));
    // GL_ERROR_THROW(isolate);
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

void initializeAura(V8Runtime* runtime, Local<v8::Context> context, Local<Object> gl) {
    v8::Isolate* isolate = runtime->isolate;
    isolate_ = isolate;
    context_ = context;

    gl->Set(context, JS_STR("STENCIL_TEST"), JS_INT(0x0B90));
    gl->Set(context, JS_STR("numTest"), JS_NUM(100.0));
    gl->Set(context, JS_STR("floatTest"), JS_FLOAT(100.0));
    gl->Set(context, JS_STR("boolTest"), JS_BOOL(true));
    gl->Set(context, JS_STR("linkProgram"), FUNC(v8Bind_LinkProgram));
}
