#include "model2.h"

#include "renderer.h"
#include "../colour.h"

#include <glm/gtc/matrix_transform.hpp>	

using namespace glm;

CModel2::CModel2() {
	CRenderer* pRenderer = &CRenderer::getInstance();
	setDrawCallout(pRenderer->phongDrawCallout, pRenderer);
	colour = vec4(col::randHue(),1.0f);
	scaleVec= vec3(1);
}

/** Set the draw function this model will use to draw itself. */
void CModel2::setDrawCallout(callbackFunctionPtr cb, void * callee) {
	drawCallee = callee;
	drawCallback = cb;
}

/** Load a mesh from memory. */
void CModel2::loadMesh(CMesh<glm::vec3> & newMesh) {
	mesh = newMesh;
	bufferMesh();
}

/** Upload the mesh data to a graphics hardware buffer. */
void CModel2::bufferMesh() {
	//TO DO: vary upload with mesh type
	buffer.storeVertexes(mesh.vertices, mesh.normals);
	buffer.storeIndex(mesh.indices.data(), mesh.indices.size());
	buffer.storeLayout(3, 3, 0, 0);
}

void CModel2::draw() {
	drawCallback(drawCallee, this);
}

void CModel2::scale(glm::vec3 & scaleVec) {
	this->scaleVec = scaleVec;
}

/** Update world matrix with current orientation. */
void CModel2::orientateWorldMatrix() {
	//glm::vec3 pos = getPos();
	//worldMatrix = glm::mat4_cast(orientation);

	mat4 rotationMatrix = glm::mat4_cast(orientation);

	//	worldMatrix =  rotationMatrix;

		//reapply translation
		//worldMatrix[3] = glm::vec4(pos,1);
	mat4 transM = glm::translate(mat4(1), getPos());
	mat4 scaleM = glm::scale(mat4(1), scaleVec);
	worldMatrix = transM * rotationMatrix * scaleM;

}

