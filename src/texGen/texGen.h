#pragma once

//#include "..\renderer\renderTexture.h"
#include "..\renderer\renderer.h"
#include "..\colourGradient.h"

#include <iostream>
#include <fstream>
#include <random>

enum TexGenType { texNoise, texColour, texRidged, texCylinder, texTurbulence,
	texScaleBias, texAdd, texSeamless, texScalePoint, texBillow, texVoronoi,
	texSelect, texLayer, texNull, texGaus, texRects, texBlocks, texCover,
	texCutup, texTerrain
};

/** A base class for creating texture generators. */
class CTexGen {
public:
	CTexGen() {};
	CTexGen(TexGenType derivedType)  ;
	~CTexGen();
	void setTarget(CRenderTexture* newTarget);
	CRenderTexture* getTarget();
	virtual void loadShader() {};
	virtual void render() {};
	virtual void setSource(CTexGen* newSource);
	virtual void setSource2(CTexGen* newSource);
	virtual void setSource3(CTexGen* newSource);
	virtual void setFrequency(float freq) {};
	virtual void setPersistence(float persist) {};
	virtual void setAngles(glm::vec3& rotationAngles);
	virtual void setTranslation(glm::vec3& translation);
	virtual void setOctaves(int octave) {};
	virtual void setPower(float power) {};
	virtual void setPalette(ColourGradient& gradient) {};
	virtual void setScale(float scale) {};
	virtual void setBias(float bias) {};
	virtual void setDistance(bool onOff) {};
	virtual void setRandomHue(bool onOff) {};
	virtual void setScalePt(glm::vec3& scale) { };
	virtual void setLowerBound(float lower) {};
	virtual void setUpperBound(float upper) {};
	virtual void setFalloff(float falloff) {};
	virtual void setSampleWidth(float width) {};
	virtual void setSampleHeight(float falloff) {};
	virtual void setGausSize(int size) {};
	virtual void setGausSigma(float sigma) {};
	virtual void setPercentage(float percentage) {};
	void setRandomSeed(int seed) { randomSeed = seed; }

	virtual int getOctaves() { return 0; }
	virtual float getFrequency() { return 0; }
	virtual float getPersistence() { return 0; }
	virtual float getPower() { return 0; }
	virtual float getScale() { return 0; }
	virtual float getBias() { return 0; }
	virtual bool getDistance() { return 0; }
	virtual bool getRandomHue() { return 0; }
	virtual glm::vec3& getAngle() { return rotationAngles; }
	virtual glm::vec3& getTranslation() { return translation; }
	virtual ColourGradient* getColourGradient() { return NULL; }
	virtual glm::vec3& getScalePt() { return glm::vec3(0); }
	virtual float getLowerBound() { return 0; }
	virtual float getUpperBound() { return 0; }
	virtual float getFalloff() { return 0; };
	virtual float getSampleWidth() { return 0; }
	virtual float getSampleHeight() { return 0; }
	virtual int getGausSize() { return 0; };
	virtual float getGausSigma() { return 0; }
	virtual float getpercentageCtrl() { return 0; }
	int getRandomSeed() { return randomSeed; }

	virtual void compose() {};

	virtual void write(std::ofstream& out);
	virtual void read(std::ifstream& in);

	virtual glm::i32vec4 getSourceColour(int x, int y) { return glm::i32vec4(0); }

	CRenderer* pRenderer;
	CShader* shader;
	CBuf targetQuad;
	glm::mat4 matrix;
	glm::vec3 rotationAngles;
	glm::vec3 translation;
	unsigned int hMatrix;
	CRenderTexture mTarget;
	CRenderTexture * mSource;
	CRenderTexture * source2;
	CRenderTexture * source3;

	CTexGen* srcTex1;
	CTexGen* srcTex2;
	CTexGen* srcTex3;

	unsigned int hSource;

	TexGenType texGenType;
	std::string name;
	int count;
	int randomSeed;
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
		frequency(1), CTexGen(texNoise) {
		name = "Noise";
	};
	void loadShader();
	void render();
	void setSample(glm::vec3& pos, glm::vec3& size);
	void setOctaves(int octaves);
	void setFrequency(float freq);
	void setPersistence(float persist);
	void setSampleWidth(float width);
	void setSampleHeight(float height);
	void compose();

	int getOctaves() { return octaves; }
	float getFrequency() { return frequency; }
	float getPersistence() { return persistence; }
	float getSampleWidth() { return sampleSize.x; }
	float getSampleHeight() { return sampleSize.y; }

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
	glm::i32vec4 getSourceColour(int x, int y);
	void setSelectedShade(int shade);
	void adjustFalloff(int delta);

	ColourGradient colourGradient;
	CRenderTexture palette;
	unsigned int hPalette;

	int selectedShade;
	unsigned int hSelectedShade;

	float selectionFalloff;
	unsigned int hSelectionFalloff;

};

class CRidgedMultiTex : public CNoiseTex {
public:
	CRidgedMultiTex() { texGenType = texRidged; name = "RidgedMF"; }
	void loadShader();
	void render();



};

class CylinderTex : public CTexGen {
public:
	CylinderTex() : CTexGen(texCylinder), frequency(1) { name = "Cylinder"; };
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
		power(0.1f) {
		texGenType = texTurbulence; name = "Turbulence";
	}
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
	CScaleBiasTex() : CTexGen(texScaleBias), scale(1), bias(0) { name = "ScaleBias"; };
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
	CAddTex() : CTexGen(texAdd) { name = "Add";  }
	void loadShader();
	void render();

	unsigned int hSource2;


};

class CSeamlessTex : public CTexGen {
public:
	CSeamlessTex() : CTexGen(texSeamless), falloff(0.5f), percentage(10) { name = "Seamless";  }
	float getpercentageCtrl() { return percentage; }
	float getFalloff() { return falloff;  }
	void setPercentage(float percentage);
	void setFalloff(float falloff);

	void loadShader();
	void render();
	void write(std::ofstream & out);
	void read(std::ifstream& in);

	float falloff;
	float percentage;

	unsigned int hFalloff;
	unsigned int hPercentage;
};

class CScalePointTex : public CTexGen {
public:
	CScalePointTex() : CTexGen(texScalePoint), scale(1) { name = "ScalePoint"; }
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
	CBillowTex() { texGenType = texBillow; name = "Billow"; }
	void loadShader();
	void render();
	void setPersistence(float persistence);



};

class CVoronoiTex : public CNoiseTex {
public:
	CVoronoiTex() : 
		distance(true), randomHue(false) {
		texGenType = texVoronoi; frequency = 8; name = "Voronoi";
	}
	void loadShader();
	void render();
	void setDistance(bool onOff);
	void setRandomHue(bool onOff);
	void setSample(glm::vec3& pos, glm::vec3& size);

	bool getDistance() { return distance; }
	bool getRandomHue() { return randomHue; }

	void write(std::ofstream & out);
	void read(std::ifstream& in);


	unsigned int hDistance;
	unsigned int hRandomHue;

	bool distance;
	bool randomHue;

};

class CSelectTex : public CTexGen {
public:
	CSelectTex() : CTexGen(texSelect), lowerBound(0.2f), upperBound(0.8f),
		falloff(0.5f) {
		name = "Select"; 
	};
	void loadShader();
	void render();
	//void setSource3(CTexGen* map);
	void setBounds(float lower, float upper);
	void setLowerBound(float lower);
	void setUpperBound(float upper);
	void setFalloff(float falloff);

	float getLowerBound() { return lowerBound; }
	float getUpperBound() { return upperBound; }
	float getFalloff() { return falloff; }

	void write(std::ofstream& out);
	void read(std::ifstream& in);

	unsigned int hSource2;
	unsigned int hMap;
	unsigned int hLowerBound;
	unsigned int hUpperBound;
	unsigned int hFalloff;


	
	float lowerBound;
	float upperBound;
	float falloff;
};


class CLayerTex : public CAddTex {
public:
	CLayerTex() { texGenType = texLayer; name = "Layer"; }
	void loadShader();
	void render();


};

class CNullTex : public CTexGen {
public:
	CNullTex();

	CRenderTexture nullTexture;

};

class CGausTex : public CTexGen {
public:
	CGausTex();
	void loadShader();
	void render();

	void write(std::ofstream & out);
	void read(std::ifstream& in);

	void setGausSize(int size);
	void setGausSigma(float sigma);

	int getGausSize() { return size; }
	float getGausSigma() { return sigma; }

	int size;
	float sigma;

	unsigned int hSigma;
	unsigned int hSize;
};

class CRectsTex : public CTexGen {
public:
	CRectsTex();
	void loadShader();
	void render();

	void write(std::ofstream & out);
	void read(std::ifstream& in);

	void setDepth(int depth) {this->depth = depth;}
	void setVariance(float variance) { this->variance = variance; }
	void setOctaves(int octaves) { this->octaves = octaves; }
	void setFrequency(float freq) { this->frequency = freq; }
	void setStyle(int style) { this->style = style; }
	void setClusters(float clusters) { this->clusters = clusters; }

	int getDepth() { return depth; }
	float getVariance() { return variance; }
	int getOctaves() { return octaves; }
	float getFrequency() { return frequency; }
	int getStyle() { return style; }
	float getClusters() { return clusters; }


	int depth;
	float variance;
	float frequency;
	int octaves;
	int style;
	float clusters;

	unsigned int hDepth;
	unsigned int hVariance;
	unsigned int hFreq;
	unsigned int hOctaves;
	unsigned int hStyle;
	unsigned int hClusters;

};

class CBlocksTex : public CTexGen {
public:
	CBlocksTex();
	void loadShader();
	void render();

	void write(std::ofstream & out);
	void read(std::ifstream& in);

	void setStages(int stages) { this->stages = stages; }
	void setIterations(int iterations) { this->iterations = iterations; }
	void setDensity(float density) { this->density = density; }
	void setScale(float scale) { this->scale = scale; }

	int getStages() { return stages; }
	int getIterations() { return iterations; }
	float getDensity() { return density; }
	float getScale() { return scale; }

	int stages;
	int iterations;
	float density;
	float scale;

	unsigned int hStages;
	unsigned int hIterations;
	unsigned int hDensity;
	unsigned int hScale;
};

class CoverTex : public CTexGen {
public:
	CoverTex();
	void loadShader();
	void render();

	void write(std::ofstream & out);
	void read(std::ifstream& in);

	void setIterations(int iterations) { this->iterations = iterations; }
	void setScale(float scale) { this->scale = scale; }

	int getIterations() { return iterations; }
	float getScale() { return scale; }

	int  iterations;
	float scale;

	unsigned int hIterations;
	unsigned int hScale;
};


class CutupTex : public CTexGen {
public:
	CutupTex();
	void loadShader();
	void render();


	unsigned int hSource;
	unsigned int hSource2;
};

class CTerrainTex : public CTexGen {
public:
	CTerrainTex();
	void loadShader();
	void render();
	void uploadGridData();
	void placeStartEnd();
	void placeFeatures();
	void placeBarrier();
	void plotShortestPath();

	void setGridSize(int size);
	void setDetails(bool onOff) { showDetails = onOff; }
	void setFeatureCount(int count) { featureCount = count; }
	void setFeatureSize(float size) { featureSize = size; }

	int getGridSize() { return gridSize; }
	bool getDetails() { return showDetails; }
	int getFeatureCount() { return featureCount; }
	float getFeatureSize() { return featureSize; }

	void write(std::ofstream & out);
	void read(std::ifstream& in);

	glm::vec2 getStartPoint();
	glm::vec2 getEndPoint();



	std::vector<float> grid;
	int gridSize;
	bool showDetails;
	int featureCount;
	float featureSize;

	unsigned int hShowDetails;
	unsigned int hGridSize;

	CRenderTexture gridTexture;
	std::mt19937 eng;

	glm::i32vec2 startPoint, endPoint;
	std::vector<glm::i32vec2> features;

	int margin;
	float startOffset;
	float endOffset;
};