#version 420 core
layout (points) in;
layout (line_strip, max_vertices = 11) out;

// width and height of sprite
in vec2 sizes[];
in vec4 color[];

out vec4 fragColor;
uniform mat4 projection;

vec4 buf;

void main() { 
    // top-left
    buf.x = -sizes[0].x / 2;
    // buf.y = sizes[0].y / 2;
    buf.z = 0.1f; 
    buf.w = 1.f;
    if (color[0].w == 0) {
        buf.y = -sizes[0].y / 2;
        buf = projection * buf;
        gl_Position = projection * gl_in[0].gl_Position + buf; 
    } else {
        buf.y = sizes[0].y / 2;
        gl_Position = gl_in[0].gl_Position + buf; 
    }
    gl_Position.w = 1.f;
    fragColor = color[0];
    EmitVertex();

    // bottom-left
    buf.x = -sizes[0].x / 2;
    // buf.y = -sizes[0].y / 2;
    buf.z = 0.1f; 
    buf.w = 1.f;
    if (color[0].w == 0) {
        buf.y = sizes[0].y / 2;
        buf = projection * buf;
        gl_Position = projection * gl_in[0].gl_Position + buf; 
    } else {
        buf.y = -sizes[0].y / 2;
        gl_Position = gl_in[0].gl_Position + buf; 
    }
    gl_Position.w = 1.f;
    fragColor = color[0];
    EmitVertex();

    // bottom-right
    buf.x = sizes[0].x / 2;
    // buf.y = -sizes[0].y / 2;
    buf.z = 0.1f; 
    buf.w = 1.f;
    if (color[0].w == 0) {
        buf.y = sizes[0].y / 2;
        buf = projection * buf;
        gl_Position = projection * gl_in[0].gl_Position + buf; 
    } else {
        buf.y = -sizes[0].y / 2;
        gl_Position = gl_in[0].gl_Position + buf; 
    }
    gl_Position.w = 1.f;
    fragColor = color[0];
    EmitVertex();
    EndPrimitive();

    // top-right
    buf.x = sizes[0].x / 2;
    // buf.y = sizes[0].y / 2;
    buf.z = 0.1f; 
    buf.w = 1.f;
    if (color[0].w == 0) {
        buf.y = -sizes[0].y / 2;
        buf = projection * buf;
        gl_Position = projection * gl_in[0].gl_Position + buf; 
    } else {
        buf.y = sizes[0].y / 2;
        gl_Position = gl_in[0].gl_Position + buf; 
    }
    gl_Position.w = 1.f;
    fragColor = color[0];
    EmitVertex();

    // top-left
    buf.x = -sizes[0].x / 2;
    // buf.y = sizes[0].y / 2;
    buf.z = 0.1f; 
    buf.w = 1.f;
    if (color[0].w == 0) {
        buf.y = -sizes[0].y / 2;
        buf = projection * buf;
        gl_Position = projection * gl_in[0].gl_Position + buf; 
    } else {
        buf.y = sizes[0].y / 2;
        gl_Position = gl_in[0].gl_Position + buf; 
    }
    // gl_Position = gl_in[0].gl_Position + buf; 
    gl_Position.w = 1.f;
    fragColor = color[0];
    EmitVertex();
    EndPrimitive();

    // Right border diplication. Somehow does not work w/o it.
    // bottom-right
    buf.x = sizes[0].x / 2;
    // buf.y = -sizes[0].y / 2;
    buf.z = 0.1f; 
    buf.w = 1.f;
    if (color[0].w == 0) {
        buf.y = sizes[0].y / 2;
        buf = projection * buf;
        gl_Position = projection * gl_in[0].gl_Position + buf; 
    } else {
        buf.y = -sizes[0].y / 2;
        gl_Position = gl_in[0].gl_Position + buf; 
    }
    gl_Position.w = 1.f;
    fragColor = color[0];
    EmitVertex();

    // top-right
    buf.x = sizes[0].x / 2;
    // buf.y = sizes[0].y / 2;
    buf.z = 0.1f; 
    buf.w = 1.f;
    if (color[0].w == 0) {
        buf.y = -sizes[0].y / 2;
        buf = projection * buf;
        gl_Position = projection * gl_in[0].gl_Position + buf; 
    } else {
        buf.y = sizes[0].y / 2;
        gl_Position = gl_in[0].gl_Position + buf; 
    }
    gl_Position.w = 1.f;
    fragColor = color[0];
    EmitVertex();
    EndPrimitive();
}