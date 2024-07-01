#version 430 core

layout (quads, fractional_odd_spacing, ccw) in;

uniform mat4 model;
uniform float height_scale;
uniform float height_shift;
uniform sampler2D terrain_data;

in vec2 c_tex_coord[];
out float height;
out float moist;
out float other;

vec2 lerp(vec2 a, vec2 b, float t) { return a + (b - a) * t; }
vec4 lerp(vec4 a, vec4 b, float t) { return a + (b - a) * t; }

void main() {
	// --- HEIGHTMAP TEXTURE LOOKUP --- 
	// abstract patch coords
	float u = gl_TessCoord.x, v = gl_TessCoord.y;

	// control point texture coords
	vec2 c_tex_0 = c_tex_coord[0];
	vec2 c_tex_1 = c_tex_coord[1];
	vec2 c_tex_2 = c_tex_coord[2];
	vec2 c_tex_3 = c_tex_coord[3];

	// interpolate
	vec2 i_tex_0 = lerp(c_tex_0, c_tex_1, u);
	vec2 i_tex_1 = lerp(c_tex_2, c_tex_3, u);
	vec2 e_tex_coord = lerp(i_tex_0, i_tex_1, v);

	// compute height at evaluated coord
	height = texture(terrain_data, e_tex_coord).x;
	moist = texture(terrain_data, e_tex_coord).y;
	other = texture(terrain_data, e_tex_coord).z;

	// --- VERTEX POSITION CALCULATION ---
	// control point coords
	vec4 c_pos_0 = gl_in[0].gl_Position;
	vec4 c_pos_1 = gl_in[1].gl_Position;
	vec4 c_pos_2 = gl_in[2].gl_Position;
	vec4 c_pos_3 = gl_in[3].gl_Position;
	
	vec4 i_pos_0 = lerp(c_pos_0, c_pos_1, u);
	vec4 i_pos_1 = lerp(c_pos_2, c_pos_3, u);
	vec4 e_pos = lerp(i_pos_0, i_pos_1, v);

	// raise in y direction since we start from a plain grid
	e_pos.y += height * height_scale - height_shift;

	// output in view space
	gl_Position = model * e_pos;
}