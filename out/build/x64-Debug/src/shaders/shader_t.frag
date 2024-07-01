#version 430 core
out vec4 FragColor;
uniform sampler2D tex;
in vec2 UVs;
void main()
{
	FragColor = texture(tex, UVs);
}