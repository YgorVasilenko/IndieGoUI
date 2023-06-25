#ifdef __APPLE__
  #define NK_SHADER_VERSION "#version 150\n"
#else
  #define NK_SHADER_VERSION "#version 420\n"
#endif


struct shader_codes {
    char* vertex_shader =
        NK_SHADER_VERSION
        "uniform mat4 ProjMtx;\n"
        "in vec2 Position;\n"
        "in vec2 TexCoord;\n"
        "in vec4 Color;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main() {\n"
        "   Frag_UV = TexCoord;\n"
        "   Frag_Color = Color;\n"
        "   gl_Position = ProjMtx * vec4(Position.xy, 0, 1);\n"
        "}\n";

    char* fragment_shader =
        NK_SHADER_VERSION
        "precision mediump float;\n"
        "uniform sampler2D Texture;\n"
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "layout (location = 0) out vec4 Out_Color;\n"
        "void main(){\n"
        "   Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
        "}\n";

    char* screen_vertex_shader = 
        NK_SHADER_VERSION
        "layout (location = 0) in vec2 position;\n"
        "layout (location = 1) in vec2 size;\n"
        "layout (location = 2) in vec4 tex;\n"
        "out vec2 sizes;\n"
        "out vec4 texture;\n"
        "uniform float scale;\n"
        "uniform mat4 projection;\n"
        "void main() {\n"
        "    gl_Position = projection * vec4(position, 0.f, 1.0);\n"
        "    sizes = size * scale;\n"
        "    texture = tex;\n"
        "}\n";
    
    char* screen_geometry_shader = 
        NK_SHADER_VERSION
        "layout (points) in;\n"
        "layout (triangle_strip, max_vertices = 6) out;\n"
        "in vec2 sizes[];\n"
        "in vec4 texture[];\n"
        "out vec4 fragColor;\n"
        "out vec2 texCoords;\n"
        "uniform mat4 projection;\n"
        "vec4 buf;\n"
        "void main() { \n"
        "    buf.x = -sizes[0].x / 2;\n"
        "    buf.y = sizes[0].y / 2;\n"
        "    buf.z = 0.f; \n"
        "    buf.w = 1.f;\n"
        "    buf = projection * buf;\n"
        "    gl_Position =  gl_in[0].gl_Position + buf; \n"
        "    gl_Position.w = 1.f;\n"
        "    texCoords.x = -texture[0].x;\n"
        "    texCoords.y = texture[0].w;\n"
        "    EmitVertex();\n"
        "    buf.x = -sizes[0].x / 2;\n"
        "    buf.y = -sizes[0].y / 2;\n"
        "    buf.z = 0.f; \n"
        "    buf.w = 1.f;\n"
        "    buf = projection * buf;\n"
        "    gl_Position = gl_in[0].gl_Position + buf; \n"
        "    gl_Position.w = 1.f;\n"
        "    texCoords.x = -texture[0].x;\n"
        "    texCoords.y = texture[0].y;\n"
        "    EmitVertex();\n"
        "    buf.x = sizes[0].x / 2;\n"
        "    buf.y = -sizes[0].y / 2;\n"
        "    buf.z = 0.f; \n"
        "    buf.w = 1.f;\n"
        "    buf = projection * buf;\n"
        "    gl_Position = gl_in[0].gl_Position + buf; \n"
        "    gl_Position.w = 1.f;\n"
        "    texCoords.x = texture[0].z;\n"
        "    texCoords.y = texture[0].y;\n"
        "    EmitVertex();\n"
        "    EndPrimitive();\n"
        "    buf.x = sizes[0].x / 2;\n"
        "    buf.y = -sizes[0].y / 2;\n"
        "    buf.z = 0.f; \n"
        "    buf.w = 1.f;\n"
        "    buf = projection * buf;\n"
        "    gl_Position = gl_in[0].gl_Position + buf; \n"
        "    gl_Position.w = 1.f;\n"
        "    texCoords.x = texture[0].z;\n"
        "    texCoords.y = texture[0].y;\n"
        "    EmitVertex();\n"
        "    buf.x = sizes[0].x / 2;\n"
        "    buf.y = sizes[0].y / 2;\n"
        "    buf.z = 0.f; \n"
        "    buf.w = 1.f;\n"
        "    buf = projection * buf;\n"
        "    gl_Position = gl_in[0].gl_Position + buf; \n"
        "    gl_Position.w = 1.f;\n"
        "    texCoords.x = texture[0].z;\n"
        "    texCoords.y = texture[0].w;\n"
        "    EmitVertex();\n"
        "    buf.x = -sizes[0].x / 2;\n"
        "    buf.y = sizes[0].y / 2;\n"
        "    buf.z = 0.f; \n"
        "    buf.w = 1.f;\n"
        "    buf = projection * buf;\n"
        "    gl_Position = gl_in[0].gl_Position + buf; \n"
        "    gl_Position.w = 1.f;\n"
        "    texCoords.x = -texture[0].x;\n"
        "    texCoords.y = texture[0].w;\n"
        "    EmitVertex();\n"
        "    EndPrimitive();\n"
        "}\n";

    char* screen_fragment_shader = 
        NK_SHADER_VERSION
        "out vec4 FragColor;\n"
        "uniform sampler2D prerender_ui;\n"
        "in vec2 texCoords;\n"
        "void main() {\n"
        // "    FragColor = texture(prerender_ui, texCoords);\n"
        "    FragColor = vec4(1, 1, 1, 1);\n"
        "};";
};

struct ScreenRect {
    // position and size
    float x, y, width, height;

    // texture
    float l = 1.f, 
        r = 1.f, 
        t = 1.f, 
        b = 1.f;
};