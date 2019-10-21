#include "3dobject.h"
#include <glm/gtc/matrix_transform.hpp>	
#include <glm/gtc/matrix_access.hpp>

using namespace glm;

/** Create an object with a world matrix positioning it at the given location.*/
C3dObject::C3dObject(glm::vec3& pos) {
	worldMatrix = glm::translate(glm::mat4(1),pos);	
	//orientation = glm::fquat(0,0,0,0);
}

/** Position the object in world space. */
void C3dObject::setPos(glm::vec3& pos) {
	worldMatrix[3] = glm::vec4(pos,1);
}

/** Move relative to current position. */
void C3dObject::translate(vec3& dir) {
	worldMatrix[3] += vec4(dir,0); 
}

/** Return the object's positionHint in world space. */
glm::vec3 C3dObject::getPos() {
	return glm::vec3(glm::column(worldMatrix,3));
}

///////////////////
//TO DO: why don't I just use the glm quaterion rotate (rotate(q,angle,axis)) to do this???
/** Rotate object about the given axis. */
void C3dObject::rotate(float angle,glm::vec3 axis) {
	//create a quaternion of the rotation
	angle = glm::radians(angle);
	axis = glm::normalize(axis);
	axis = axis * sinf(angle / 2.0f);
	float scalar = cosf(angle / 2.0f);
	glm::fquat offset(scalar, axis.x, axis.y, axis.z);

	
	//rotate the object's existing orientation quat by it
	orientation = offset * orientation;
	orientation = glm::normalize(orientation);

	//set the object's matrix to this orientation
	orientateWorldMatrix();
}

/** Update world matrix with current orientation. */
void C3dObject::orientateWorldMatrix() {
	//glm::vec3 pos = getPos();
	//worldMatrix =  glm::mat4_cast(orientation);

	mat4 rotationMatrix = glm::mat4_cast(orientation);

	mat4 transM = glm::translate(mat4(1), getPos());

	worldMatrix = transM * rotationMatrix ;

}

