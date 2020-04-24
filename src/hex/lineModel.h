#pragma once

#include "renderer/buf.h"

#include "renderer/mesh.h"

/** A class encapsulating the vertex buffer, sub meshes 
	and 3D operations required to draw a line model. */
class CLineModel {
public:
	void setColourR(glm::vec4& colour);
	TModelNode* getNode(const std::string&  name);

	TModelNode model;
	CBuf* buffer;

private:
	void recurseColour(TModelNode& node, glm::vec4& colour);
	TModelNode* recurseName(TModelNode& node, const std::string& name);
};