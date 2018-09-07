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

	CRenderer* pRenderer;
	CShader* shader;
	CBuf targetQuad;
	glm::mat4 ortho;
	unsigned int hOrthoMatrix;
	CRenderTexture * mTarget;
	CRenderTexture * mSource;
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
	CNoiseTex() {};
	void loadShader();
	void render();

	unsigned int hSamplePos;
	unsigned int hSampleSize;
	unsigned int hOctaves;
};


class CColourTex : public CTexGen {
public:
	CColourTex() {
		palette.resize(256, 0); };
	void loadShader();
	void render();

	CRenderTexture palette;
	unsigned int hSource;
	unsigned int hPalette;
};