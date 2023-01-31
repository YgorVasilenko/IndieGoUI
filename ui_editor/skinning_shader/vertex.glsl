#version 420 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 size;
layout (location = 2) in vec4 tex;

out vec2 sizes;
out vec4 texture;

uniform float scale;

void main() {
    gl_Position = vec4(position, 0.f, 0.0);
    sizes = size * scale;
    texture = tex;
}