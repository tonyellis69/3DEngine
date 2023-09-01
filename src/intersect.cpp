#include "intersect.h"
#include "intersect.h"



bool segIntersect(const glm::vec3 & seg1A, const glm::vec3 & seg1B, const glm::vec3 & seg2A, const glm::vec3 & seg2B) {
	float a1 = signed2DTriArea(seg1A, seg1B, seg2B);
	float a2 = signed2DTriArea(seg1A, seg1B, seg2A);

	if (a1 * a2 < 0.0f)
		return true;

	return false;
}

bool segIntersect(const glm::vec3 & seg1A, const glm::vec3 & seg1B, const glm::vec3 & seg2A, const glm::vec3 & seg2B, glm::vec3& intersect) {
	
	//Find the area of the triangles formed by seg1 and each end of seg2
	float a1 = signed2DTriArea(seg1A, seg1B, seg2B);
	float a2 = signed2DTriArea(seg1A, seg1B, seg2A);

	//there's only an intersection if one triangle has normal area and the other 'negative' 
	//(inverted) area.
	//if either a1 or a2 = 0, that seg2 point is colinear with seg1


	if (a1 * a2 < 0.0f) {
		float a3 = signed2DTriArea(seg2A, seg2B, seg1A);
		float a4 = a3 + a2 - a1;

		if (a3 * a4 < 0.0f) {
			float t = a3 / (a3 - a4);
			intersect = seg1A + t * (seg1B - seg1A);
			return true;
		}
	}

	//if seg1 intersects either end of seg2, report this
	float t = 1E-05f; 
	if (abs(a1) < t && inColinearSegment(seg1A, seg1B, seg2B))
		intersect = { 0,1,0 };
	else if (abs(a2) < t && inColinearSegment(seg1A, seg1B, seg2A))
		intersect = { 1,0,0 };

	return false;
}


/** Returns area of triangle. */
float signed2DTriArea(const glm::vec3 & a, const glm::vec3 & b, const glm::vec3 & c) {
	return (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);
}


/** Returns true if pt is within the bounds of the given colinear segment. */
bool inColinearSegment(const glm::vec3 & segA, const glm::vec3 & segB, const glm::vec3 & pt) {
	if (segA.x > segB.x || segB.x > segA.x) {
		if (segA.x > segB.x) 
			return pt.x >= segB.x && pt.x <= segA.x;
		return pt.x >= segA.x && pt.x <= segB.x;
	}

	//NB! for segments with only microscopic differences in x, this will return true
	//without checking if pt lies outside in the y direction!
	//in case of problems, enforce y check as well as x.

	if (segA.y > segB.y) 
		return pt.y >= segB.y && pt.y <= segA.y;
	return pt.y >= segA.y && pt.y <= segB.y;
}

/** Returns squared distance between point and segment. */
float sqrDistPointSegment(const glm::vec3& segA, const glm::vec3& segB, const glm::vec3& pt) {
	glm::vec3 ab = segB - segA;
	glm::vec3 ac = pt - segA;
	glm::vec3 bc = pt - segA;

	float e = glm::dot(ab, ac);
	if (e <= 0.0f)
		return glm::dot(ac, ac);
	float f = glm::dot(ab, ab);
	if (e >= f)
		return glm::dot(bc, bc);

	return glm::dot(ac, ac) - e * e / f;
}

/** Returns closest position on a segment to a point. */
glm::vec3 closestPointSegment(const glm::vec3& segA, const glm::vec3& segB, const glm::vec3& pt) {
	glm::vec3 ab = segB - segA;
	glm::vec3 ac = pt - segA;
	float t = glm::dot(ac, ab) / glm::dot(ab, ab);
	if (t < 0.0f) t = 0.0f;
	if (t > 1.0f) t = 1.0f;

	return segA + t * ab;
}


