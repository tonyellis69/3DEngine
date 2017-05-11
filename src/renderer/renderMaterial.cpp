#include "renderMaterial.h"


void CRenderMaterial::addImage(std::string filename) {
	CTextureSlot tex;
	tex.hTexture = pRenderer->textureManager.getTexture(filename);
	tex.tile = glm::vec2(1);
	tex.offset = glm::vec2(0);
	tex.textureUnit = textures.size();
	textures.push_back(tex);
}

unsigned int CRenderMaterial::getTexture(int texNo) {
	return textures[texNo].hTexture;
}

void CRenderMaterial::assign() {


	//default action: just draw using given colour.
	pRenderer->setShaderValue(pRenderer->hColour, 1, colour);

	//if you have a texture, use it. OR, use the texture unit no??
	for (size_t texNo = 0; texNo < textures.size(); texNo++) {
		pRenderer->attachTexture(texNo, textures[texNo].hTexture);
		pRenderer->setShaderValue(pRenderer->hTextureUnit[texNo], 1,(int) textures[texNo].textureUnit);
		pRenderer->setShaderValue(pRenderer->hTile[texNo], 1, textures[texNo].tile);
		pRenderer->setShaderValue(pRenderer->hOffset[texNo], 1, textures[texNo].offset);
	}


}

void CRenderMaterial::setColour(glm::vec4 & iColour) {
	colour = iColour;
}

void CRenderMaterial::setShader(unsigned int shader) {
	hShader = shader;
}

void CRenderMaterial::setOffset(int texNo,  glm::vec2 & offset) {
	textures[texNo].offset = offset;

}

void CRenderMaterial::setTile(int texNo, glm::vec2 & tile) {
	textures[texNo].tile = tile;
}
