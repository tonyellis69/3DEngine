#pragma once

#include "renderer/mesh.h"

/** A high-level class representing a line-based model.
	Stores the meshes, matrices and any sub meshes,
	and a pointer to a buffer for the verts. */
class CLineModel {
public:
	void setColourR(glm::vec4& colour);
	TModelData* getNode(const std::string&  name);
	bool circleCollision(glm::vec3& segA, glm::vec3& segB);
	bool BBcollision(glm::vec3& segA, glm::vec3& segB);
	float getRadius();

	TModelData model;
	CBuf2* buffer2;
	glm::vec4 colour = { 0.5f,0.5f,0.5f,1.0f }; //temporary solution, may become an array etc

private:
	void recurseColour(TModelData& node, glm::vec4& colour);
	TModelData* recurseName(TModelData& node, const std::string& name);
};			