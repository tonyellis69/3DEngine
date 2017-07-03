#include "playerObj.h"



CPlayerObj::CPlayerObj() {
	width = 0.5f;
	height = 1.75f;

	eyeOffset = glm::vec3(0, height / 2, 0);
	updatePov();
}

void CPlayerObj::updatePov() {
	povCam.setPos(getPos() + eyeOffset);
}

void CPlayerObj::setPos(glm::vec3 & pos) {
	C3dObject::setPos(pos);
	updatePov();
	pModel->setPos(getPos());
}
