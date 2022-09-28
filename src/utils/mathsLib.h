#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>

#include <cmath>

/** Any maths thing I use more than once. */

float vec2angle(glm::vec3& v);

float shortestAngle(glm::vec3& v1, glm::vec3& v2);

float shortestAngle(float angle, glm::vec3& v1);

glm::vec3 turnLeft(const glm::vec3& v);

glm::vec3 turnRight(const glm::vec3& v);

bool isAcute(glm::vec3& vecA, glm::vec3& vecB);