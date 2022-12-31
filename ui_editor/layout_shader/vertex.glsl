#version 420 core
layout (location = 0) in vec2 position;
layout (location = 1) in vec2 size;
layout (location = 2) in vec4 color_;

out vec2 sizes;
out vec4 color;

void main() {
    gl_Position = vec4(position, 0.f, 0.0);
    sizes = size;
    color = color_;
}