#include "lineModel.h"

#include "intersect.h"

/**	Set colour for all meshes. */
void CLineModel::setColourR(glm::vec4& colour) {
	recurseColour(model, colour);
}

/** Return a pointer to the node named. */
TModelNode* CLineModel::getNode(const std::string& name) {
	return recurseName(model, name);
}


/** Return true if the given segment intersects the circle 
	described by the outermost vertex. */
bool CLineModel::circleCollision(glm::vec3& segA, glm::vec3& segB) {
	float radius = glm::length(model.extents.furthestVert) ;
	glm::vec3 circleOrig = model.matrix[3];
	glm::vec3 localA = segA - circleOrig;
	glm::vec3 segVec = (segB - segA);
	glm::vec3 segVecN = glm::normalize(segVec);

	//circle-segment check:
	float b = glm::dot(localA, segVecN);
	float c = glm::dot(localA, localA) - radius * radius;

	if (c > 0 && b > 0)
		return false;

	float d = b * b - c;
	if (d < 0)
		return false;

	float t = -b - sqrt(d);
	if (t > glm::length(segVec))
		return false;

	return true;
}

/** Return true if the segment intersects our bounding box. */
bool CLineModel::BBcollision(glm::vec3& segA, glm::vec3& segB) {
	glm::vec3 intersection;
	//for each line in bounding box
	//test intersection
	glm::vec3 cornerA(model.extents.BBmin.x, model.extents.BBmax.y, 0);
	glm::vec3 cornerB = model.extents.BBmax;
	glm::vec3 cornerC(model.extents.BBmax.x, model.extents.BBmin.y, 0);
	glm::vec3 cornerD = model.extents.BBmin;

	cornerA = model.matrix * glm::vec4(cornerA,1);
	cornerB = model.matrix * glm::vec4(cornerB, 1);
	if (segIntersect(segA, segB, cornerA, cornerB, intersection))
		return true;

	cornerC = model.matrix * glm::vec4(cornerC, 1);
	if (segIntersect(segA, segB, cornerB, cornerC, intersection))
		return true;

	cornerD = model.matrix * glm::vec4(cornerD, 1);
	if (segIntersect(segA, segB, cornerC, cornerD, intersection))
		return true;

	if (segIntersect(segA, segB, cornerD, cornerA, intersection))
		return true;

	return false;
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


