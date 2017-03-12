#include "model.h"

CModel::CModel(glm::vec3& pos) :  C3dObject(pos) {
	init();
}

CModel::CModel() {
	init();
}

void CModel::init() {
	drawMode = GL_TRIANGLES;
	hBuffer = 0;
	hVAO = 0;
	hIndex = 0;
	nTris = 0;
}
