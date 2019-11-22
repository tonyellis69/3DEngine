#include "lineBuffer2.h"




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
	//get the text, font, any other details... use callbacks.
	TRichTextRec textObj = pCallbackObj->getTexObjCallback(fragment.textObj);

	textSprites.push_back(CTextSprite(fragment.renderStartX, fragment.renderStartY));
	auto sprite = textSprites.back();
	sprite.size = glm::i32vec2(fragment.textLength, fragment.height);
	sprite.bufId = imageBuf.getBuf(sprite.size);

	//reserve space on imageBuf - tick

	//render text to buffer - steal from CTextBuffer::addFragment
	//create the finished text sprite
	//stick it on the list
}
