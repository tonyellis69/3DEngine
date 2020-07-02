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
	model.setPos(getPos());
}

void CPlayerObj::translate(glm::vec3& t) {
	C3dObject::translate(t);
	updatePov();
	model.translate(t);
}


CPlayerObj2::CPlayerObj2() {
	width = 0.5f;
	height = 1.75f;

	eyeOffset = glm::vec3(0, height / 2, 0);
	updatePov();
}

void CPlayerObj2::updatePov() {
	povCam.setPos(getPos() + eyeOffset);
}

void CPlayerObj2::setPos(glm::vec3& pos) {
	C3dObject::setPos(pos);
	updatePov();
	model.setPos(getPos());
}

void CPlayerObj2::translate(glm::vec3& t) {
	C3dObject::translate(t);
	updatePov();
	model.translate(t);
}