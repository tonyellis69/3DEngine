#pragma once
#include "renderer/mesh.h"

/** General class defining a model consisting of one or more meshes. */
class CModel {
public:

	TModelMesh* getMesh(const std::string& name);
	bool circleCollision(glm::vec3& segA, glm::vec3& segB);
	bool BBcollision(glm::vec3& segA, glm::vec3& segB);
	float getRadius();

	std::vector<TModelMesh> meshes;

	glm::mat4 tmpMatrix;
	TMeshExtents extents;
};