#include "fractalTree.h"
#include "..\3DEngine\src\colour.h"
#include "..\3DEngine\src\renderer\material.h"

#include <glm/gtx/rotate_vector.hpp>



CFractalTree::CFractalTree() {
	currentIndex = 0;
	stemFaces = 4;
	maxStages =  5;
	randEngine.seed(740608);
	stageScale = 0.6f;
	branchInset = 0.995f;
	branchAngle = 30.0f;
	nBranches =  1;
}

/** Create the entire plant. */
void CFractalTree::create() {
	TStem baseStem = { 1, glm::vec3(0), glm::vec3(0,1,0), stemLength, stemRadius, 0};
	createStem(baseStem);
}

void CFractalTree::createStem(TStem& stem) {

	
	stem.length +=  stemLengthVariance(randEngine) * stem.length;
	unsigned int endRingStart;
	glm::vec3 end = createStemSolid(stem, endRingStart);

	if (stem.stage < maxStages) {
		glm::vec3 rotationAxis = getNormal(stem.angle); //create an axis perpendicular to this stem
		glm::vec3 branchPoint = stem.pos + stem.angle * stem.length ;
		float newLength = stem.length * stageScale;
		int numBranches = nBranches + branchNumVariance(randEngine);
		for (int branch = 0; branch < numBranches; branch++) {
			float variance = branchAngle * branchAngleVariance(randEngine);
			glm::vec3 newAngle = glm::rotate(stem.angle, glm::radians(branchAngle + variance), rotationAxis);

			TStem childStem = { stem.stage + 1, branchPoint, newAngle, newLength, stem.radius  * stageScale, endRingStart };
			createStem(childStem);
			rotationAxis = glm::rotate(rotationAxis, glm::radians(360.0f/ numBranches), stem.angle);
		}
	}
}

void CFractalTree::setStemLength(float length, float variation) {
	stemLength = length;
	using param_t = std::uniform_real_distribution<>::param_type;
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
	using param_t = std::uniform_int_distribution<>::param_type;
	param_t p{ -variation, variation };
	branchNumVariance.param(p);
}

void CFractalTree::setBranchAngle(float angle, float variation) {
	branchAngle = angle;
	using param_t = std::uniform_real_distribution<>::param_type;
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

glm::vec3 CFractalTree::createStemSolid(TStem& stem, unsigned int& newEndRingStart) {
	glm::vec3 stemNormal = getNormal(stem.angle); //create an axis perpendicular to this stem
	
	//float currentRadius = stemRadius * float(1.5f / stem.stage);
	float rot = 360 / stemFaces;

	vBuf::T3DnormVert vert;
	unsigned int baseRingStart;

	if (!stem.parentEndRing) {
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
	unsigned int endRingStart = verts.size();
	glm::vec3 end = stem.pos + stem.angle * stem.length;
	for (int face = 0; face < stemFaces; face++) {
		vert.v = glm::vec3(0) + (stemNormal * endRadius);
		vert.v = glm::rotate(vert.v, glm::radians(-rot * face), stem.angle);
		vert.normal = glm::normalize(vert.v);
		vert.v = end + vert.v;
		verts.push_back(vert);
	}

	//stitch them together via indexing
	int startIndex = currentIndex;
	int face;
	for ( face = 0; face < stemFaces-1 ; face++) {
		index.push_back(baseRingStart + face); //0
		index.push_back(endRingStart + face); //4
		index.push_back(baseRingStart + face + 1); //1
	
		index.push_back(endRingStart + 1 + face); //5
		index.push_back(baseRingStart + 1 + face); //1
		index.push_back(endRingStart + face); //4
		//currentIndex++;
	}
	
	index.push_back(baseRingStart + face); //0
	index.push_back(endRingStart + face); //4
	index.push_back(baseRingStart); //1

	index.push_back(endRingStart); //5
	index.push_back(baseRingStart); //1
	index.push_back(endRingStart + face); //4

	
	//currentIndex += stemFaces +1;
	newEndRingStart = endRingStart;

	return end;
}

glm::vec3 CFractalTree::getNormal(const glm::vec3& v) {
	glm::vec3 n = glm::cross(v, glm::vec3(0, 0, 1));
	if (glm::length(n) < 0.00001f)
		n = glm::cross(v, glm::vec3(1, 0, 0));
	return glm::normalize(n);
}

