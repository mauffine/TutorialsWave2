#version 410

layout(location = 0) in vec3 Position;
layout(location = 1) in vec2 TexCoord;

out vec2 vTexCoord;
out vec3 position;

uniform vec3 myPosition;

void main() {
	gl_Position = projectionView * (Position + myPosition);
	vTexCoord = TexCoord;
}