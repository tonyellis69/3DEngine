#pragma once

#include "shader.h"

//#include "renderer.h"
//using namespace std;

class CRenderer;
class CRenderShader : public CShader {
public:
	CRenderShader();
	void load(shaderType shader, std::string shaderFile);
	void create(std::string shaderName);
	void create(std::string shaderName, int nFeedbacks);
	void attach();
	void setFeedbackData(unsigned int nVars);
	void link();
	unsigned int getShaderHandle();
	void recompile();
	unsigned int getUniformHandle(std::string name);

	void setShaderValue(unsigned int matrixHandle, glm::mat4& matrix);
	void setShaderValue(unsigned int matrixHandle, glm::mat3& matrix);
	void setShaderValue(unsigned int vecHandle, glm::vec2& vector);
	void setShaderValue(unsigned int vecHandle, glm::i32vec2& vector);
	void setShaderValue(unsigned int vecHandle, glm::vec3& vector);
	void setShaderValue(unsigned int vecHandle, const glm::vec4& vector);
	void setShaderValue(unsigned int intHandle, int value);
	void setShaderValue(unsigned int floatHandle, float value);
	void setTextureUnit(unsigned int samplerHandle,  int textureUnit);


	void setShaderValue(unsigned int textureHandle, CBaseTexture & texture);

	
	unsigned int hShader; ///<OpenGL shader handle.



protected:
	CRenderer* pRenderer;
private:
	void loadFiles();


};


