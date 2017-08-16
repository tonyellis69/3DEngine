#include "fractalTree.h"
#include "..\3DEngine\src\colour.h"
#include "..\3DEngine\src\renderer\material.h"

#include <glm/gtx/rotate_vector.hpp>

CFractalTree::CFractalTree() {
	currentIndex = 0;
	stemFaces = 6;
	maxStages = 6;
}

void CFractalTree::createStem(const int stage, const glm::vec3 & pos, const glm::vec3 & angle) {
	if (stage > maxStages)
		return;

	//glm::vec3 end = createStemWire(stage, pos, angle);
	glm::vec3 end = createStemSolid(stage, pos, angle);


	//call self 3-4 times
	glm::vec3 newAngle;
	glm::vec3 rotationAxis = glm::cross(angle, glm::vec3(0, 0, 1)); //create an axis perpendicular to this stem
	glm::vec3 branchPoint = pos + angle * stemLength * float(1.49f / stage);
	for (int branch = 0; branch < 3; branch++) {
		newAngle = glm::rotate(angle, glm::radians(30.0f), rotationAxis);
		rotationAxis = glm::rotate(rotationAxis, glm::radians(120.0f), angle);
		createStem(stage + 1, branchPoint, newAngle);
	}
}

void CFractalTree::setStemLength(float length) {
	stemLength = length;
}

void CFractalTree::setStemRadius(float radius) {
	stemRadius = radius;
}

void CFractalTree::setStemFaces(int nFaces) {
	stemFaces = nFaces;
}


void CFractalTree::setColour(glm::vec4& col) {
	colour = col;
}

/** Create a model of this tree and return a pointer to it. */
void CFractalTree::getModel(CModel* pModel) {
	pModel->storeVertexes(verts.data(), sizeof(vBuf::T3DnormVert), verts.size());
	pModel->storeIndex(index.data(), index.size());
	pModel->storeLayout(3, 3, 0, 0);
	pModel->setDrawMode(drawTris);
	pModel->getMaterial()->setColour(glm::vec4(col::randHue(), 1));
}

glm::vec3 CFractalTree::createStemWire(const int stage, const glm::vec3 & pos, const glm::vec3 & angle) {
	//base vector
	vBuf::T3DnormVert baseVert;
	baseVert.v = pos;
	verts.push_back(baseVert);
	index.push_back(currentIndex++);

	//end vector
	vBuf::T3DnormVert endVert;
	endVert.v = pos + angle * stemLength * float(1.5f / stage);
	verts.push_back(endVert);
	index.push_back(currentIndex++);
	return endVert.v;
}

glm::vec3 CFractalTree::createStemSolid(const int stage, const glm::vec3 & pos, const glm::vec3 & angle) {
	//find normal to current stem
	glm::vec3 stemNormal = glm::cross(angle, glm::vec3(0, 0, 1)); //create an axis perpendicular to this stem
	stemNormal = glm::normalize(stemNormal);
	
	float currentRadius = stemRadius * float(1.5f / stage);
	float rot = 360 / stemFaces;

	vBuf::T3DnormVert vert;
	//Create a ring of base vectors
	for (int face = 0; face < stemFaces; face++) {
		vert.v = glm::vec3(0) + (stemNormal * currentRadius );
		vert.v = glm::rotate(vert.v, glm::radians(-rot * face), angle);
		vert.normal = glm::normalize(vert.v);
		vert.v = pos + vert.v;
		verts.push_back(vert);	
	}

	currentRadius *= 0.7f;

	//create ring of end vectors
	glm::vec3 end = pos + angle * stemLength * float(1.5f / stage);
	for (int face = 0; face < stemFaces; face++) {
		vert.v = glm::vec3(0) + (stemNormal * currentRadius );
		vert.v = glm::rotate(vert.v, glm::radians(-rot * face), angle);
		vert.normal = glm::normalize(vert.v);
		vert.v = end + vert.v;
		verts.push_back(vert);
	}

	//stitch them together via indexing
	int startIndex = currentIndex;
	
	for (int face = 0; face < stemFaces-1 ; face++) {
		index.push_back(currentIndex); //0
		index.push_back(currentIndex + stemFaces); //4
		index.push_back(currentIndex + 1); //1
	
		index.push_back(currentIndex + stemFaces + 1); //5
		index.push_back(currentIndex + 1); //1
		index.push_back(currentIndex + stemFaces); //4
		currentIndex++;
	}

	index.push_back(currentIndex); //0
	index.push_back(currentIndex + stemFaces); //4
	index.push_back(startIndex); //1

	index.push_back(startIndex + stemFaces ); //5
	index.push_back(startIndex  ); //1
	index.push_back(currentIndex + stemFaces); //4


	currentIndex += stemFaces +1;

	return end;
}
