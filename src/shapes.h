#pragma once

#include <vector>

#include "glm\glm.hpp"

#include "renderer/mesh.h"


namespace shape {
	/* A convenience library for creating shapes such as cubes. */

	void cube(std::vector<glm::vec3>* verts, std::vector<glm::vec3>* normals, std::vector<unsigned int>* index);
	void cubePoints(std::vector<glm::vec3>* verts, std::vector<unsigned int>* index);
	void scale(std::vector<glm::vec3>& verts, glm::vec3& scale);

	CMesh cubeMesh();

}