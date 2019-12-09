#include "lineBuffer2.h"

#include "glm\gtc\matrix_transform.hpp"

#include "../renderer/renderer.h"


CLineBuffer2::CLineBuffer2() {
	pRenderer = &CRenderer::getInstance();
	lineBufShader = pRenderer->createShader("lineBuf");
	hOrthoMatrix = lineBufShader->getUniformHandle("orthoMatrix");
	hTextureUnit = lineBufShader->getUniformHandle("textureUnit");
	hOffset = lineBufShader->getUniformHandle("blockOffset");
	hSize = lineBufShader->getUniformHandle("blockSize");
	
}

void CLineBuffer2::setCallbackObj(ILineBufferCallback* obj) {
	pCallbackObj = obj;
}

/** Set the dimensions of the page we're writing to. This will wipe the existing
	text sprite image buffers. */
void CLineBuffer2::setPageSize(int width, int height) {
	this->width = width;
	this->height = height;
	textSprites.clear();
	imageBuf.setSize(this->width, this->height);
	orthoView = glm::ortho<float>(0, (float)width, 0, height);
}

/** Delete all text sprites and clear the page of any existing text. */
void CLineBuffer2::clear() {
	//delete text sprites here.
	//should implicitly clear imageBuf too
	//reset anything else that needs it
}

/**	Add this text fragment as a text sprite. */
void CLineBuffer2::addTextSprite(TLineFragment& fragment) {
	return;

	if (width == 780)
		int b = 0;

	textSprites.push_back(CTextSprite(fragment.renderStartX, fragment.renderStartY));
	auto& sprite = textSprites.back();


	TRichTextRec textObj = pCallbackObj->getTexObjCallback(fragment.textObj);
	//TO DO: textObj should carry an up-to-date pointer to font
	CFont* font = &CRenderer::getInstance().fontManager.getFont(textObj.style.font);
	//should only do this once

	std::string text = textObj.text.substr(fragment.textPos, fragment.textLength);
	int lineWidth = sprite.makeTextVerts(text, font);
	if (lineWidth == 0) {
		textSprites.pop_back();
		return;
	}

	sprite.size = glm::i32vec2(lineWidth, fragment.height);
	sprite.bufId = imageBuf.reserve(sprite.size);


	sprite.renderText(getBuffer(),textObj.style.colour);
}

CRenderTexture& CLineBuffer2::getBuffer() {
	return imageBuf.getBuffer();
}

/** Draw all the text sprites to the given text buffer. */
void CLineBuffer2::renderToTextBuf(CRenderTexture& textBuf) {

	//imageBuf.getBuffer().resize(65, 64);

	pRenderer->setShader(lineBufShader);
	pRenderer->attachTexture(0, imageBuf.getBuffer().handle);
	lineBufShader->setTextureUnit(hTextureUnit, 0);

	glm::vec2 imageBufScale(1.0f / imageBuf.getBuffer().width, 1.0f / imageBuf.getBuffer().height);



	//display whole buffer
	/*
	glm::mat4 blockPos(1);// = orthoView;// *shape;
	lineBufShader->setShaderValue(hOrthoMatrix, blockPos);
	pRenderer->renderToTextureTriStrip(*pRenderer->screenQuad, textBuf);

	//return;
	*/
	if (width == 780 /*&& textSprites.size() == 10*/)
		int b = 0;
	
	
	for (auto sprite : textSprites) {

		glm::vec3 spriteOriginDist = glm::vec3(sprite.size, 0) * 0.5f;
		

		glm::vec3 pageOriginDist = glm::vec3(textBuf.width,textBuf.height,0) * 0.5f;
		glm::vec3 spriteTranslation = pageOriginDist - spriteOriginDist + glm::vec3(sprite.positionOnPage, 0);
		spriteTranslation.x *= -1;

		glm::mat4 trans = glm::translate(glm::mat4(1), spriteTranslation);
		glm::mat4 shape = glm::scale(trans, spriteOriginDist);

		glm::mat4 spritePos = orthoView * shape;

		

		glm::vec3 test = spritePos * glm::vec4( 1,-1, 0,0);
	

		lineBufShader->setShaderValue(hOrthoMatrix, spritePos);
		lineBufShader->setShaderValue(hOffset, glm::vec2(0,sprite.bufId) * imageBufScale);
		lineBufShader->setShaderValue(hSize, (glm::vec2(0, sprite.bufId)+ glm::vec2(sprite.size)) * imageBufScale);

		pRenderer->renderToTextureTriStrip(*pRenderer->screenQuad,textBuf);
		//return;
	}
	

}


