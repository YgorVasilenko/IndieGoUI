#version 420 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D color_sampler;

void main() {
    FragColor = texture(color_sampler, TexCoord);
};