#define BAD_PARAMETER_TYPE(x) x->ThrowException(v8::Exception::Error(String::NewFromUtf8(x, "Bad parameter type")))
bool CHECK_GL_ERRORS = true;

#define GL_ERROR_THROW(x) \
if (CHECK_GL_ERRORS) { \
  if (glGetError() != GL_NO_ERROR) { \
      x->ThrowException(v8::Exception::Error(String::NewFromUtf8(x, "GL Error")));\
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
    gl->Set( String::NewFromUtf8(isolate, "COLOR_ATTACHMENT0"), Integer::New(isolate, 0x8CE0) );
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

}
