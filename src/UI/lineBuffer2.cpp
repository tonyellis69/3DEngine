#include "lineBuffer2.h"

#include "../renderer/renderer.h"


void CLineBuffer2::setCallbackObj(ILineBufferCallback* obj) {
	pCallbackObj = obj;
}

/** Set the dimensions of the page we're writing to. This will wipe the existing
	text sprite image buffers. */
void CLineBuffer2::setPageSize(int width, int height) {
	this->width = width;
	this->height = height;
	imageBuf.setSize(width, height);
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

	textSprites.push_back(CTextSprite(fragment.renderStartX, fragment.renderStartY));
	auto& sprite = textSprites.back();


	TRichTextRec textObj = pCallbackObj->getTexObjCallback(fragment.textObj);
	//TO DO: textObj should carry an up-to-date pointer to font
	CFont* font = &CRenderer::getInstance().fontManager.getFont(textObj.style.font);
	//should only do this once

	std::string text = textObj.text.substr(fragment.textPos, fragment.textLength);
	int lineWidth = sprite.makeTextVerts(text, font);

	sprite.size = glm::i32vec2(lineWidth, fragment.height);
	sprite.bufId = imageBuf.reserve(sprite.size);


	sprite.renderText(getBuffer(),textObj.style.colour);
}

CRenderTexture& CLineBuffer2::getBuffer() {
	return imageBuf.getBuffer();
}

/** Draw all the text sprites to the given text buffer. */
void CLineBuffer2::renderToTextBuf(CRenderTexture& texBuf) {

		GLuint hTempFrameBuffer;
		glGenFramebuffers(1, &hTempFrameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, hTempFrameBuffer);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, imageBuf.getBuffer().handle, 0);
		glBindTexture(GL_TEXTURE_2D, texBuf.handle);

		for (auto sprite : textSprites) {
			

			glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, sprite.positionOnPage.y, 0, sprite.bufId, sprite.size.x, sprite.size.y);

		}

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

}


