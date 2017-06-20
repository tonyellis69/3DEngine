#pragma once

#include "glm\glm.hpp"
#include "bsphere.h"


/** Library of collision routines. */

glm::vec3 closestPointToTriangle(glm::vec3& triA, glm::vec3& triB,  glm::vec3& triC, glm::vec3& p);


int triSphereIntersection(CBsphere& sphere, glm::vec3& triA, glm::vec3& triB, glm::vec3& triC, glm::vec3& p);

int triLineIntersection(glm::vec3& p, glm::vec3& q, glm::vec3& a, glm::vec3& b, glm::vec3& c,
	float &u, float &v, float &w);