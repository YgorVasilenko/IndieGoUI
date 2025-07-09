#version 450 core

layout(binding = 0) uniform UniformBufferObject {
    mat4 proj;
    mat4 transform; // scale and shift
    vec4 color;
} ubo;

vec2 positions[5] = vec2[](
    vec2(-0.5, 0.5),
    vec2(0.5, 0.5),
    vec2(0.5, -0.5),
    vec2(-0.5, -0.5),
    vec2(-0.5, 0.5)
);

layout(location = 0) out vec4 fragColor;

void main() {
    gl_Position =  ubo.transform * ubo.proj * vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColor = ubo.color;
}