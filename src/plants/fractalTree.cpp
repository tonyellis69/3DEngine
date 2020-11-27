#include "fractalTree.h"
#include "..\3DEngine\src\colour.h"
#include "..\3DEngine\src\renderer\material.h"
#include "..\shapes.h"

#include <glm/gtx/rotate_vector.hpp>

#include <vector>

CFractalTree::CFractalTree() {
	currentIndex = 0;
	stemFaces = 5;
	maxStages =  4;
	randEngine.seed(740608);
	stageScale = 0.6f;
	branchInset = 0.995f;
	branchAngle = 30.0f;
	branchesPerStage =  1;
	branchType = split;
	maxJoints = 0;
	leadingBranch = false;
	taper2 = 0.7f; //=reduction over stage length
	leadingBranchDominance = 0.75f;
	lateralPointsPerStage = 0;
}

/** Create the entire plant. */
void CFractalTree::create() {
	float distToNextLateral = stage1stemLength / lateralPointsPerStage;
	taperFactor = taper2 / (maxJoints + 1);
	float stemDivisor = maxJoints + 1.0f;
	if (leadingBranch) 
		stemDivisor++; //avoid branches getting too disproportionate.
	stage1stemLength = stage1stemLength / stemDivisor;
	
	TStem stage1Stem = { 1, glm::vec3(0,-2,0), glm::vec3(0,1,0), stage1stemLength, stage1stemRadius, 0, 
		glm::vec3(0,0,-1), maxJoints,  stageScale, 0, 0 };
	createStem(stage1Stem);
}

/** Recursively create this stem and any sub-stems. */
void CFractalTree::createStem(TStem& stem) {
	stem.length +=  stemLengthVariance(randEngine) * stem.length;//TO DO: do this before creating stem
	float endRadius = stem.baseRadius - stem.baseRadius * (taperFactor);

	unsigned int endRingIndexPos;
	glm::vec3 end = createStemSolid(stem, endRadius,  endRingIndexPos);
	float offset = 0;
	if (stem.stage < maxStages && branchType == lateral) {
		addLateralBranches(stem);
	}
	
	//create an appending segment, if required
	if (stem.jointCount > 0) {
		addAppendingSegment(stem, endRadius, endRingIndexPos);
		return;
	} 
	
	//end this run of segments in some way
	if (stem.stage < maxStages && branchType == split) {
		glm::vec3 branchPoint = end - (stem.direction * stem.length * 0.05f);
		createBranches(stem, branchPoint);
	}

	if (stem.stage == maxStages) {
		addHead(stem, end);

	}
}

/** add a segment to the end of the current one. */
void CFractalTree::addAppendingSegment(TStem & stem, float startRadius, unsigned int endRingIndexPos) {
	glm::vec3 startPoint = stem.basePos + stem.direction * stem.length;

	//give the new segment a slight, random angle change
	glm::vec3 jointAxis = stem.normal; 
	jointAxis = glm::rotate(jointAxis, glm::radians(getRandAngle(360)), stem.direction); //create axis
	glm::vec3 childStemAngle = glm::rotate(stem.direction, glm::radians(jointAngle), jointAxis); //tilt it on that axix
	glm::vec3 childStemNormal = glm::rotate(stem.normal, glm::radians(jointAngle), jointAxis);
	childStemNormal = glm::normalize(childStemNormal);

	float childStemLength = stem.length;
	
	TStem childStem = { stem.stage, startPoint, childStemAngle, childStemLength, startRadius , endRingIndexPos,
		childStemNormal, stem.jointCount - 1,  stem.branchScale, stem.distFromLastLateral, stem.extension + childStemLength };
	createStem(childStem);
}


/** Recursively create a new stage of branches at the given point. */
void CFractalTree::createBranches(TStem& stem, glm::vec3 branchPoint) {
	/* To create child branches, we need to know their size: length and radius+. Both depend on user-set proportions, but also on
	where we are in the tree. We can use the stage number, which gives proportions for a stage as a whole, modified by the distance 
	down the parent branch. The later raises problems, as the child may be growing from a segment several segments down the length
	of the parent branch, so how do we know the distance? Segments mostly only carry information about themselves, not parents.
		Solution: each segment needs to carry a value of how distant it personally is from the start of its branch. Let's call it
	'extension'. As extension grows, segments need to become smaller, so it needs to be divided into some constant.
		+We also need to know much the radius of the branch shrinks from one end to the other. This is also a user-set value, eg,
	'1 in every 20'. But we don't need to modify this value, it's a general rule. Call it 'taper'.
		For simplicity, the user specifies how much he wants a branch radius to decrease over its entire stage, eg, 0.1. */



	int numBranches = branchesPerStage + branchNumVariance(randEngine);

	float childStemLength = stem.length * stem.branchScale;

	float endRadius = stem.baseRadius - stem.baseRadius * (taperFactor);
	float childStartRadius = endRadius; //TO DO, not entirely accurate, should be affected by distance from base
	childStartRadius *= stem.branchScale;

	glm::vec3 branchAxis = stem.normal;
	for (int branch = 0; branch < numBranches; branch++) {
		branchAxis = glm::rotate(branchAxis, glm::radians(360.0f / numBranches), stem.direction);
		float finalAngle = branchAngle + (branchAngle * branchAngleVariance(randEngine));  // - + ???
		glm::vec3 childStemAngle = glm::rotate(stem.direction, glm::radians(finalAngle), branchAxis);
		glm::vec3 childStemNormal = glm::rotate(stem.normal, glm::radians(finalAngle), branchAxis);
		childStemNormal = glm::normalize(childStemNormal);

		TStem childStem = { stem.stage + 1, branchPoint, childStemAngle, childStemLength, childStartRadius , 0,
			childStemNormal, maxJoints,  stem.branchScale,0,0 };
		createStem(childStem);
	}
} 

/**	Grow one or more stems off of this one, without otherwise affecting it. */
void CFractalTree::addLateralBranches(TStem& stem) {
	/* To add lateral branches, we need to know the point at which they occur on the parent stage. The
	user sets the number of lateral points per stage - this is a general rule.  */
	float approxStageLength = stem.length * (maxJoints + 1);
	float gapNetweenLaterals = approxStageLength / lateralPointsPerStage;
	gapNetweenLaterals *= 0.9f; //adjust down to avoid laterals too near the end of the parent stage
	float distToNextLateral = gapNetweenLaterals - stem.distFromLastLateral;
	

	glm::vec3 branchPos = stem.basePos + (stem.direction * distToNextLateral);
	float travel = distToNextLateral;
	while (travel < stem.length) {
		createBranches(stem, branchPos);
		travel += gapNetweenLaterals;
		branchPos += stem.direction * gapNetweenLaterals;
	}
	float offset = stem.length - (travel - gapNetweenLaterals);
	stem.distFromLastLateral = offset;
}

void CFractalTree::setLength(float length, float variation) {
	stage1stemLength = length;
	using param_t = std::uniform_real_distribution<float>::param_type;
	param_t p{ -variation, variation };
	stemLengthVariance.param(p);
}

void CFractalTree::setStageScale(float scale) {
	stageScale = scale;
}

void CFractalTree::setStemRadius(float radius) {
	stage1stemRadius = radius;
}

void CFractalTree::setNumBranches(int numBranches, int variation) {
	branchesPerStage = numBranches;
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

void CFractalTree::setMaxJoints(int nJoints) {
	maxJoints = nJoints;
}

void CFractalTree::setJointAngle(float angle, float variation) {
	jointAngle = angle;
}

void CFractalTree::setLeadingBranch(bool onOff) {
	leadingBranch = onOff;
}


float CFractalTree::getRandAngle(float maxAngle) {
	std::uniform_real_distribution<float> d{};
	using param_t = decltype(d)::param_type;
	return d(randEngine, param_t{ -maxAngle, maxAngle });
}

void CFractalTree::setBranchType(Tbranch bType) {
	branchType = bType;
}

/** Set the number of lateral branching points per stage. */
void CFractalTree::setNumLateralPoints(int nPoints) {
	lateralPointsPerStage = nPoints;
}

void CFractalTree::setMaxStages(int nStages) {
	maxStages = nStages;
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
	endVert.v = pos + angle * stage1stemLength * float(1.5f / stage);
	verts.push_back(endVert);
	index.push_back(currentIndex++);
	return endVert.v;
}

glm::vec3 CFractalTree::createStemSolid(TStem& stem, float endRadius, unsigned int& newendRingIndexPos) {
	glm::vec3 stemNormal = stem.normal;// getNormal(stem.angle); //create an axis perpendicular to this stem
	
	//float currentRadius = stemRadius * float(1.5f / stem.stage);
	float rot = 360.0f / stemFaces;

	vBuf::T3DnormVert vert;
	unsigned int baseRingStart;

	if (!stem.parentEndRingIndexPos) 
	{
		//Create a ring of base vectors
		baseRingStart = verts.size();
		for (int face = 0; face < stemFaces; face++) {
			vert.v = glm::vec3(0) + (stemNormal * stem.baseRadius);
			vert.v = glm::rotate(vert.v, glm::radians(-rot * face), stem.direction);
			vert.normal = glm::normalize(vert.v);
			vert.v = stem.basePos + vert.v;
			verts.push_back(vert);
		}
	}
	else
		baseRingStart = stem.parentEndRingIndexPos;

	//float endRadius = stem.radius *  stem.scaleFactor; // stageScale;

	//create ring of end vectors
	unsigned int endRingIndexPos = verts.size();
	glm::vec3 end = stem.basePos + stem.direction * stem.length;
	for (int face = 0; face < stemFaces; face++) {
		vert.v = glm::vec3(0) + (stemNormal * endRadius);
		vert.v = glm::rotate(vert.v, glm::radians(-rot * face), stem.direction);
		vert.normal = glm::normalize(vert.v);
		vert.v = end + vert.v;
		verts.push_back(vert);
	}

	//stitch them together via indexing
	int face = 0;



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
	
	newendRingIndexPos = endRingIndexPos;

	return end;
}

void CFractalTree::addHead(TStem & stem, glm::vec3 headPos) {

	//get the verts for a head.
	//scale them
	//add to existing verts. Will need to use primitive restart for every tri

	std::vector<glm::vec3> headVerts, normals;
	std::vector<unsigned int> headIndex;
	float scale = stem.baseRadius * 20.0f; //or something
	shape::cube(&headVerts, &normals, &headIndex);
	shape::scale(headVerts,glm::vec3(scale));

	//rotate to stem angle
	float angle = glm::dot(stem.direction, glm::vec3(0, 1, 0));
	angle = glm::acos(angle);
	angle = glm::degrees(angle);

	glm::vec3 rotAxis = glm::cross(stem.direction, glm::vec3(0, 1, 0));
	glm::mat3 rotMat = glm::rotate(angle, rotAxis);

	int i = 0;
	for (auto& v : headVerts) {
		v = rotMat * v;
		normals[i] = rotMat * normals[i];
		i++;
	}

	//translate to end of stem
	for (auto& v : headVerts) {
		v = v + headPos;
	}




	int offset = verts.size();

	vBuf::T3DnormVert nv;
	i = 0;
	for (auto& v : headVerts) {
		nv.v = v; nv.normal = normals[i++];
		verts.push_back(nv);
	}

	i = 0;
	for (auto& ind : headIndex) {
		index.push_back(ind + offset);
		i++;
		if (i == 3) {
			i = 0;
			index.push_back(65535);
		}
	}


}


