#pragma once

#include "compositor.h"

class ComposeNoise : public Compositor {
public:
	void initTex();
	void compose();
	void updateOctaves(int octaves);
	void updateFrequency(float freq);

	int octaves;
	float frequency;
};