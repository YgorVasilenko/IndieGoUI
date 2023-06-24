// Shader pipeline to display screen-filled quad
// assumes tilemap takes whole drawing area
#version 420 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in vec3 aOrigin;

// TODO : add scale
out vec2 TexCoord;

void main() {
    gl_Position = vec4(aPos, 1.0);
    TexCoord = aTex;
};