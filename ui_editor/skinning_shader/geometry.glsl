#version 420 core
layout (points) in;
layout (triangle_strip, max_vertices = 6) out;


// width and height of sprite
in vec2 sizes[];
in vec4 texture[];

out vec4 fragColor;
out vec2 texCoords;

uniform mat4 projection;

vec4 buf;

void main() { 
    // First triangle
    // top-left
    buf.x = -sizes[0].x / 2;
    buf.y = sizes[0].y / 2;
    buf.z = 0.f; 
    buf.w = 1.f;
    buf = projection * buf;
    gl_Position =  gl_in[0].gl_Position + buf; 
    gl_Position.w = 1.f;
    // tex coord for BOTTOM-left
    texCoords.x = texture[0].x;
    texCoords.y = texture[0].w;
    EmitVertex();

    // bottom-left
    buf.x = -sizes[0].x / 2;
    buf.y = -sizes[0].y / 2;
    buf.z = 0.f; 
    buf.w = 1.f;
    buf = projection * buf;
    gl_Position = gl_in[0].gl_Position + buf; 
    gl_Position.w = 1.f;
    // text coord for TOP-left
    texCoords.x = texture[0].x;
    texCoords.y = texture[0].y;
    EmitVertex();

    // bottom-right
    buf.x = sizes[0].x / 2;
    buf.y = -sizes[0].y / 2;
    buf.z = 0.f; 
    buf.w = 1.f;
    buf = projection * buf;
    gl_Position = gl_in[0].gl_Position + buf; 
    gl_Position.w = 1.f;
    // tex coord for TOP-right
    texCoords.x = texture[0].z;
    texCoords.y = texture[0].y;
    EmitVertex();
    EndPrimitive();

    // Second triangle
    // bottom-right
    buf.x = sizes[0].x / 2;
    buf.y = -sizes[0].y / 2;
    buf.z = 0.f; 
    buf.w = 1.f;
    buf = projection * buf;
    gl_Position = gl_in[0].gl_Position + buf; 
    gl_Position.w = 1.f;
    // tex coord for TOP-right
    texCoords.x = texture[0].z;
    texCoords.y = texture[0].y;
    EmitVertex();

    // top-right
    buf.x = sizes[0].x / 2;
    buf.y = sizes[0].y / 2;
    buf.z = 0.f; 
    buf.w = 1.f;
    buf = projection * buf;
    gl_Position = gl_in[0].gl_Position + buf; 
    gl_Position.w = 1.f;
    // tex coord for BOTTOM-right
    texCoords.x = texture[0].z;
    texCoords.y = texture[0].w;
    EmitVertex();

    // top-left
    buf.x = -sizes[0].x / 2;
    buf.y = sizes[0].y / 2;
    buf.z = 0.f; 
    buf.w = 1.f;
    buf = projection * buf;
    gl_Position = gl_in[0].gl_Position + buf; 
    gl_Position.w = 1.f;
    // coord for BOTTOM-left
    texCoords.x = texture[0].x;
    texCoords.y = texture[0].w;
    EmitVertex();
    EndPrimitive();
}  