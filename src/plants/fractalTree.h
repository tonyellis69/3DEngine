#pragma once

#include <vector>
#include "glm\glm.hpp"

#include "..\3DEngine\src\renderer\model.h"

/** The base class for creating fractal tree models. */

class CFractalTree {
public:
	CFractalTree();
	void createStem(const int stage, const glm::vec3& pos, const glm::vec3& angle);
	void setStemLength(float length);
	void setStemRadius(float radius);
	void setStemFaces(int nFaces);
	void setColour(glm::vec4& col);
	void getModel(CModel* pModel);
	glm::vec3 createStemWire(const int stage, const glm::vec3 & pos, const glm::vec3 & angle);
	glm::vec3 createStemSolid(const int stage, const glm::vec3 & pos, const glm::vec3 & angle);

	float stemLength; ///<Length of initial stem.
	float stemRadius; ///<Half thickness of initial stem.
	int stemFaces; ///<Number of stages to a stem cross-section.

	std::vector<vBuf::T3DnormVert> verts; ///<The vertices required to draw this model.
	std::vector<unsigned short> index;
	unsigned int currentIndex; ///<Current index for indexed drawing.

	glm::vec4 colour;
	int maxStages; ///<Maximum level of recursion.
};