#pragma once
#include "renderer\3dobject.h"
#include "renderer\model.h"
#include "renderer\camera.h"

//TO DO: rather than C3dObject, create a 3d subclass that can carry
// and reposition other 3d objects.

/** Basic player object. */
class CPlayerObj : public C3dObject  {
public:
	CPlayerObj();
	void updatePov();
	void setPos(glm::vec3& pos);

	float width;
	float height;
	glm::vec3 eyeOffset;

	CModel* pModel;
	CCamera povCam;
};