#pragma once
#pragma once
#include <glm/glm.hpp>
#include <algorithm>

class Plane {
public:
private:
	glm::vec3 point;
	glm::vec3 normal;

public:
	Plane() : point(glm::vec3(0)), normal(glm::vec3(0, 1, 0)) {};
	Plane(const glm::vec3& point, const glm::vec3& normal) : point(point), normal(normal) {}


	void set(const glm::vec3& point, const glm::vec3& normal) {
		this->point = point;
		this->normal = normal;
	}

	glm::vec3 get_point() { return point; }
	glm::vec3 get_normal() { return normal;  }

	float distance(const glm::vec3& p) {
		return glm::dot(normal, p - point);
	}
};