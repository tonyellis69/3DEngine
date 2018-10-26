#pragma once

//#include "..\renderer\renderTexture.h"
#include "..\renderer\renderer.h"
#include "..\colourGradient.h"

/** A base class for creating texture generators. */
class CTexGen {
public:
	CTexGen() ;
	void setTarget(CRenderTexture* newTarget);
	CRenderTexture* getTarget();
	virtual void loadShader() {};
	virtual void render() {};
	virtual void setSource(CRenderTexture* newSource);
	virtual void setSource2(CRenderTexture* newSource) {};
	virtual void setFrequency(float freq);
	virtual void setAngles(glm::vec3& rotationAngles);
	virtual void setTranslation(glm::vec3& translation);
	virtual void setOctaves(int octave) {};
	virtual void setPower(float power) {};
	virtual void setPalette(ColourGradient& gradient) {};
	virtual void setScale(float scale) {};
	virtual void setBias(float bias) {};

	virtual void compose() {};

	CRenderer* pRenderer;
	CShader* shader;
	CBuf targetQuad;
	glm::mat4 matrix;
	unsigned int hMatrix;
	CRenderTexture * mTarget;
	CRenderTexture * mSource;

	float frequency;
	unsigned int hFrequency;
	unsigned int hSource;
private:


	
};



class CTestTex : public CTexGen {
public:
	CTestTex() {};
	void loadShader();
	void render();
};

class CNoiseTex : public CTexGen {
public:
	CNoiseTex() : samplePos(0), sampleSize(2,2,0) , octaves(1), persistence(0.5) {};
	void loadShader();
	void render();
	void setSample(glm::vec3& pos, glm::vec3& size);
	void setOctaves(int octaves);
	void compose();

	unsigned int hSamplePos;
	unsigned int hSampleSize;
	unsigned int hOctaves;
	unsigned int hPersistence;

	glm::vec3 samplePos;
	glm::vec3 sampleSize;
	int octaves;
	float persistence;
};


class CColourTex : public CTexGen {
public:
	CColourTex();
	void setPalette(ColourGradient& newGradient);
	void loadShader();
	void render();

	CRenderTexture palette;
	unsigned int hPalette;


};

class CRidgedMultiTex : public CNoiseTex {
public:
	void loadShader();
	void render();
};

class CylinderTex : public CTexGen {
public:
	void loadShader();
	void render();
};

class CTurbulenceTex : public CTexGen {
public:
	CTurbulenceTex() : samplePos(0), sampleSize(1, 1, 0),
		power(0.1f), octaves(1) {};
	void loadShader();
	void render();
	void setPower(float power);
	void setOctaves(int octave);
	void setSample(glm::vec3& pos, glm::vec3& size);
	

	unsigned int hPower;
	unsigned int hRoughness;
	unsigned int hSamplePos;
	unsigned int hSampleSize;

	float power;
	int octaves;

	glm::vec3 samplePos;
	glm::vec3 sampleSize;
};

class CScaleBiasTex : public CTexGen {
public:
	CScaleBiasTex() : scale(1), bias(0) {};
	void loadShader();
	void setScaleBias(float scale, float bias);
	void setScale(float scale);
	void setBias(float bias);
	void render();

	unsigned int hScale;
	unsigned int hBias;

	float scale;
	float bias;
};

class CAddTex : public CTexGen {
public:
	void setSource2(CRenderTexture* src2);
	void loadShader();
	void render();

	
	unsigned int hSource2;

	CRenderTexture * source2;
};

class CSeamlessTex : public CTexGen {
public:
	void loadShader();
	void render();
};

class CScalePointTex : public CTexGen {
public:
	void setScale(glm::vec3& scale);
	void loadShader();
	void render();

	unsigned int hScale;
	glm::vec3 scale;
};

class CBillowTex : public CNoiseTex {
public:
	void loadShader();
	void render();
	void setPersistence(float persistence);



};

class CVoronoiTex : public CTexGen {
public:
	CVoronoiTex() : samplePos(0), sampleSize(2, 2, 0),
	distance(true), randomHue(false) {};
	void loadShader();
	void render();
	void setDistance(bool onOff);
	void setRandomHue(bool onOff);
	void setSample(glm::vec3& pos, glm::vec3& size);

	unsigned int hSamplePos;
	unsigned int hSampleSize;
	unsigned int hDistance;
	unsigned int hRandomHue;

	glm::vec3 samplePos;
	glm::vec3 sampleSize;
	bool distance;
	bool randomHue;
};

class CSelectTex : public CTexGen {
public:
	void loadShader();
	void render();
	void setSource2(CRenderTexture* src2);
	void setControl(CRenderTexture* map);
	void setBounds(float lower, float upper);
	void setFalloff(float falloff);

	unsigned int hSource2;
	unsigned int hMap;
	unsigned int hLowerBound;
	unsigned int hUpperBound;
	unsigned int hFalloff;

	CRenderTexture* source2;
	CRenderTexture* map;
	float lowerBound;
	float upperBound;
	float falloff;
};


class CLayerTex : public CAddTex {
public:
	
	void loadShader();
	void render();


};