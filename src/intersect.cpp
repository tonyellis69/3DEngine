#include "intersect.h"



bool segIntersect(glm::vec3& seg1A, glm::vec3& seg1B, glm::vec3& seg2A, glm::vec3& seg2B) {
	float a1 = signed2DTriArea(seg1A, seg1B, seg2B);
	float a2 = signed2DTriArea(seg1A, seg1B, seg2A);

	if (a1 * a2 < 0.0f)
		return true;

	return false;
}

bool segIntersect(glm::vec3& seg1A, glm::vec3& seg1B, glm::vec3& seg2A, glm::vec3& seg2B, glm::vec3& intersect) {
	
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
	float t = 1E-03f; 
	if (abs(a1) < t && inColinearSegment(seg1A, seg1B, seg2B))
		intersect = { 0,1,0 };
	else if (abs(a2) < t && inColinearSegment(seg1A, seg1B, seg2A))
		intersect = { 1,0,0 };

	return false;
}


/** Returns area of triangle. */
float signed2DTriArea(glm::vec3& a, glm::vec3& b, glm::vec3& c) {
	return (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);
}


/** Returns true if pt is within the bounds of the given colinear segment. */
bool inColinearSegment(glm::vec3& segA, glm::vec3& segB, glm::vec3& pt) {
	if (segA.x > segB.x || segB.x > segA.x) {
		if (segA.x > segB.x) 
			return pt.x >= segB.x && pt.x <= segA.x;
		return pt.x >= segA.x && pt.x <= segB.x;
	}

	if (segA.y > segB.y) 
		return pt.y >= segB.y && pt.y <= segA.y;
	return pt.y >= segA.y && pt.y <= segB.y;
}
