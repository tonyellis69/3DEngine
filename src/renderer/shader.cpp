#include "shader.h"

TStandardShaderType CShader::getType() {
	return ident;
}

void CShader::setType(TStandardShaderType ident) {
	CShader::ident = ident;
}
