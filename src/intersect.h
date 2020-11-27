#pragma once

#include <glm/glm.hpp>

/** Some basic geometry utils. */

bool segIntersect(glm::vec3& seg1A, glm::vec3& seg1B, glm::vec3& seg2A, glm::vec3& seg2B);

bool segIntersect(glm::vec3& seg1A, glm::vec3& seg1B, glm::vec3& seg2A, glm::vec3& seg2B, glm::vec3& intersect);

float signed2DTriArea(glm::vec3& a, glm::vec3& b, glm::vec3& c);