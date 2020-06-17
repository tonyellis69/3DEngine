#pragma once

#include "glm/glm.hpp"

using glm::vec3;

/** Returns true if the line segment intersects triangle abc, with the intersection point
	in the baryonic coordinates u,v,w. */
bool triSegmentIntersection(glm::vec3 p, glm::vec3 q, glm::vec3 a, glm::vec3 b, glm::vec3 c,
	float& u, float& v, float& w, float& t) {

	vec3 ab = b - a;
	vec3 ac = c - a;
	vec3 qp = p - q;

	// Compute triangle normal.
	vec3 n = cross(ab, ac);

	// Compute denominator d. If d <= 0, segment is parallel to or points
	// away from triangle, so exit early
	float d = dot(qp, n);
	if (d <= 0.0f)
		return false;

	// Compute intersection t value of pq with plane of triangle. A ray
	// intersects if 0 <= t. Segment intersects if 0 <= t <= 1. Delay
	// dividing by d until intersection has been found to pierce triangle

	glm::vec3 ap = p - a;
	t = dot(ap, n);
	if (t < 0.0f)
		return false;
	if (t > d) // For segment; exclude this code line for a ray test
		return false;

	// Compute barycentric coordinate components and test if within bounds
	vec3 e = cross(qp, ap);
	v = dot(ac, e);
	if (v < 0.0f || v > d)
		return false;
	w = -dot(ab, e);
	if (w < 0.0f || v + w > d)
		return false;

	// Segment/ray intersects triangle. Perform delayed division and
	// compute the last barycentric coordinate component
	float ood = 1.0f / d;
	t *= ood;
	v *= ood;
	w *= ood;
	u = 1.0f - v - w;
	return true;
}