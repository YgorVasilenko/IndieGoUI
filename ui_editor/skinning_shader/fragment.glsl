#version 450 core

layout(binding = 1) uniform sampler2D texSampler;

layout(location = 0) out vec4 FragColor;

layout(location = 0) in float displaySkinImage;
layout(location = 1) in vec2 fragTexCoord;

void main() {
    if (displaySkinImage == 1.)
        FragColor = texture(texSampler, fragTexCoord);
    else
        FragColor = vec4(1);
}