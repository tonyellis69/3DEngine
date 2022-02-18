#include "model.h"

#include "intersect.h"


TModelMesh* CModel::getMesh(const std::string& name) {
	for (auto& mesh : meshes)
		if (mesh.name == name)
			return &mesh;
	return nullptr;
}

/** Return true if the given segment intersects the circle
	described by the outermost vertex. */
//TO DO: needs rewrite for CModel.
bool CModel::circleCollision(glm::vec3& segA, glm::vec3& segB) {
	float radius = glm::length(extents.furthestVert);
	glm::vec3 circleOrig = tmpMatrix[3];
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
bool CModel::BBcollision(glm::vec3& segA, glm::vec3& segB) {
	glm::vec3 intersection;
	//for each line in bounding box
	//test intersection
	glm::vec3 cornerA(extents.BBmin.x, extents.BBmax.y, 0);
	glm::vec3 cornerB = extents.BBmax;
	glm::vec3 cornerC(extents.BBmax.x, extents.BBmin.y, 0);
	glm::vec3 cornerD = extents.BBmin;

	cornerA = tmpMatrix * glm::vec4(cornerA, 1);
	cornerB = tmpMatrix * glm::vec4(cornerB, 1);
	if (segIntersect(segA, segB, cornerA, cornerB, intersection))
		return true;

	cornerC = tmpMatrix * glm::vec4(cornerC, 1);
	if (segIntersect(segA, segB, cornerB, cornerC, intersection))
		return true;

	cornerD = tmpMatrix * glm::vec4(cornerD, 1);
	if (segIntersect(segA, segB, cornerC, cornerD, intersection))
		return true;

	if (segIntersect(segA, segB, cornerD, cornerA, intersection))
		return true;

	return false;
}


float CModel::getRadius() {
	return glm::length(extents.furthestVert);
}

/** Probably temporary colour assignment. */
void CModel::setBasePalette(std::vector<glm::vec4>& palette) {
	for (auto& mesh : meshes)
		mesh.palette = palette;
}
