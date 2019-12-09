#pragma once

#include <string>
#include "glm\glm.hpp"

#include "renderTexture.h"
#include "texManager.h"

enum shaderType { vertex, frag, geometry };

enum TStandardShaderType { standardTex, standardPhong, standardWire, standardMultiTex, standardBillboard,
	uiRect, uiTex, text, userShader};

/**	Encapsulates a shader, to conveniently gather the various data handles it needs
	and also hide OpenGL specifics from user. */


class CShader {
public:
	CShader() { ident = userShader; };
	~CShader() {};
	virtual void load(shaderType shader, std::string shaderFile) {};
	virtual void create(std::string shaderName) {};
	virtual void create(std::string shaderName, int nFeedbacks) {};
	virtual void attach() {};
	virtual void setFeedbackData(int nVars, const char** strings) {};
	virtual void link() {};
	virtual unsigned int getShaderHandle() { return 0; };
//	virtual unsigned int getMVPhandle() { return 0; };
	virtual void recompile() {};
	TStandardShaderType getType();
	void setType(TStandardShaderType ident);
//	virtual void getShaderHandles() {};
	virtual unsigned int getUniformHandle(std::string name) { return 0; };

	virtual void setShaderValue(unsigned int matrixHandle, glm::mat4& matrix) {};
	virtual void setShaderValue(unsigned int matrixHandle,  glm::mat3& matrix) {};
	virtual void setShaderValue(unsigned int vecHandle, glm::vec2& vector) {};
	virtual void setShaderValue(unsigned int vecHandle, glm::i32vec2& vector) {};
	virtual void setShaderValue(unsigned int vecHandle,  glm::vec3& vector) {};
	virtual void setShaderValue(unsigned int vecHandle, const glm::vec4& vector) {};
	virtual void setShaderValue(unsigned int intHandle,  int value) {};
	virtual void setShaderValue(unsigned int floatHandle,  float value) {};
	virtual void setShaderValue(unsigned int textureHandle, CBaseTexture& texture) {};
	virtual void setTextureUnit(unsigned int samplerHandle,  int textureUnit) {}
	TStandardShaderType ident;

	std::string vertFile;
	std::string geomFile;
	std::string fragFile;

	const char* feedbackVaryings[10];
	unsigned int noFeedbackSettings;
};

template <typename T>
class CShader2 : public CShader {

	T uniforms;
};