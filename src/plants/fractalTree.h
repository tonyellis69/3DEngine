#pragma once

#include <vector>
#include <random>

#include "glm\glm.hpp"

#include "..\3DEngine\src\renderer\model.h"


struct TStem {
	int stage;
	glm::vec3 pos;
	glm::vec3 angle;
	float length;
	float radius;
	unsigned int parentEndRing;
};

/** The base class for creating fractal tree models. */

class CFractalTree {
public:
	CFractalTree();
	void create();
	void createStem(TStem& stem);
	void setStemLength(float length, float variation);
	void setStageScale(float scale);
	void setStemRadius(float radius);
	void setNumBranches(int nBranches, int variation);
	void setBranchAngle(float angle, float variation);
	void setStemFaces(int nFaces);
	void setColour(glm::vec4& col);
	void getModel(CModel* pModel);
	glm::vec3 createStemWire(const int stage, const glm::vec3 & pos, const glm::vec3 & angle);
	glm::vec3 createStemSolid(TStem& stem, unsigned int& newEndRingStart);

	glm::vec3 getNormal(const glm::vec3& v);

	float stemLength; ///<Length of initial stem.
	float stageScale; ///<Proportion by which successive stages scale.
	float stemRadius; ///<Half thickness of initial stem.
	int stemFaces; ///<Number of stages to a stem cross-section.

	std::vector<vBuf::T3DnormVert> verts; ///<The vertices required to draw this model.
	std::vector<unsigned int> index;
	unsigned int currentIndex; ///<Current index for indexed drawing.

	glm::vec4 colour;
	int maxStages; ///<Maximum level of recursion.

private:
	std::mt19937 randEngine{};
	std::uniform_real_distribution<float> stemLengthVariance;
	std::uniform_int_distribution<int> branchNumVariance;
	std::uniform_real_distribution<float> branchAngleVariance;

	float currentStemLength; ///<Stem length of the current stem.
	float branchInset;
	float branchAngle;
	int nBranches;
};