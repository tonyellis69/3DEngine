#include "model2.h"

#include "renderer.h"
#include "../colour.h"

#include <glm/gtc/matrix_transform.hpp>	

using namespace glm;

CModel2::CModel2() {
	CRenderer* pRenderer = &renderer; // &CRenderer::getInstance();
	setDrawCallout(pRenderer->phongDrawCallout, pRenderer);
	material.diffuse = vec4(col::randHue(),1.0f);
	material.ambient = material.diffuse;
	material.specular = vec4(1);
	material.texture1 = pRenderer->texture1x1;

	scaleVec= vec3(1);
}

/** Set the draw function this model will use to draw itself. */
void CModel2::setDrawCallout(callbackFunctionPtr cb, void * callee) {
	drawCallee = callee;
	drawCallback = cb;
}

/** Load a mesh from memory. */
void CModel2::loadMesh(CMesh& newMesh) {
	mesh = newMesh;
	bufferMesh();
	calcBBextent();
}

/** Upload the mesh data to a graphics hardware buffer. */
//!!!!Adapted to CBuf2 without testing!!!!!!!!!!!!
void CModel2::bufferMesh() {
	if (mesh.texCoords.empty()) {
		/*buffer.storeVertexes(mesh.vertices, mesh.normals);
		buffer.storeIndex(mesh.indices.data(), mesh.indices.size());
		buffer.storeLayout(3, 3, 0, 0);*/

		std::vector<vBuf::T3DnormVert> normVerts;
		int i = 0;
		for (auto v : mesh.vertices) {
			vBuf::T3DnormVert nv;
			nv.v = v; nv.normal = mesh.normals[i++];
			normVerts.push_back(nv);
		}

		buffer.storeVerts(normVerts, mesh.indices, 3, 3);
		return;
	}

	/*buffer.storeVertexes(mesh.vertices, mesh.normals,mesh.texCoords);
	buffer.storeIndex(mesh.indices.data(), mesh.indices.size());
	buffer.storeLayout(3, 3, 2, 0);*/

	std::vector<vBuf::T3DnormTexVert> normTexVerts;
	int i = 0;
	for (auto v : mesh.vertices) {
		normTexVerts.push_back({ v,mesh.normals[i],mesh.texCoords[i] });
		i++;
	}
	unsigned int nVerts = normTexVerts.size();
	buffer.storeVerts(normTexVerts, mesh.indices, 3, 3, 2);
}

void CModel2::draw() {
	drawCallback(drawCallee, this);
	//material.draw();
	//TO DO: do I want material to own the 
}

void CModel2::scale(glm::vec3 & scaleVec) {
	this->scaleVec = scaleVec;
	orientateWorldMatrix();
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

void CModel2::setTexture(CRenderTexture * texture) {
	material.texture1 = texture;
}

/** Find the half-width extent for the axis-aligned bounding box
	of this model's mesh.
	TO DO: consider moving to CPhysModel if never used here.*/
void CModel2::calcBBextent() {
	//find most distant x,y,z
	BBextent = glm::vec3(FLT_MIN);
	for (auto vert : mesh.vertices) {
		BBextent = glm::max(BBextent, glm::abs(vert));
	}
}

