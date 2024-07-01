#include "terrain.h"
#include <iostream>

Terrain::Terrain(unsigned int width, unsigned int height, unsigned int resolution, Shader* shader, ComputeShader* generator, NoiseSettings noise_settings)
	: width(width) ,height(height), resolution(resolution), shader(shader), generator(generator), noise_settings(noise_settings){
	gen_data();
	gen_vertices();
	std::cout << "Loaded vertices: " << vertices.size() / 3 << " for a total of " << vertices.size() * sizeof(float) * 3 << " bytes." << std::endl;

	// LEGACY: no tessellation
	//gen_indices();
	//std::cout << "Loaded indices: " << indices.size() << std::endl;
	gen_buffers();
}

Terrain::~Terrain()
{
	std::cout << "Deleting terrain" << std::endl;
	shader = nullptr;
	generator = nullptr;
	glDeleteTextures(1, &data_tex);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO);
}

void Terrain::draw() {
	shader->use();
	glBindVertexArray(VAO);	
	glDrawArrays(GL_PATCHES, 0, resolution * resolution * 4);
	// LEGACY: no tessellation
	//for (unsigned int strip = 0; strip < NUM_STRIPS; strip++)
	//{
	//	glDrawElements(
	//		GL_TRIANGLE_STRIP,   // primitive type
	//		NUM_TRIS_PER_STRIP, // number of indices to render
	//		GL_UNSIGNED_INT,     // index data type
	//		(void*)(sizeof(unsigned int) * NUM_TRIS_PER_STRIP * strip) // offset to starting index
	//	); 
	//}
}

void Terrain::set_uniforms(Camera* camera, glm::mat4 view_projection)
{
	shader->use();
	shader->setMat4("model", view_projection);
	shader->setMat4("view", camera->get_view_matrix());
	shader->setFloat("height_scale", height_scale);
	shader->setFloat("height_shift", height_shift);
	shader->setInt("min_tess_level", min_tess_level);
	shader->setInt("max_tess_level", max_tess_level);
	shader->setFloat("min_distance", min_distance);
	shader->setFloat("max_distance", max_distance);
	if (data_tex)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTextureUnit(0, data_tex);
		shader->setInt("terrain_data", 0);
	}
	else
		std::cout << "Failed to load data." << std::endl;
}

void Terrain::gen_data() 
{
	glCreateTextures(GL_TEXTURE_2D, 1, &data_tex);
	glTextureParameteri(data_tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(data_tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTextureParameteri(data_tex, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(data_tex, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureStorage2D(data_tex, 1, GL_RGBA32F, width, height);
	glBindImageTexture(0, data_tex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);


	// set noise params
	generator->use();
	generator->setVec3("offset", noise_settings.offset);
	generator->setFloat("frequency", noise_settings.frequency);
	generator->setInt("octaves", noise_settings.octaves);
	generator->setFloat("amplitude", noise_settings.amplitude);
	generator->setFloat("lacunarity", noise_settings.lacunarity);
	generator->setFloat("gain", noise_settings.gain);
	generator->setFloat("range", noise_settings.range);
	// dispatch shader
	glDispatchCompute((GLuint)width, (GLuint)height, 1);
	// ensure shader is done writing
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void Terrain::gen_vertices() 
{
	// LEGACY: no tessellation
	//for (unsigned int i = 0; i < height; i++)
	//	for (unsigned int j = 0; j < width; j++) {
	//		vertices.push_back(width * i / (float)width - (float)width / 2.0f);   // vx
	//		vertices.push_back(data[(j + width * i) * channels] * height_scale - height_shift);   // vy
	//		vertices.push_back(height * i / (float)height - (float)height / 2.0f);   // vz
	//		//uvs.push_back(x / vert_dim);
	//		//uvs.push_back(z / vert_dim);
	//	}
	
	// Create a grid of RxR patches (R = resolution), each with 4 control points
	for (unsigned int i = 0; i < resolution; i++) {
		for (unsigned int j = 0; j < resolution; j++) {
			// 1st CONTROL POINT
			// vertex coords
			vertices.emplace_back(width * i / (float)resolution - (float)width / 2.0f);
			vertices.emplace_back(0.0f);
			vertices.emplace_back(height * j / (float)resolution - (float)height / 2.0f);

			// uv coords
			vertices.emplace_back(i / (float)resolution);
			vertices.emplace_back(j / (float)resolution);

			// 2nd CONTROL POINT
			// vertex coords
			vertices.emplace_back(width * (i + 1) / (float)resolution - (float)width / 2.0f);
			vertices.emplace_back(0.0f);
			vertices.emplace_back(height * j / (float)resolution - (float)height / 2.0f);

			// uv coords
			vertices.emplace_back((i + 1) / (float)resolution);
			vertices.emplace_back(j / (float)resolution);

			// 3rd CONTROL POINT
			// vertex coords
			vertices.emplace_back(width * i / (float)resolution - (float)width / 2.0f);
			vertices.emplace_back(0.0f);
			vertices.emplace_back(height * (j + 1) / (float)resolution - (float)height / 2.0f);

			// uv coords
			vertices.emplace_back(i / (float)resolution);
			vertices.emplace_back((j + 1) / (float)resolution);

			// 4th CONTROL POINT
			// vertex coords
			vertices.emplace_back(width * (i + 1) / (float)resolution - (float)width / 2.0f);
			vertices.emplace_back(0.0f);
			vertices.emplace_back(height * (j + 1) / (float)resolution - (float)height / 2.0f);

			// uv coords
			vertices.emplace_back((i + 1) / (float)resolution);
			vertices.emplace_back((j + 1) / (float)resolution);
		}
	}
}


// LEGACY: no tessellation

//void Terrain::gen_indices() {
//	for (unsigned int i = 0; i < height - 1; i += resolution) {
//		for (unsigned int j = 0; j < width; j += resolution) {
//			for (unsigned int k = 0; k < 2; k++) {
//				indices.push_back(j + (i + k * resolution) * width);
//			}
//		}
//	}
//}

void Terrain::gen_buffers() {
	// generate and bind VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// generate and bind VBO and fill it with vertex data
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

	// generate and bind EBO and fill it with index data
	//glGenBuffers(1, &EBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

	int pos_location = glGetAttribLocation(shader->ID, "v_pos");
	glVertexAttribPointer(pos_location, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(pos_location);

	int tex_location = glGetAttribLocation(shader->ID, "v_tex");
	glVertexAttribPointer(tex_location, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(tex_location);

	glPatchParameteri(GL_PATCH_VERTICES, 4);

	glBindVertexArray(0);

}
