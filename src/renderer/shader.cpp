#include "shader.h"

TStandardShaderType CShaderOld::getType() {
	return ident;
}

void CShaderOld::setType(TStandardShaderType ident) {
	CShaderOld::ident = ident;
}
