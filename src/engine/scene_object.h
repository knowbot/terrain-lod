#pragma once
#include <glm/glm.hpp>
class SceneObject {
protected:
	glm::mat4 model = glm::mat4(1.0);

public:
	glm::mat4 get_model() { return model; };
	void set_model(glm::mat4 mat) { model = mat; };
	virtual void draw() = 0;
};