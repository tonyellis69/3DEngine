#pragma once

#include <vector>

#include "glm\glm.hpp"

#include "renderer/mesh.h"


namespace shape {
	/* A convenience library for creating shapes such as cubes. */

	void cube(std::vector<glm::vec3>* verts, std::vector<glm::vec3>* normals, std::vector<unsigned int>* index);
	void cube(std::vector<glm::vec3>* verts, std::vector<glm::vec3>* normals, std::vector<glm::vec2>* texCoords, std::vector<unsigned int>* index, float topScale);
	void cubePoints(std::vector<glm::vec3>* verts, std::vector<unsigned int>* index);

	void cylinder(CMesh& mesh, int steps);

	void scale(std::vector<glm::vec3>& verts, glm::vec3& scale);

	CMesh cubeMesh();
	CMesh fustrumMesh(float gradient);
	CMesh cylinderMesh(float radius, float height, float steps);
}