#version 450 core
layout(location = 0) out vec4 FragColor;

layout(location = 0) in vec4 fragColor;

// "Discard" squares:
layout(binding = 1) uniform DiscardBufferObject { 
    // - main
    vec4 main_square;
    // - element properties
    vec4 elements_square;
    // - skinning
    vec4 skinning_square;
    // - fonts
    vec4 fonts_square;
} dbo;

void main() {
    // TODO : get coords from editor UI widgets, discard if fall to them
    if (
        gl_FragCoord.x > dbo.main_square.x && gl_FragCoord.x < dbo.main_square.z
        && gl_FragCoord.y > dbo.main_square.y && gl_FragCoord.y < dbo.main_square.w
    )
        discard;

    if (
        gl_FragCoord.x > dbo.elements_square.x && gl_FragCoord.x < dbo.elements_square.z
        && gl_FragCoord.y > dbo.elements_square.y && gl_FragCoord.y < dbo.elements_square.w
    )
        discard;

    if (
        gl_FragCoord.x > dbo.skinning_square.x && gl_FragCoord.x < dbo.skinning_square.z
        && gl_FragCoord.y > dbo.skinning_square.y && gl_FragCoord.y < dbo.skinning_square.w
    )
        discard;
    
    if (
        gl_FragCoord.x > dbo.fonts_square.x && gl_FragCoord.x < dbo.fonts_square.z
        && gl_FragCoord.y > dbo.fonts_square.y && gl_FragCoord.y < dbo.fonts_square.w
    )
        discard;

    FragColor = fragColor;
    FragColor.a = 1.f;
}