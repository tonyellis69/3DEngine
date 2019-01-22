#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

/** Basic class for 3d objects. */
class C3dObject {
public:
	C3dObject() { setPos(glm::vec3(0,0,0)); };
	C3dObject(glm::vec3& pos);
	virtual void setPos(glm::vec3& pos);
	void translate(glm::vec3& dir);
	glm::vec3 getPos();
	void rotate(float angle,glm::vec3 axis);
	virtual void orientateWorldMatrix();

	glm::mat4 worldMatrix; ///<Transforms this object to a place and orientation in world space.

	glm::fquat orientation;

};