#pragma once

#include <vector>

#include "glm\glm.hpp"

#include "renderer/mesh.h"


namespace shape {
	/* A convenience library for creating shapes such as cubes. */

	void cube(std::vector<glm::vec3>* verts, std::vector<glm::vec3>* normals, std::vector<unsigned int>* index);
	void cube(std::vector<glm::vec3>* verts, std::vector<glm::vec3>* normals, std::vector<glm::vec2>* texCoords, std::vector<unsigned int>* index, float topScale);
	void cubePoints(std::vector<glm::vec3>* verts, std::vector<unsigned int>* index);

	void cylinder(CMesh<glm::vec3>& mesh, int steps);

	void scale(std::vector<glm::vec3>& verts, glm::vec3& scale);

	CMesh<glm::vec3> cubeMesh();
	CMesh<glm::vec3> fustrumMesh(float gradient);
	CMesh<glm::vec3> cylinderMesh(float radius, float height, float steps);
}