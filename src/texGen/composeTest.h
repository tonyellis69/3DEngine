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
	CTexGen* createBillowTex();
	CTexGen* createVoronoiTex();
	CTexGen* createTurbulenceTex(CTexGen* source);
	CTexGen* createColouriseTex(CTexGen* source, ColourGradient* colourGradient);
	CTexGen* createScaleBiasTex(CTexGen* source);
	CTexGen * createSeamlessTex();
	CTexGen* createScalePointTex(CTexGen* source);
	CTexGen* createAddTex(CTexGen* source1, CTexGen* source2);
	CTexGen* createLayerTex(CTexGen* source1, CTexGen* source2);
	CTexGen* createSelectTex(CTexGen* source1, CTexGen* source2, CTexGen* control);
	CTexGen* createGausTex();

	void addToStack(CTexGen * texGen);
	

	CRenderTexture* createTarget();

	void updateOctaves(int octaves);
	void updateFrequency(float freq);
	void updatePersistence(float persist);
	void updateAngle(glm::vec3 angle);
	void updatePosition(glm::vec3 angle);
	void updateScalePt(glm::vec3 scale);
	void updatePower(float power);
	void updateScale(float scale);
	void updateBias(float bias);
	void updateDistance(bool dist);
	void updateRandomHue(bool hue);
	void updateColourGradient(ColourGradient& gradient);
	void updateLower(float lower);
	void updateUpper(float upper);
	void updateFalloff(float falloff);
	void updateSampleWidth(float width);
	void updateSampleHeight(float height);

	void scrollStack(int direction);

	void setCurrentLayer(int layerNo);

	void addTexGen(CTexGen* texGen);
	void deleteTexGen(int stackNo);

//	int octaves;
//	float frequency;
	glm::vec3 angle;

	int stackPosition;
	CTexGen* currentTexGen;
	std::vector<CTexGen*> texGens;
	CRenderTexture* currentTarget;
	//std::vector<CRenderTexture*> targets;


	int textureSize;
	CNullTex nullTexGen;
};