#version 330 core
in vec2 textCoords;

out vec4 fragColor;

uniform sampler2D screenTexture;

void main() {
    fragColor = 1.0 - texture(screenTexture, textCoords);
}