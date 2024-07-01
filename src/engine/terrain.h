#pragma once
#include <vector>
#include <bitset>
#include <algorithm>
#include "shader.h"
#include "compute_shader.h"
#include "scene_object.h"
#include "camera.h"

struct NoiseSettings {
	glm::vec3 offset;
	float frequency;
	int octaves;
	float amplitude;
	float lacunarity;
	float gain;
	float range;

	NoiseSettings(glm::vec3 offset, float frequency, int octaves, float amplitude, float lacunarity, float gain, float range) 
		: offset(offset), frequency(frequency), octaves(octaves), amplitude(amplitude), lacunarity(lacunarity), gain(gain), range(range) {};
};


class Terrain : public SceneObject {
private:
	const unsigned short int NUM_CHANNELS = 4;
	// buffer objects
	GLuint VAO = 0;
	GLuint VBO = 0;

	// shader programs
	Shader* shader = nullptr;
	ComputeShader* generator = nullptr;

	// terrain data
	unsigned int width, height, resolution;

	// terrain texture
	GLuint data_tex;

	NoiseSettings noise_settings;

	std::vector<GLfloat> vertices;

	void gen_data();
	void gen_vertices();
	void gen_buffers();

public:
	// keep em public cause its easier to manage
	float height_scale = 128.0f, height_shift = 64.0f;
	int min_tess_level = 4;
	int max_tess_level = 64;
	float min_distance = 25;
	float max_distance = 500;

	Terrain(unsigned int width, unsigned int height, unsigned int resolution, Shader* shader, ComputeShader* generator, NoiseSettings noise_settings);
	~Terrain();
	void draw(); // draw full mesh
	void set_uniforms(Camera* camera, glm::mat4 view_projection);
};