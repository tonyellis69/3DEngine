#include "collision.h"

using namespace glm;

/** Returns true if sphere s intersects triangle ABC, false otherwise.
The point p on abc closest to the sphere center is also returned. */
glm::vec3 closestPointToTriangle(glm::vec3 & triA, glm::vec3 & triB, glm::vec3 & triC, glm::vec3 & p) {
	// Check if P in vertex region outside A
	vec3 ab = triB - triA;
	vec3 ac = triC - triA;
	vec3 ap = p - triA;
	float d1 = glm::dot(ab, ap);
	float d2 = glm::dot(ac, ap);
	if (d1 <= 0.0f && d2 <= 0.0f) return triA; // barycentric coordinates (1,0,0)
	
	// Check if P in vertex region outside B
	vec3 bp = p - triB;
	float d3 = glm::dot(ab, bp);
	float d4 = glm::dot(ac, bp);
	if (d3 >= 0.0f && d4 <= d3) return triB; // barycentric coordinates (0,1,0)

	// Check if P in edge region of AB, if so return projection of P onto AB
	float vc = d1*d4 - d3*d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
		float v = d1 / (d1 - d3);
		return triA + v * ab; // barycentric coordinates (1-v,v,0)
	}

	// Check if P in vertex region outside C
	vec3 cp = p - triC;
	float d5 = glm::dot(ab, cp);
	float d6 = glm::dot(ac, cp);
	if (d6 >= 0.0f && d5 <= d6) return triC; // barycentric coordinates (0,0,1)

	// Check if P in edge region of AC, if so return projection of P onto AC
	float vb = d5*d2 - d1*d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
		float w = d2 / (d2 - d6);
		return triA + w * ac; // barycentric coordinates (1-w,0,w)
	}

	// Check if P in edge region of BC, if so return projection of P onto BC
	float va = d3*d6 - d5*d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		return triB + w * (triC - triB); // barycentric coordinates (0,1-w,w)
	}

	// P inside face region. Compute Q through its barycentric coordinates (u,v,w)
	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	return triA + ab * v + ac * w; // = u*a + v*b + w*c, u = va * denom = 1.0f - v - w
}

/** Returns true if the triangle intersects the sphere, with the closest point of intersection in p. */
int triSphereIntersection(CBsphere & sphere, glm::vec3 & triA, glm::vec3 & triB, glm::vec3 & triC, glm::vec3 & p) {
	// Find point P on triangle ABC closest to sphere centre
	p = closestPointToTriangle(sphere.centre, triA, triB, triC);

	// Sphere and triangle intersect if the (squared) distance from sphere
	// centre to point p is less than the (squared) sphere radius
	vec3 v = p - sphere.centre;
	return glm::dot(v, v) <= sphere.radius * sphere.radius;
}

/** Returns true if the line intersects the triangle, with the point of intersection in baryonic coordiantes. */
int triLineIntersection(const glm::vec3& p, const glm::vec3& q, glm::vec3& a, glm::vec3& b, glm::vec3& c,
	float &u, float &v, float &w)  {

	glm::vec3 pq = q - p;
	glm::vec3 pa = a - p;
	glm::vec3 pb = b - p;
	glm::vec3 pc = c - p;

	// Test if pq is inside the edges bc, ca and ab. 
	glm::vec3 m = glm::cross(pq, pc);
	u = glm::dot(pb, m); // ScalarTriple(pq, pc, pb);
	if (u < 0.0f) return 0; 
	v = -glm::dot(pa, m); // ScalarTriple(pq, pa, pc);
	if (v < 0.0f) return 0;
	w = glm::dot( glm::cross(pq, pb), pa);
	if (w < 0.0f) return 0;

	// Compute the barycentric coordinates (u, v, w) determining the
	// intersection point r, r = u*a + v*b + w*c
	float denom = 1.0f / (u + v + w);
	u *= denom;
	v *= denom;
	w *= denom; // w = 1.0f - u - v;
	return 1;
}

/** Returns true if the line segment intersects triangle abc, with the intersection point
	in the baryonic coordinates u,v,w. */
int triSegmentIntersection(glm::vec3 p, glm::vec3 q, glm::vec3 a, glm::vec3 b, glm::vec3 c,
	float &u, float &v, float &w, float &t) {
	
	vec3 ab = b - a;
	vec3 ac = c - a;
	vec3 qp = p - q;

	// Compute triangle normal.
	vec3 n = cross(ab, ac);

	// Compute denominator d. If d <= 0, segment is parallel to or points
	// away from triangle, so exit early
	float d = dot(qp, n);
	if (d <= 0.0f) 
		return 0;

	// Compute intersection t value of pq with plane of triangle. A ray
	// intersects if 0 <= t. Segment intersects if 0 <= t <= 1. Delay
	// dividing by d until intersection has been found to pierce triangle

	glm::vec3 ap = p - a;
	t = dot(ap, n);
	if (t < 0.0f) 
		return 0;
	if (t > d) // For segment; exclude this code line for a ray test
		return 0; 
	
	// Compute barycentric coordinate components and test if within bounds
	vec3 e = cross(qp, ap);
	v = dot(ac, e);
	if (v < 0.0f || v > d) 
		return 0;
	w = -dot(ab, e);
	if (w < 0.0f || v + w > d) 
		return 0;

	// Segment/ray intersects triangle. Perform delayed division and
	// compute the last barycentric coordinate component
	float ood = 1.0f / d;
	t *= ood;
	v *= ood;
	w *= ood;
	u = 1.0f - v - w;
	return 1;
}
