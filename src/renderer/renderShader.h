#pragma once

#include "shader.h"

//#include "renderer.h"
using namespace std;

class CRenderer;
class CRenderShader : public CShader {
public:
	CRenderShader();
	void load(shaderType shader, std::string shaderFile);
	void create(std::string shaderName);
	void attach();
	void setFeedbackData(unsigned int nVars);
	void link();
	void getDataHandles();
	CShader * getThisShader();
	unsigned int getShaderHandle();
	void recompile();
	unsigned int getUniformHandle(std::string name);

	void setShaderValue(unsigned int matrixHandle, glm::mat4& matrix);
	void setShaderValue(unsigned int matrixHandle, glm::mat3& matrix);
	void setShaderValue(unsigned int vecHandle, glm::vec2& vector);
	void setShaderValue(unsigned int vecHandle, glm::vec3& vector);
	void setShaderValue(unsigned int vecHandle, glm::vec4& vector);
	void setShaderValue(unsigned int intHandle, int value);
	void setShaderValue(unsigned int floatHandle, float value);

	
	unsigned int hShader; ///<OpenGL shader handle.

	const char* feedbackVaryings[10];

protected:
	CRenderer* pRenderer;
private:
	void loadFiles();
	unsigned int noFeedbackSettings;

};


