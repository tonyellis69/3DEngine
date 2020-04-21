#include "physModel.h"

void CPhysModel::updateModelPosition(glm::vec3& dPos) {
	translate(dPos);
}

void CPhysModel::scale(glm::vec3& scaleVec) {
	CModel2::scale(scaleVec);
	setBB(scaleVec);
}

glm::vec3 CPhysModel::getPos() {
	return CModel2::getPos();
}

TAaBB CPhysModel::calcAABB() {
	TAaBB AaBb = { -BBextent, BBextent };
	glm::vec4 AABBmin = worldMatrix * glm::vec4(-BBextent, 1);
	glm::vec4 AABBmax = worldMatrix * glm::vec4(BBextent, 1);
	return { glm::vec3(AABBmin), glm::vec3(AABBmax) };
}
