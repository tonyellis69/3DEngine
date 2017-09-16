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

	
	unsigned int hShader; ///<OpenGL shader handle.

	const char* feedbackVaryings[10];

protected:
	CRenderer* pRenderer;
private:
	void loadFiles();
	unsigned int noFeedbackSettings;

};


