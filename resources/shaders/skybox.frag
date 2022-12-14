#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
    vec4 fogColor = vec4(0.8, 0.8, 0.8, 1);
    FragColor = texture(skybox, TexCoords) * (1 - 0.7) + 0.7 * fogColor;
}
