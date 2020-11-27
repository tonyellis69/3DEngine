#include "intersect.h"



bool segIntersect(glm::vec3& seg1A, glm::vec3& seg1B, glm::vec3& seg2A, glm::vec3& seg2B) {
	float a1 = signed2DTriArea(seg1A, seg1B, seg2B);
	float a2 = signed2DTriArea(seg1A, seg1B, seg2A);

	if (a1 * a2 < 0.0f)
		return true;

	return false;
}

bool segIntersect(glm::vec3& seg1A, glm::vec3& seg1B, glm::vec3& seg2A, glm::vec3& seg2B, glm::vec3& intersect) {
	float a1 = signed2DTriArea(seg1A, seg1B, seg2B);
	float a2 = signed2DTriArea(seg1A, seg1B, seg2A);

	if (a1 * a2 < 0.0f) {
		float a3 = signed2DTriArea(seg2A, seg2B, seg1A);
		float a4 = a3 + a2 - a1;

		if (a3 * a4 < 0.0f) {
			float t = a3 / (a3 - a4);
			intersect = seg1A + t * (seg1B - seg1A);
			return true;
		}
	}

	return false;
}



float signed2DTriArea(glm::vec3& a, glm::vec3& b, glm::vec3& c) {
	return (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);
}
