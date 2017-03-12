#pragma once

#include "3dobject.h"


/** Class for 3d light source. */
class CLight : public C3dObject {
public:
	CLight();
	CLight(glm::vec3& pos);
	glm::vec3 direction; ///<Direction of light.
	
	glm::vec4 intensity; ///<Colour composition.
	
};