#include "model.h"
#include "buf.h"
#include "renderer.h"
#include "../colour.h"

CModel::CModel(glm::vec3& pos) :  C3dObject(pos) {
	init();
}

CModel::CModel() {
	init();
}

void CModel::init() {
	drawMode = GL_TRIANGLES;

}

