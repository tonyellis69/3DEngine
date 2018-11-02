#pragma once

#include "compositor.h"

class ComposeTest : public Compositor {
public:
	~ComposeTest();
	void initTex();
	void compose();
	CTexGen* createNoiseTex();
	CTexGen* createRidgedMFTex();
	CTexGen* createCylinderTex();
	CTexGen* createTurbulenceTex(CTexGen* source);
	CTexGen* createColouriseTex(CTexGen* source, ColourGradient* colourGradient);
	CTexGen* createScaleBiasTex(CTexGen* source);
	CTexGen* createScalePointTex(CTexGen* source);
	CTexGen* createAddTex(CTexGen* source1, CTexGen* source2);
	CTexGen* createLayerTex(CTexGen* source1, CTexGen* source2);

	CRenderTexture* createTarget();

	void updateOctaves(int octaves);
	void updateFrequency(float freq);
	void updateAngle(glm::vec3 angle);
	void updatePosition(glm::vec3 angle);
	void updateScalePt(glm::vec3 scale);
	void updatePower(float power);
	void updateScale(float scale);
	void updateBias(float bias);
	void updateColourGradient(ColourGradient& gradient);

	void scrollStack(int direction);

	void setCurrentLayer(int layerNo);

//	int octaves;
//	float frequency;
	glm::vec3 angle;

	int stackPosition;
	CTexGen* currentTexGen;
	std::vector<CTexGen*> texGens;
	CRenderTexture* currentTarget;
	std::vector<CRenderTexture*> targets;

	int textureSize;
};