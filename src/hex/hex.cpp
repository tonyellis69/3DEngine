#include "hex.h"

/** Return position in axial coordiantes. */
glm::i32vec2 CHex::getAxial() {
	return glm::i32vec2(x,z);
}
