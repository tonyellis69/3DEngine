#pragma once

#include "glm\glm.hpp"
#include "bsphere.h"


/** Library of collision routines. */

glm::vec3 closestPointToTriangle(glm::vec3& triA, glm::vec3& triB,  glm::vec3& triC, glm::vec3& p);

/** Returns true if sphere s intersects triangle ABC, false otherwise. 
	The point p on abc closest to the sphere center is also returned. */
int triSphereIntersection(CBsphere& sphere, glm::vec3& triA, glm::vec3& triB, glm::vec3& triC, glm::vec3& p);
