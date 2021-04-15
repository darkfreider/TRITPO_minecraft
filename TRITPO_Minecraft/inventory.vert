#version 330 core
layout (location = 0) in vec3 pos;

out vec2 texCoords;

uniform mat4 model;

void main() {
	texCoords = (pos.xy + vec2(1.0f, 1.0f)) / 2;
	gl_Position = model * vec4(pos, 1.0f);
}  