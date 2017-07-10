#pragma once

#include "shader.h"

//#include "renderer.h"
using namespace std;

class CRenderer;
class CRenderShader : public CShader {
public:
	CRenderShader() { noFeedbackSettings = 0; };
	void load(shaderType shader, std::string shaderFile);
	void create(std::string shaderName);
	void attach();
	void setFeedbackData(unsigned int nVars);
	void link();
	void getDataHandles();
	CShader * getThisShader();
	unsigned int getShaderHandle();
	void recompile();

	CRenderer* pRenderer;
	unsigned int hShader; ///<OpenGL shader handle.

	const char* feedbackVaryings[10];
private:
	void loadFiles();
	unsigned int noFeedbackSettings;
};


