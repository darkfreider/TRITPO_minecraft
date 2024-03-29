#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;
uniform float ambient_factor;

void main() {
	vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
	vec3 light = lightColor * ambient_factor;

    FragColor = texture(skybox, TexCoords) * vec4(light, 1.0f);
}