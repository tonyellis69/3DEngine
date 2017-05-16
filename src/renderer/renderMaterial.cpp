#include "renderMaterial.h"


void CRenderMaterial::addImage(std::string filename) {
	CTextureSlot tex;
	tex.hTexture = pRenderer->textureManager.getTexture(filename);
	tex.tile = glm::vec2(1);
	tex.offset = glm::vec2(0);
//	tex.textureUnit = textures.size();
	//for now, let's try not to permanently attach textures to texture units
	textures.push_back(tex);
}

unsigned int CRenderMaterial::getTexture(int texNo) {
	return textures[texNo].hTexture;
}

void CRenderMaterial::assign() {
	


	pRenderer->setShader(shader);


	//TO DO: need to ask shader what kind it is
	//Better: do it when shader is attached, so we only do it once
	if (currentShaderType == standardPhong) {
		CPhongShader* subShader = (CPhongShader*)shader->getThisShader();
		subShader->setColour(colour);
	}
	
	if (currentShaderType == standardTex) {
		CTexShader* subShader = (CTexShader*)shader->getThisShader();

		//if we have textures, assign them
		for (size_t texNo = 0; texNo < textures.size(); texNo++) {
			pRenderer->attachTexture(texNo, textures[texNo].hTexture);

			subShader->setTextureUnit(texNo, texNo);
			subShader->setTiling(texNo, textures[texNo].tile);
			subShader->setOffset(texNo, textures[texNo].offset);
		}
	}

}


void CRenderMaterial::setColour(glm::vec4 & iColour) {
	colour = iColour;
}

void CRenderMaterial::setShader(CShader* shader) {
	//hShader = shader->getShaderHandle();
	CRenderMaterial::shader = shader;
	currentShaderType = shader->getType();
}

void CRenderMaterial::setOffset(int texNo,  glm::vec2 & offset) {
	textures[texNo].offset = offset;

}

void CRenderMaterial::setTile(int texNo, glm::vec2 & tile) {
	textures[texNo].tile = tile;
}

void CRenderMaterial::free() {
	pRenderer->setShader(0);
	pRenderer->attachTexture(0, 0);
	//for some reason need the above, I'm guessing legacy code in data textures
	//see if we still need this once everything's working the new way
	for (size_t texNo = 0; texNo < textures.size(); texNo++) {
		//pRenderer->attachTexture(texNo, 0);
	}
}

CShader * CRenderMaterial::getShader() {
	return shader;
}
