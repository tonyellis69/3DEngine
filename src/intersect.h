#pragma once

#include <glm/glm.hpp>

/** Some basic geometry utils. */

bool segIntersect(const glm::vec3 & seg1A, const glm::vec3 & seg1B, const glm::vec3 & seg2A, const glm::vec3 & seg2B);

bool segIntersect(const glm::vec3 & seg1A, const glm::vec3 & seg1B, const glm::vec3 & seg2A, const glm::vec3 & seg2B, glm::vec3& intersect);

float signed2DTriArea(const glm::vec3 & a, const glm::vec3 & b, const glm::vec3 & c);

bool inColinearSegment(const glm::vec3 & segA, const glm::vec3 & segB, const glm::vec3 & pt);

float sqrDistPointSegment(const glm::vec3& segA, const glm::vec3& segB, const glm::vec3& pt);

glm::vec3 closestPointSegment(const glm::vec3& segA, const glm::vec3& segB, const glm::vec3& pt);

