#pragma once

#include "renderer/model2.h"
#include "physics2/physObj2.h"

/** A class defining the implementationd of the 
	basic physics-enabled 3d model.*/
class CPhysModel : public CModel2, public CPhysObj2 {
public:
	CPhysModel() {};
	void updateModelPosition(glm::vec3& dPos);
	void scale(glm::vec3& scaleVec);
	glm::vec3 getPos();
	TAaBB calcAABB();
	glm::vec3 calcBaseVertPos();
};