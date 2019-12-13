#include "lineBuffer2.h"

#include "glm\gtc\matrix_transform.hpp"

#include "../renderer/renderer.h"

#include "../utils/log.h"


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
	imageBuf.setSize(this->width, this->height ); 
	
	orthoView = glm::ortho<float>((float)width * -0.5f, (float)width * 0.5f, -(float)height * 0.5f, (float)height * 0.5f );
}

/** Delete all text sprites and clear the page of any existing text. */
void CLineBuffer2::clear() {
	//delete text sprites here.
	//should implicitly clear imageBuf too
	//reset anything else that needs it
}

/**	Add this text fragment as a text sprite. */
void CLineBuffer2::addTextSprite(TLineFragment& fragment) {
	//return;

	

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
	sprite.textObj = fragment.textObj;
	sprite.textPos = fragment.textPos;


	sprite.renderText(getBuffer(),textObj.style.colour);

}

CRenderTexture& CLineBuffer2::getBuffer() {
	return imageBuf.getBuffer();
}

/** Draw all the text sprites to the given text buffer. */
void CLineBuffer2::renderToTextBuf(CRenderTexture& textBuf) {
	pRenderer->rendertToTextureClear(textBuf, glm::vec4(0, 0, 0, 0));
	pRenderer->setShader(lineBufShader);
	pRenderer->attachTexture(0, imageBuf.getBuffer().handle);
	lineBufShader->setTextureUnit(hTextureUnit, 0);

	glm::vec2 imageBufScale(1.0f / imageBuf.getBuffer().width, 1.0f / imageBuf.getBuffer().height);

	
	for (auto sprite : textSprites) {
		glm::vec3 spriteOriginDist = glm::vec3(sprite.size, 0) * 0.5f;
		glm::vec3 pageOriginDist = glm::vec3(textBuf.width,textBuf.height,0) * 0.5f;
		glm::vec3 spriteTranslation = spriteOriginDist - pageOriginDist  + glm::vec3(sprite.positionOnPage, 0);

		glm::mat4 trans = glm::translate(glm::mat4(1), spriteTranslation);
		glm::mat4 shape = glm::scale(trans, spriteOriginDist);
		glm::mat4 spritePos = orthoView  *shape;

		lineBufShader->setShaderValue(hOrthoMatrix, spritePos);
		lineBufShader->setShaderValue(hOffset, glm::vec2(0,sprite.bufId) * imageBufScale);
		lineBufShader->setShaderValue(hSize, ( glm::vec2(sprite.size)) * imageBufScale);

		pRenderer->renderToTextureTriStrip(*pRenderer->screenQuad,textBuf);
	}
}

/** Move all text sprites up or down by the given amount.*/
int CLineBuffer2::scroll(int scrollAmount) {
	int overlap = getOverlap();
	int maxScroll = std::min(scrollAmount, overlap);
	//return;
	if (width == 780)
		sysLog << "\nscroll: " << maxScroll;
	for (auto sprite = textSprites.begin(); sprite != textSprites.end();) {
		sprite->positionOnPage.y -= maxScroll;
		if (sprite->positionOnPage.y + sprite->size.y < 0) {
			sprite = textSprites.erase(sprite);
			//TO DO: free imagebuf memory!
		}
		else
			sprite++;
	}
	return maxScroll;
}

/** Return the distance in pixels between the bottom of the page and the bottom of the lowest
	text sprite.*/
int CLineBuffer2::getOverlap() {
	int overlap = 0;
	for (auto sprite : textSprites)
		overlap = std::max(overlap, sprite.positionOnPage.y + sprite.size.y);
	overlap = overlap - height;
	return std::max(0, overlap);
}

/** Return a data structure showing the topmost textObj drawn on the page
	and the position in its text at which drawing commences. */
TCharacterPos CLineBuffer2::getStartText() {
	TCharacterPos startText;
	int earliestTextObj = INT_MAX; int earliesTextPos;
	for (auto sprite : textSprites) {
		int currentTextObj = std::min(earliestTextObj, sprite.textObj);
		if (currentTextObj < earliestTextObj) {
			earliesTextPos = INT_MAX;
			earliestTextObj = currentTextObj;
		}
		earliesTextPos = std::min(earliesTextPos, sprite.textPos);
	}

	startText = { earliestTextObj, earliesTextPos };
	return startText;
}




