#include "light.h"

CLight::CLight() {
	C3dObject(glm::vec3(0,0,0));
	direction = glm::normalize(glm::vec3(0.866f, 0.9f, 0.5f));
	intensity = glm::vec4(0.8f,0.8f,0.8f,1);
}

CLight::CLight(glm::vec3& pos) :  C3dObject(pos) {
	//lightDir = glm::vec3(0.866f, 0.5f, 0.5f);

}
