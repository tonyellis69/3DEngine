#pragma once

#include "3dobject.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/norm.hpp>

/**Class for a 3d camera. */
class CCamera : public C3dObject {
public:
	CCamera();
	CCamera(glm::vec3& pos);
	void lookAt(glm::vec3& target);
	void setAspectRatio(float w, float h);
	void setPos(glm::vec3& pos);
	void adjustTarget(glm::vec2 inc);
	void yaw(float angle);
	void pitch(float angle);
	void dolly(float dist);
	glm::vec3 getTargetDir();
	void track(float dist);
	void elevate(float dist);
	void keepUpright(const glm::vec3& target);
	void createClipMatrix();
	void freeRotate(glm::vec3& axis, float& angle);

	glm::mat4 clipMatrix; ///<Transforms coordinates in clip space, ready for rasterizing.
	glm::mat4 perspectiveMatrix; ///<The perspective transform of this camera.

	glm::mat4 camMatrix; ///<Inverse transform of camera position.


private:


	float zNear;
	float zFar;
	float Fov;
	float viewW;
	float viewH;

};