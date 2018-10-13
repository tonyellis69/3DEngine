#pragma once

#include <vector>

#include "..\terrain\shell.h"

#include "..\direction.h"


class CTerrain2 {
public:
	CTerrain2() : playerOffset(0) {};
	void createLoD1shell(float _LoD1cubeSize, int chunkCubes, int SCchunks, int shellSCs);
	void addShell(int extent);
	float getShellSize(unsigned int shellNo);
	void playerWalk(glm::vec3& move);

	std::vector<CShell> shells;

	float LoD1cubeSize; //<In worldspace.
	float LoD1chunkSize; //<In worldspace.
	int chunkCubes;	//<cubes per chunk edge.
	int SCchunks;  //<SC size in chunks.

	glm::vec3 playerOffset; //<Deterimines if player moved far enough to advance scenery.

};