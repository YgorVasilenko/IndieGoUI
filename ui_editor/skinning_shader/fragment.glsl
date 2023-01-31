#version 420 core
out vec4 FragColor;

uniform sampler2D skin_image;

in vec4 texCoords;

void main() {
    FragColor = texture(skin_image, texCoords);
};