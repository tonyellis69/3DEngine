#include "renderMaterial.h"


void CRenderMaterial::setTexure(std::string filename) {
	//load a file
	//turn it into an OpenGL texture
	hTexture = pRenderer->textureManager.getTexture(filename);


}

void CRenderMaterial::assign() {
	//default action: just draw using given colour.
	pRenderer->setShaderValue(pRenderer->hColour, 1, colour);
}

void CRenderMaterial::setColour(glm::vec4 & iColour) {
	colour = iColour;
}
