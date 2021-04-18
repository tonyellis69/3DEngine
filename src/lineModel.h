#pragma once

//#include "renderer/buf.h"

#include "renderer/mesh.h"

/** A class encapsulating the vertex buffer, sub meshes 
	and 3D operations required to draw a line model. */
class CLineModel {
public:
	void setColourR(glm::vec4& colour);
	TModelNode* getNode(const std::string&  name);
	bool circleCollision(glm::vec3& segA, glm::vec3& segB);
	bool BBcollision(glm::vec3& segA, glm::vec3& segB);

	TModelNode model;
	//CBuf* buffer;
	CBuf2* buffer2;

private:
	void recurseColour(TModelNode& node, glm::vec4& colour);
	TModelNode* recurseName(TModelNode& node, const std::string& name);
};