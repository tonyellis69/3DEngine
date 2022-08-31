#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>

/** Any maths thing I use more than once. */

float shortestAngle(glm::vec3& v1, glm::vec3& v2) {
	return glm::orientedAngle(glm::normalize(v1), glm::normalize(v2), glm::vec3(0, 0, 1));
}

glm::vec3 turnLeft(const glm::vec3& v) {
	return glm::vec3(-v.y, v.x, v.z);
}

glm::vec3 turnRight(const glm::vec3& v) {
	return glm::vec3(v.y, -v.x, v.z);
}

bool isAcute(glm::vec3& vecA, glm::vec3& vecB) {
	return glm::dot(vecA, vecB) > 0;
}