#include "colour.h"


namespace col {

	float h = 0.5f;
	double golden_ratio_conjugate = 0.618033988749895;


	glm::vec3 randHue() {

		h += golden_ratio_conjugate;
		h = fmod(h, 1.0f);
		return glm::vec3(glm::rgbColor(glm::vec3(h * 360.0f, 0.45f, 0.95f)));
	}


}