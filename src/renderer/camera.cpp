#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/perpendicular.hpp>

CCamera::CCamera() : C3dObject() {
	//set up some default perspective settings
	zNear = 0.1f; zFar = 90000.0f; Fov = 45.0f;
	lookAt(glm::vec3(0, 0, -1));
	setAspectRatio(800,600);
}
	
CCamera::CCamera(glm::vec3& pos) : C3dObject(pos) {
	//set up some default perspective settings
	zNear =  0.3f; zFar = 90000.0f; Fov = 45.0f;
	setAspectRatio(800,600);
}


/** Aim camera at the given positionHint.*/
void CCamera::lookAt(glm::vec3& target) {
	target = glm::normalize(target);

	//orientate by angle between lookat axis and target
	orientation = glm::rotation(glm::vec3(0,0,-1),target);

	keepUpright(target);

	orientateWorldMatrix();
	createClipMatrix();
}

/** Recreate the perspective transform based on the given aspect ratio. */
void CCamera::setAspectRatio(float w, float h) {
	perspectiveMatrix = glm::perspective<float>(Fov,w/h,zNear,zFar);
	createClipMatrix();
	viewW = w; viewH = h;
}

/** Reposition camera, moving the target proportionally.*/
void CCamera::setPos(glm::vec3& pos)  {
	glm::vec3 translation = pos - getPos();
	C3dObject::setPos(pos);
	createClipMatrix();
}

void CCamera::translate(glm::vec3& dir)  {
	C3dObject::translate(dir);
	createClipMatrix();
}


/** Create a transformation to clip space based on this camera's position, orientation and perspective.*/
void CCamera::createClipMatrix() {
	camMatrix = glm::inverse(worldMatrix);
	clipMatrix = perspectiveMatrix * camMatrix;
}


void CCamera::yaw(float angle) {
	rotate(angle,glm::vec3(0,1,0)); //ajusts orientation quat
	createClipMatrix();
}

void CCamera::pitch(float angle) {
	rotate(angle,glm::vec3(1,0,0)); //ajusts orientation quat
	createClipMatrix();
}

/** Redirect camera by moving the target vector.*/
void CCamera::adjustTarget(glm::vec2 inc) {

	glm::vec3 normInc = glm::normalize(glm::vec3(inc,1));

	orientation = glm::rotate(orientation,normInc.y,glm::vec3(1,0,0));
	orientation = glm::rotate(orientation,-normInc.x,glm::vec3(0,1,0));

	glm::vec3 dir(0,0,-1);
	dir = orientation * dir;

	keepUpright(dir);

	orientateWorldMatrix();

	createClipMatrix();
}

/** Ensure the orientation remains free of any roll while pointed at the target. */
void CCamera::keepUpright(const glm::vec3& target) {
	glm::vec3 desiredUp(0,1,0);
	glm::vec3 right = glm::cross(target, desiredUp);
	desiredUp = glm::cross(right, target);
	desiredUp = glm::normalize(desiredUp);
	
	glm::vec3 currentUp = orientation * glm::vec3(0.0f, 1.0f, 0.0f);
	currentUp = glm::normalize(currentUp);
	glm::quat adjustment = glm::rotation(currentUp, desiredUp);

	orientation = adjustment * orientation;
}


/** Move in direction of target. */
void CCamera::dolly(float dist) {
	glm::vec3 pos = getPos();
	glm::vec3 dir = glm::vec3(0,0,-1);
	dir = orientation * dir;
	dir = dir * dist;
	pos += dir;
	setPos(pos);
}

glm::vec3 CCamera::getTargetDir() {
	glm::vec3 dir = glm::vec3(0, 0, -1);
	return orientation * dir;
}


/** Move perpendicular to target. */
void CCamera::track(float dist) {
	//find target vector
	glm::vec3 pos = getPos();
	glm::vec3 dir = glm::vec3(1,0,0);
	dir = orientation * dir;
	dir = dir * dist;
	pos += dir;
	setPos(pos);
}

/** Move vertically. */
void CCamera::elevate(float dist) {
	glm::vec3 pos = getPos();
	glm::vec3 dir = glm::vec3(0,1,0);
	dir = orientation * dir;
	dir = dir * dist;
	pos += dir;
	setPos(pos);
}

/** Rotate camera around the given axis, without roll. */
void CCamera::freeRotate(glm::vec3& axis, float& angle) {

	orientation = glm::rotate(orientation,glm::radians(angle),axis);

	glm::vec3 dir(0,0,-1);
	dir = orientation * dir;

	keepUpright(dir);

	orientateWorldMatrix();

	createClipMatrix();

}

/** Return the back plane of this camera's frustum. */
void CCamera::getBackPlane(glm::vec3 & planePos, glm::vec3 & planeNormal) {
	planePos = getPos();
	planeNormal = getTargetDir();
}

//TO DO: camera subclass of rotate must update the camera's clip matrix, because we've changed not just the
//camera object but how it changes the view


