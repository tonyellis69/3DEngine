#pragma once

#include <glm/glm.hpp>
//#include "skyShader.h"
#include "renderer\model.h"
#include "renderer\billboard.h"

enum TSkyDomeRange {skyHorizon,skyNearHorizon,skyLowerThird,skyDominant};

class CSkyDome {
public:
	CSkyDome();
	~CSkyDome();
	void setHorizonColour(glm::vec3& colour);
	void setNearHorizonColour(glm::vec3& colour);
	void setLowerThirdColour(glm::vec3& colour);
	void setDominantColour(glm::vec3& colour);
	void setModel(CModel* model);
	void update(double& dT);


	CModel* dome;
	glm::vec3 heightColours[4];
	float levels[4];
	

	CModel* plane;
	CMaterial* cloud;
	glm::vec2 cloudOffset;

	CShader* skyShader;
	GLuint hSkyDomeHeightColours;
	GLuint hSkyDomeMVP;

	CBillboard* sunBoard;
	CMaterial* sunMat;
};