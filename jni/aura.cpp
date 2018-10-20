// #define BAD_PARAMETER_TYPE(x) x->ThrowException(v8::Exception::Error(String::NewFromUtf8(x, "Bad parameter type")))
// bool CHECK_GL_ERRORS = true;

// #define GL_ERROR_THROW(x) \
// if (CHECK_GL_ERRORS) { \
//   if (glGetError() != GL_NO_ERROR) { \
//       x->ThrowException(v8::Exception::Error(String::NewFromUtf8(x, "GL Error")));\
//   } \
// }

// void v8Bind_ActiveTexture (const v8::FunctionCallbackInfo<v8::Value>& args) {
//     v8::Isolate* isolate = args.GetIsolate();
//     v8::Local<v8::Value> arg0= args[0];
//     if(!arg0->IsInt32()) {
//         BAD_PARAMETER_TYPE(isolate);
//     }
//
//     // glActiveTexture((GLenum)arg0->Int32Value());
//     // GL_ERROR_THROW(isolate);
// }

void initializeAura(v8::Isolate* isolate, Persistent<Object>* globalObject, Handle<Context> context) {
    v8::Context::Scope context_scope(context);
    Local<Object> gl = Object::New(isolate);
    context->Global()->Set(v8::String::NewFromUtf8(isolate, "_gl"), gl);

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

    // gl->Set( String::NewFromUtf8(isolate, "activeTexture"), Function::New(isolate, v8Bind_ActiveTexture) );
}
