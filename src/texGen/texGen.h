#pragma once

//#include "..\renderer\renderTexture.h"
#include "..\renderer\renderer.h"

/** A base class for creating texture generators. */
class CTexGen {
public:
	CTexGen() ;
	void setTarget(CRenderTexture* newTarget);
	virtual void loadShader() {};
	virtual void render() {};
	virtual void setSource(CRenderTexture* newSource);
	virtual void setFrequency(float freq);
	virtual void setAngles(glm::vec3& rotationAngles);
	virtual void setTranslation(glm::vec3& translation);

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
	void setPalette(CRenderTexture* newPalette);
	void loadShader();
	void render();

	CRenderTexture* palette;
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
	CTurbulenceTex() : samplePos(0), sampleSize(2, 2, 0) {};
	void loadShader();
	void render();
	void setPower(float power);
	void setRoughness(int roughness);
	void setSample(glm::vec3& pos, glm::vec3& size);

	unsigned int hPower;
	unsigned int hRoughness;
	unsigned int hSamplePos;
	unsigned int hSampleSize;

	float power;
	int roughness;

	glm::vec3 samplePos;
	glm::vec3 sampleSize;
};

class CScaleBiasTex : public CTexGen {
public:
	void loadShader();
	void setScaleBias(float scale, float bias);
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
	CVoronoiTex() : samplePos(0), sampleSize(2, 2, 0) {};
	void loadShader();
	void render();

	unsigned int hSamplePos;
	unsigned int hSampleSize;

	glm::vec3 samplePos;
	glm::vec3 sampleSize;

};

class CSelectTex : public CTexGen {
public:
	void loadShader();
	void render();
	void setSource2(CRenderTexture* src2);
	void setControl(CRenderTexture* map);
	void setBounds(float lower, float upper);

	unsigned int hSource2;
	unsigned int hMap;
	unsigned int hLowerBound;
	unsigned int hUpperBound;

	CRenderTexture* source2;
	CRenderTexture* map;
	float lowerBound;
	float upperBound;
};