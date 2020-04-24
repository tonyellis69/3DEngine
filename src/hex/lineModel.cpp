#include "lineModel.h"

/**	Set colour for all meshes. */
void CLineModel::setColourR(glm::vec4& colour) {
	recurseColour(model, colour);
}

/** Return a pointer to the node named. */
TModelNode* CLineModel::getNode(const std::string& name) {
	return recurseName(model, name);
}



/** Recurse through meshes, setting colour. */
void CLineModel::recurseColour(TModelNode& node, glm::vec4& colour) {
	for (auto& mesh : node.meshes)
		mesh.colour = colour;

	for (auto& subNode : node.subNodes)
		recurseColour(subNode, colour);
}

TModelNode* CLineModel::recurseName(TModelNode& node, const std::string& name) {
	if (node.name == name)
		return &node;

	for (auto& subNode : node.subNodes) {
		TModelNode* found = recurseName(subNode, name);
		if (found)
			return found;
	}

	return NULL;
}


