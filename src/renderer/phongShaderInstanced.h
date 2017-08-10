#pragma once

#include <glew.h>
#include <glm/glm.hpp>
#include "renderShader.h"



/** A wrapper for the instanced phong shader. */
class CPhongShaderInstanced : public CRenderShader {
public:
	CPhongShaderInstanced() {};
	void getShaderHandles();
	//unsigned int getMVPhandle();
	void setNormalModelToCameraMatrix(glm::mat3& matrix);
	void setLightDirection(glm::vec3& lightDir);
	void setLightIntensity(glm::vec4 lightIntensity);
	void setAmbientLight(glm::vec4 ambient);
	void setColour (glm::vec4& colour);
	void setMVP(glm::mat4 matrix);


	GLuint hNormalModelToCameraMatrix;
	GLuint hLightDirection;
	GLuint hLightIntensity;
	GLuint hAmbientLight;
	GLuint hColour;

	GLuint hMVP; ///<Handle to standard texture shader MVP matrix

	};