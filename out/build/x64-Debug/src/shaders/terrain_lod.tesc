#version 430 core

layout (vertices = 4) out;

uniform mat4 view;

uniform int min_tess_level;
uniform int max_tess_level;
uniform float min_distance;
uniform float max_distance;

in vec2 v_tex_coord[];
out vec2 c_tex_coord[];

// control values for deciding tessellation level

float distance_from_camera(vec4 pos) { return clamp((abs(pos.z) - min_distance) / (max_distance - min_distance), 0.0, 1.0); };

void main() {
	// Pass through the vertex attribute data
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	c_tex_coord[gl_InvocationID] = v_tex_coord[gl_InvocationID];

	// Set tessellation levels
	if (gl_InvocationID == 0) {
		// get eye space coordinates, so we can use z coord to determine distance from camera
		vec4 eye_pos_0 = view * gl_in[0].gl_Position;
		vec4 eye_pos_1 = view * gl_in[1].gl_Position;
		vec4 eye_pos_2 = view * gl_in[2].gl_Position;
		vec4 eye_pos_3 = view * gl_in[3].gl_Position;

		// calc patch distance
		float dist_0 = distance_from_camera(eye_pos_0);
		float dist_1 = distance_from_camera(eye_pos_1);
		float dist_2 = distance_from_camera(eye_pos_2);
		float dist_3 = distance_from_camera(eye_pos_3);

		// interpolate tessellation levels
		gl_TessLevelOuter[0] = mix(max_tess_level, min_tess_level, min(dist_0, dist_2));
		gl_TessLevelOuter[1] = mix(max_tess_level, min_tess_level, min(dist_0, dist_1));
		gl_TessLevelOuter[2] = mix(max_tess_level, min_tess_level, min(dist_1, dist_3));
		gl_TessLevelOuter[3] = mix(max_tess_level, min_tess_level, min(dist_2, dist_3));
		
		gl_TessLevelInner[0] = max(gl_TessLevelOuter[1], gl_TessLevelOuter[3]);
		gl_TessLevelInner[1] = max(gl_TessLevelOuter[0], gl_TessLevelOuter[2]);
	}
}
