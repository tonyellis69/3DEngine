#pragma once

#include "glm/glm.hpp"

class ITerrainCallback {
public:
	virtual bool scIntersectionCheckCallback(glm::vec3& pos, float scSize) { return false; };

};

/** A container for zero or more chunks occupying a cubic volume of space. */
class CSuperChunk2 {
public:
	CSuperChunk2() {};
	void checkForIntersection();
	void setSampleSpacePosition(glm::vec3& pos);
	void setSampleSize(float sampleSize);
	void setCallbackApp(ITerrainCallback* pApp);

	glm::vec4 colour; //TO DO probably temp
	glm::i32vec3 origIndex; //TO DO probably temp

	glm::vec3 sampleSpacePos;
	float sampleSize;
	static ITerrainCallback* pCallbackApp; ///<Pointer to the app used for callbacks.

	bool isEmpty; ///<True unless terrain intersects SC.
};

