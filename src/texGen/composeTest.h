#pragma once

#include "compositor.h"

class ComposeTest : public Compositor {
public:
	ComposeTest() : currentTexGen(&nullTexGen) {
		fileVersion = -1;
	};
	~ComposeTest();
	void initTex();
	void compose();
	CTexGen* createNoiseTex();
	CTexGen* createRidgedMFTex();
	CTexGen* createCylinderTex();
	CTexGen* createBillowTex();
	CTexGen* createVoronoiTex();
	CTexGen* createTurbulenceTex();
	CTexGen* createColouriseTex();
	CTexGen* createScaleBiasTex();
	CTexGen * createSeamlessTex();
	CTexGen* createScalePointTex();
	CTexGen* createaddTex();
	CTexGen* createLayerTex();
	CTexGen* createSelectTex();
	CTexGen* createGausTex();
	CTexGen* createRectsTex();
	CTexGen * createBlocksTex();
	CTexGen * createCoverTex();
	CTexGen * createCutupTex();
	CTexGen * createTerrainTex();

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

	void save(std::string filename);
	void restore(std::string filename);

	CRenderTexture& getComposedTexture();

	void saveCurrentTexGenPNG(std::string filename);

	glm::i32vec2 getStartPoint();
	glm::i32vec2 getEndPoint();

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

	int fileVersion;

};