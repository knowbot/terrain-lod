#version 430 core
uniform float height_scale;
uniform float height_shift;

in float height;
in float moist;
in float other;

out vec4 FragColor;

vec4 pick_color() {
	if (height > 0.7) {
		if(moist > 0.66) {
			return vec4(1.0, 1.0, 1.0, 1.0); // snow
		}
		if (moist > 0.33) {
			return vec4(0.725, 0.705, 0.639, 1.0); // tundra
		}
		return vec4(0.435, 0.384, 0.380, 1.0); // rocks
	}
	if (height > 0.4) {
		if(moist > 0.6) {
			return vec4(0.231, 0.403, 0.294, 1.0); // forest
		}
		return vec4(0.329, 0.670, 0.454, 1.0); // shrubs
	}
	if (height > 0.2) {
		if(moist > 0.66) {
			return vec4(0.466, 0.772, 0.486, 1.0); // grass
		}
		if (moist > 0.33) {
			return vec4(0.811, 0.847, 0.674, 1.0); // savannah
		}
		return vec4(0.741, 0.650, 0.498, 1.0); // scorched
	}
	if (height > 0.12) {
		return vec4(0.980, 0.929, 0.803, 1.0); // beach
	}
	if (height > 0.08) {
		return vec4(0.107, 0.343, 0.641, 0.5); // light water
	}
	return vec4(0.007, 0.243, 0.541, 0.5); // default - water
}

void main()
{
	vec4 color = pick_color();
    FragColor = color - vec4(vec3(other / 4.0), 0.0);
//	FragColor = vec4(vec3(moist), 1.0);
}