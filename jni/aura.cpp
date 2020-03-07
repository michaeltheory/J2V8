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
