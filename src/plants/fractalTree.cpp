#include "fractalTree.h"
#include "..\3DEngine\src\colour.h"
#include "..\3DEngine\src\renderer\material.h"

#include <glm/gtx/rotate_vector.hpp>



CFractalTree::CFractalTree() {
	currentIndex = 0;
	stemFaces = 5;
	maxStages =  4;
	randEngine.seed(740608);
	stageScale = 0.6f;
	branchInset = 0.995f;
	branchAngle = 30.0f;
	nBranches =  1;
}

/** Create the entire plant. */
void CFractalTree::create() {
	TStem baseStem = { 1, glm::vec3(0,-2,0), glm::vec3(0,1,0), stemLength, stemRadius, 0, glm::vec3(0,0,-1) };
	createStem(baseStem);
}

/** Recursively create this stem and any sub-stems. */
void CFractalTree::createStem(TStem& stem) {
	stem.length +=  stemLengthVariance(randEngine) * stem.length;

	unsigned int endRingIndexPos;
	glm::vec3 end = createStemSolid(stem, endRingIndexPos);
	if (stem.stage < maxStages) {
		glm::vec3 rotationAxis = stem.normal;//  getNormal(stem.angle); //create an axis perpendicular to this stem
		glm::vec3 branchPoint = stem.pos + stem.angle * stem.length ;
		float childStemLength = stem.length * stageScale;
		int numBranches = nBranches + branchNumVariance(randEngine);
		for (int branch = 0; branch < numBranches; branch++) {
			float variance = branchAngle * branchAngleVariance(randEngine);
			glm::vec3 childStemAngle = glm::rotate(stem.angle, glm::radians(branchAngle + variance), rotationAxis);
			glm::vec3 childStemNormal = glm::rotate(stem.normal, glm::radians(branchAngle + variance), rotationAxis);
			childStemNormal = glm::normalize(childStemNormal);

			TStem childStem = { stem.stage + 1, branchPoint, childStemAngle, childStemLength, stem.radius  * stageScale, endRingIndexPos,
				childStemNormal };
			createStem(childStem);
			rotationAxis = glm::rotate(rotationAxis, glm::radians(360.0f/ numBranches), stem.angle);
		}
	}
}

void CFractalTree::setStemLength(float length, float variation) {
	stemLength = length;
	using param_t = std::uniform_real_distribution<float>::param_type;
	param_t p{ -variation, variation };
	stemLengthVariance.param(p);
}

void CFractalTree::setStageScale(float scale) {
	stageScale = scale;
}

void CFractalTree::setStemRadius(float radius) {
	stemRadius = radius;
}

void CFractalTree::setNumBranches(int numBranches, int variation) {
	nBranches = numBranches;
	using param_t = std::uniform_int_distribution<int>::param_type;
	param_t p{ -variation, variation };
	branchNumVariance.param(p);
}

void CFractalTree::setBranchAngle(float angle, float variation) {
	branchAngle = angle;
	using param_t = std::uniform_real_distribution<float>::param_type;
	param_t p{ -variation, variation };
	branchAngleVariance.param(p);

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
	pModel->setDrawMode(drawTriStrip);
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

glm::vec3 CFractalTree::createStemSolid(TStem& stem, unsigned int& newendRingIndexPos) {
	glm::vec3 stemNormal = stem.normal;// getNormal(stem.angle); //create an axis perpendicular to this stem
	
	//float currentRadius = stemRadius * float(1.5f / stem.stage);
	float rot = 360.0f / stemFaces;

	vBuf::T3DnormVert vert;
	unsigned int baseRingStart;

	if (!stem.parentEndRing) 
	{
		//Create a ring of base vectors
		baseRingStart = verts.size();
		for (int face = 0; face < stemFaces; face++) {
			vert.v = glm::vec3(0) + (stemNormal * stem.radius);
			vert.v = glm::rotate(vert.v, glm::radians(-rot * face), stem.angle);
			vert.normal = glm::normalize(vert.v);
			vert.v = stem.pos + vert.v;
			verts.push_back(vert);
		}
	}
	else
		baseRingStart = stem.parentEndRing;

	float endRadius = stem.radius * stageScale;

	//create ring of end vectors
	unsigned int endRingIndexPos = verts.size();
	glm::vec3 end = stem.pos + stem.angle * stem.length;
	for (int face = 0; face < stemFaces; face++) {
		vert.v = glm::vec3(0) + (stemNormal * endRadius);
		vert.v = glm::rotate(vert.v, glm::radians(-rot * face), stem.angle);
		vert.normal = glm::normalize(vert.v);
		vert.v = end + vert.v;
		verts.push_back(vert);
	}

	//stitch them together via indexing
	int face = 0;
/*	for ( face = 0; face < stemFaces-1 ; face++) {
		index.push_back(baseRingStart + face); //0
		index.push_back(endRingIndexPos + face); //4
		index.push_back(baseRingStart + face + 1); //1
	
		index.push_back(endRingIndexPos + 1 + face); //5
		index.push_back(baseRingStart + 1 + face); //1
		index.push_back(endRingIndexPos + face); //4
		//currentIndex++;
	}
	
	index.push_back(baseRingStart + face); //0
	index.push_back(endRingIndexPos + face); //4
	index.push_back(baseRingStart); //1

	index.push_back(endRingIndexPos); //5
	index.push_back(baseRingStart); //1
	index.push_back(endRingIndexPos + face); //4 */


	//first tri:
	index.push_back(baseRingStart + face); //0
	index.push_back(endRingIndexPos + face); //4
	index.push_back(baseRingStart + face + 1); //1

	//first tri strip vert
	index.push_back(endRingIndexPos + 1 + face); //5

	
	for (face = 1; face < stemFaces - 2; face++) {
		index.push_back(baseRingStart + face + 1);
		index.push_back(endRingIndexPos + 1 + face);
	}

	//stitch-up face
	index.push_back(baseRingStart); //0
	index.push_back(endRingIndexPos ); //4

	index.push_back(65535); //signals the end of this sequence
	
	//currentIndex += stemFaces +1;
	newendRingIndexPos = endRingIndexPos;

	return end;
}

glm::vec3 CFractalTree::getNormal(const glm::vec3& v) {
	glm::vec3 n = glm::cross(v, glm::vec3(0, 0, 1));
	if (glm::length(n) < 0.00001f)
		n = glm::cross(v, glm::vec3(1, 0, 0));
	return glm::normalize(n);
}

