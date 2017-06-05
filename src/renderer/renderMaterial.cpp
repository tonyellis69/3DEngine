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

	if (currentShaderType == standardPhong) {
		CPhongShader* subShader = (CPhongShader*)shader->getThisShader();
		subShader->setColour(colour);
	}
	
	else if (currentShaderType == standardMultiTex) {
		CMultiTexShader* subShader = (CMultiTexShader*)shader->getThisShader();

		//if we have textures, assign them
		for (size_t texNo = 0; texNo < textures.size(); texNo++) {
			pRenderer->attachTexture(texNo, textures[texNo].hTexture);

			subShader->setTextureUnit(texNo, texNo);
			subShader->setTiling(texNo, textures[texNo].tile);
			subShader->setOffset(texNo, textures[texNo].offset);
		}
	}
	else if (currentShaderType == standardTex) {
		CTexShader* subShader = (CTexShader*)shader->getThisShader();

			pRenderer->attachTexture(0, textures[0].hTexture);

			subShader->setTextureUnit(0);
			subShader->setTiling(textures[0].tile);
			subShader->setOffset(textures[0].offset);
	}
	else if (currentShaderType == standardBillboard) {
		CBillboardShader* subShader = (CBillboardShader*)shader->getThisShader();

		pRenderer->attachTexture(0, textures[0].hTexture);

		subShader->setTextureUnit(0);
		subShader->setTiling(textures[0].tile);
		subShader->setOffset(textures[0].offset);
	}

}


void CRenderMaterial::setColour(glm::vec4 & iColour) {
	colour = iColour;
}

void CRenderMaterial::setShader(CShader* shader) {
	CRenderMaterial::shader = shader;
	currentShaderType = shader->getType();
}

void CRenderMaterial::setOffset(int texNo,  glm::vec2 & offset) {
	textures[texNo].offset = offset;

}

void CRenderMaterial::setTile(int texNo, glm::vec2 & tile) {
	textures[texNo].tile = tile;
}

void CRenderMaterial::unAssign() {
	//pRenderer->setShader(0); //turned this off as there's a performance hit
	
	pRenderer->attachTexture(0, 0);
	//Don't seem to need the above any more, either.
	//TO DO: ACHTUNG! Currently need this for text, but probably because text is still drawn old-style.
	//modernise text drawing then see.


	for (size_t texNo = 0; texNo < textures.size(); texNo++) {
		//pRenderer->attachTexture(texNo, 0);
	}
}

CShader * CRenderMaterial::getShader() {
	return shader;
}
