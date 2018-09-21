#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/color_space.hpp>
#include <stdlib.h>


namespace col {
	glm::vec3 randHue();
	glm::vec4 byteToFloat(glm::i32vec4 colour255);

}