#version 450 core

layout(binding = 0) uniform sampler2D layoutSampler;
layout(binding = 1) uniform sampler2D skinningSampler;
layout(binding = 2) uniform sampler2D overlaySampler;

layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec2 fragTexCoord;

void main() {
    vec4 layout_color = texture(layoutSampler, fragTexCoord);
    vec4 skinning_color = texture(skinningSampler, fragTexCoord);
    vec4 overlay_color = texture(overlaySampler, fragTexCoord);

    if (layout_color.a == 0) {
        if (overlay_color.a == 0)
            FragColor = skinning_color;
        else
            FragColor = overlay_color;
    } else {
        FragColor = layout_color;
    }
}