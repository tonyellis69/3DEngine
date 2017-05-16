#pragma once

#include "shader.h"

//#include "renderer.h"
using namespace std;

class CRenderer;
class CRenderShader : public CShader {
public:
	CRenderShader() {};
	void load(shaderType shader, std::string shaderFile);
	void create(std::string shaderName);
	void attach();
	void setFeedbackData(int nVars, const char** strings);
	void link();
	void getDataHandles();
	CShader * getThisShader();
	unsigned int getShaderHandle();

	CRenderer* pRenderer;
	unsigned int hShader; ///<OpenGL shader handle.
};


