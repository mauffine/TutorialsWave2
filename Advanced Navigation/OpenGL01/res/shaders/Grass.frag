#version 410

in vec2 vTexCoord;

uniform sampler2D diffuse;

out vec4 fragColour;

void main() {
fragColour = texture(diffuse, vTexCoord);
}