#version 430 core

layout (location = 0) in vec3 v_pos;
layout (location = 1) in vec2 v_tex;
out vec2 v_tex_coord;

uniform mat4 model;

void main()
{
    gl_Position = vec4(v_pos, 1.0);
	v_tex_coord = v_tex;
}