#include "collision.h"

using namespace glm;

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

int triSphereIntersection(CBsphere & sphere, glm::vec3 & triA, glm::vec3 & triB, glm::vec3 & triC, glm::vec3 & p) {
	// Find point P on triangle ABC closest to sphere centre
	p = closestPointToTriangle(sphere.centre, triA, triB, triC);

	// Sphere and triangle intersect if the (squared) distance from sphere
	// centre to point p is less than the (squared) sphere radius
	vec3 v = p - sphere.centre;
	return glm::dot(v, v) <= sphere.radius * sphere.radius;
}
