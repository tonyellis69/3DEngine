#pragma once

//#include "..\renderer\renderTexture.h"
#include "..\renderer\renderer.h"
#include "..\colourGradient.h"

#include <iostream>
#include <fstream>

enum TexGenType { texNoise, texColour, texRidged, texCylinder, texTurbulence,
	texScaleBias, texAdd, texSeamless, texScalePoint, texBillow, texVoronoi,
	texSelect, texLayer};

/** A base class for creating texture generators. */
class CTexGen {
public:
	CTexGen(TexGenType derivedType)  ;
	void setTarget(CRenderTexture* newTarget);
	CRenderTexture* getTarget();
	virtual void loadShader() {};
	virtual void render() {};
	virtual void setSource(CTexGen* newSource);
	virtual void setSource2(CTexGen* newSource);
	virtual void setFrequency(float freq) {};
	virtual void setAngles(glm::vec3& rotationAngles);
	virtual void setTranslation(glm::vec3& translation);
	virtual void setOctaves(int octave) {};
	virtual void setPower(float power) {};
	virtual void setPalette(ColourGradient& gradient) {};
	virtual void setScale(float scale) {};
	virtual void setBias(float bias) {};
	virtual void setScalePt(glm::vec3& scale) { };

	virtual int getOctaves() { return 0; }
	virtual float getFrequency() { return 0; }
	virtual float getPower() { return 0; }
	virtual float getScale() { return 0; }
	virtual float getBias() { return 0; }
	virtual glm::vec3& getAngle() { return rotationAngles; }
	virtual glm::vec3& getTranslation() { return translation; }
	virtual ColourGradient* getColourGradient() { return NULL; }
	virtual glm::vec3& getScalePt() { return glm::vec3(0); }

	virtual void compose() {};

	virtual void write(std::ofstream& out);
	virtual void read(std::ifstream& in);

	CRenderer* pRenderer;
	CShader* shader;
	CBuf targetQuad;
	glm::mat4 matrix;
	glm::vec3 rotationAngles;
	glm::vec3 translation;
	unsigned int hMatrix;
	CRenderTexture * mTarget;
	CRenderTexture * mSource;
	CRenderTexture * source2;

	CTexGen* srcTex1;
	CTexGen* srcTex2;

	unsigned int hSource;

	TexGenType texGenType;
private:
	void buildMatrix();

	
};

// Convenience function for writing simple objects to files.
template<class T, class S>
void writeObject(const T& to_write, S& out)
{
	out.write(reinterpret_cast<const char*>(&to_write), sizeof(T));
}

template<class T, class S>
std::istream& readObject(T& to_read, S& in) {
	return in.read(reinterpret_cast<char*>(&to_read), sizeof(T));
}


class CNoiseTex : public CTexGen {
public:
	CNoiseTex() : samplePos(0), sampleSize(2,2,0) , octaves(1), persistence(0.5),
		frequency(1), CTexGen(texNoise) {};
	void loadShader();
	void render();
	void setSample(glm::vec3& pos, glm::vec3& size);
	void setOctaves(int octaves);
	void setFrequency(float freq);
	void compose();

	int getOctaves() { return octaves; }
	float getFrequency() { return frequency; }

	void write(std::ofstream & out);
	void read(std::ifstream& in);

	unsigned int hSamplePos;
	unsigned int hSampleSize;
	unsigned int hOctaves;
	unsigned int hPersistence;
	unsigned int hFrequency;

	glm::vec3 samplePos;
	glm::vec3 sampleSize;
	int octaves;
	float persistence;
	float frequency;
};


class CColourTex : public CTexGen {
public:
	CColourTex();
	void setPalette(ColourGradient& newGradient);
	void loadShader();
	void render();

	void write(std::ofstream & out);
	void read(std::ifstream& in);

	ColourGradient* getColourGradient() { return &colourGradient; }

	ColourGradient colourGradient;
	CRenderTexture palette;
	unsigned int hPalette;


};

class CRidgedMultiTex : public CNoiseTex {
public:
	CRidgedMultiTex() { texGenType = texRidged; }
	void loadShader();
	void render();



};

class CylinderTex : public CTexGen {
public:
	CylinderTex() : CTexGen(texCylinder), frequency(1) {};
	void loadShader();
	void render();
	void setFrequency(float freq);

	void write(std::ofstream & out);
	void read(std::ifstream& in);

	unsigned int hFrequency;
	float frequency;

	float getFrequency() { return frequency; }
};

class CTurbulenceTex : public CNoiseTex {
public:
	CTurbulenceTex() : samplePos(0), sampleSize(1, 1, 0),
		power(0.1f) { texGenType = texTurbulence; }
	void loadShader();
	void render();
	void setPower(float power);
	void setOctaves(int octave);
	void setSample(glm::vec3& pos, glm::vec3& size);
	
	float getPower() { return power; }

	void write(std::ofstream & out);
	void read(std::ifstream& in);

	unsigned int hPower;
	unsigned int hRoughness;
	unsigned int hSamplePos;
	unsigned int hSampleSize;

	float power;


	glm::vec3 samplePos;
	glm::vec3 sampleSize;
};

class CScaleBiasTex : public CTexGen {
public:
	CScaleBiasTex() : CTexGen(texScaleBias), scale(1), bias(0)  {};
	void loadShader();
	void setScaleBias(float scale, float bias);
	void setScale(float scale);
	void setBias(float bias);
	void render();

	void write(std::ofstream & out);
	void read(std::ifstream& in);

	float getScale() { return scale; }
	float getBias() { return bias; }

	unsigned int hScale;
	unsigned int hBias;

	float scale;
	float bias;
};

class CAddTex : public CTexGen {
public:
	CAddTex() : CTexGen(texAdd) {}
	void loadShader();
	void render();

	unsigned int hSource2;


};

class CSeamlessTex : public CTexGen {
public:
	CSeamlessTex() : CTexGen(texSeamless) {}
	void loadShader();
	void render();
};

class CScalePointTex : public CTexGen {
public:
	CScalePointTex() : CTexGen(texScalePoint), scale(1) {}
	void setScalePt(glm::vec3& scale);
	void loadShader();
	void render();

	void write(std::ofstream & out);
	void read(std::ifstream& in);

	glm::vec3& getScalePt();
	unsigned int hScale;
	glm::vec3 scale;
};

class CBillowTex : public CNoiseTex {
public:
	CBillowTex() { texGenType = texBillow;  }
	void loadShader();
	void render();
	void setPersistence(float persistence);



};

class CVoronoiTex : public CTexGen {
public:
	CVoronoiTex() : CTexGen(texVoronoi), samplePos(0), sampleSize(2, 2, 0),
	distance(true), randomHue(false) {};
	void loadShader();
	void render();
	void setDistance(bool onOff);
	void setRandomHue(bool onOff);
	void setSample(glm::vec3& pos, glm::vec3& size);
	void setFrequency(float freq);

	unsigned int hSamplePos;
	unsigned int hSampleSize;
	unsigned int hDistance;
	unsigned int hRandomHue;
	unsigned int hFrequency;

	glm::vec3 samplePos;
	glm::vec3 sampleSize;
	bool distance;
	bool randomHue;
	float frequency;
};

class CSelectTex : public CTexGen {
public:
	CSelectTex() : CTexGen(texSelect) {}
	void loadShader();
	void render();
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
	CLayerTex() { texGenType = texLayer;  }
	void loadShader();
	void render();


};