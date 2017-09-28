#pragma once

#include <vector>
#include <random>

#include "glm\glm.hpp"

#include "..\3DEngine\src\renderer\model.h"

enum Tbranch {split, lateral};

struct TStem {
	int stage; ///<What recursive stage we're at
	glm::vec3 basePos;
	glm::vec3 direction; ///<Direction stem extends from its base.
	float length;
	float baseRadius; ///<Radius of stem base. End will usually be smaller.
	unsigned int parentEndRingIndexPos; ///<Locates previous segment's end verts, for re-use. 
	glm::vec3 normal; ///<Used to find rotation axis etc
	int jointCount; ///<The number of joints in this segment
	float branchScale; ///<Extent to which child branches scale this parent.
	float distFromLastLateral; ///<Track if it's time for another lateral branch;
	float extension; ///<Distance from start of this stage.
	//TO DO: maybe track length of whole stage - its useful
};

/** The base class for creating fractal tree models. */

class CFractalTree {
public:
	CFractalTree();
	void create();
	void createStem(TStem& stem);
	void addAppendingSegment(TStem& stem, float startRadius, unsigned int endRingIndexPos);
	void splitBranch(TStem& stem, unsigned int endRingIndexPos);
	void createBranches(TStem& stem, glm::vec3 branchPoint);
	void addLateralBranches(TStem & stem);
	void setLength(float length, float variation);
	void setStageScale(float scale);
	void setStemRadius(float radius);
	void setNumBranches(int nBranches, int variation);
	void setBranchAngle(float angle, float variation);
	void setMaxJoints(int nJoints);
	void setJointAngle(float angle, float variation);
	void setLeadingBranch(bool onOff);
	void initRandAngle();
	float getRandAngle(float maxAngle);
	void setBranchType(Tbranch branchType);
	void setNumLateralPoints(int nPoints);
	void setMaxStages(int nStages);
	void setStemFaces(int nFaces);
	void setColour(glm::vec4& col);
	void getModel(CModel* pModel);
	glm::vec3 createStemWire(const int stage, const glm::vec3 & pos, const glm::vec3 & angle);
	glm::vec3 createStemSolid(TStem& stem, float endRadius, unsigned int& newEndRingStart);
	void addHead(TStem& stem, glm::vec3 headPos);

	float stage1stemLength; ///<Length of initial stem.
	float stageScale; ///<Proportion by which successive stages scale.
	float stage1stemRadius; ///<Radius of base of initial stem.
	int stemFaces; ///<Number of stages to a stem cross-section.

	std::vector<vBuf::T3DnormVert> verts; ///<The vertices required to draw this model.
	std::vector<unsigned int> index;
	unsigned int currentIndex; ///<Current index for indexed drawing.

	glm::vec4 colour;
	int maxStages; ///<Maximum level of recursion.
	int maxJoints; ///<Maximum joints per segment
	float jointAngle; ///<Degree to which appending segments change direction
private:
	std::mt19937 randEngine{};
	std::uniform_real_distribution<float> stemLengthVariance;
	std::uniform_int_distribution<int> branchNumVariance;
	std::uniform_real_distribution<float> branchAngleVariance;
	std::uniform_real_distribution<float> randAngle;

	float currentStemLength; ///<Stem length of the current stem.
	float branchInset;
	float branchAngle;
	int branchesPerStage;
	Tbranch branchType;
	bool leadingBranch; ///<True if branching features a leading branch
	float taper; ///<How much the diameter of a stem shrinks, start to end.
	float taperFactor; ///<How much taper occurs over distance.
	float leadingBranchDominance; ///<How much bigger the leading branch is than its siblings.
	float leadingBranchMaxAngle; ///<Degree to which leading branch varies from parent direction.
	int lateralPointsPerStage; ///<Number of lateral branch start points per stage.

	float taper2;
};