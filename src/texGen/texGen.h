#pragma once

//#include "..\renderer\renderTexture.h"
#include "..\renderer\renderer.h"

/** A base class for creating texture generators. */
class CTexGen {
public:
	CTexGen();
	void setTarget(CRenderTexture* newTarget);
	virtual void loadShader() {};
	virtual void render() {};
	virtual void setSource(CRenderTexture* newSource);
	virtual void setFrequency(float freq);
	virtual void setAngles(glm::vec3 rotationAngles);

	CRenderer* pRenderer;
	CShader* shader;
	CBuf targetQuad;
	glm::mat4 matrix;
	unsigned int hMatrix;
	CRenderTexture * mTarget;
	CRenderTexture * mSource;

	float frequency;
	unsigned int hFrequency;
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
	CNoiseTex() : samplePos(0), sampleSize(4,4,0) , octaves(1) {};
	void loadShader();
	void render();
	void setSample(glm::vec3& pos, glm::vec3& size);
	void setOctaves(int octaves);

	unsigned int hSamplePos;
	unsigned int hSampleSize;
	unsigned int hOctaves;

	glm::vec3 samplePos;
	glm::vec3 sampleSize;
	int octaves;
};


class CColourTex : public CTexGen {
public:
	void setPalette(CRenderTexture* newPalette);
	void loadShader();
	void render();

	CRenderTexture* palette;
	unsigned int hSource;
	unsigned int hPalette;
};

class CRidgedMultiTex : public CNoiseTex {
public:
	void loadShader();
};

class CylinderTex : public CTexGen {
public:
	void loadShader();
	void render();

};

class CTurbulenceTex : public CTexGen {
public:
	void loadShader();
	void render();
	void setPower(float power);
	void setRoughness(int roughness);

	unsigned int hSource;
	unsigned int hPower;
	unsigned int hRoughness;

	float power;
	int roughness;
};

class CScaleBiasTex : public CTexGen {
public:
	void loadShader();
	void setScaleBias(float scale, float bias);
	void render();

	unsigned int hSource;
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

	unsigned int hSource;
	unsigned int hSource2;

	CRenderTexture * source2;
};