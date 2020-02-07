#include "lineBuffer2.h"

#include "glm\gtc\matrix_transform.hpp"

#include "../renderer/renderer.h"

#include "../utils/log.h"


CLineBuffer2::CLineBuffer2() {
	initShader();
	clear();

}

CLineBuffer2::~CLineBuffer2() {
	//for (auto &sprite : textSprites)
//delete &sprite;
	
}

void CLineBuffer2::setCallbackObj(ILineBufferCallback* obj) {
	pCallbackObj = obj;
}

/** Set the dimensions of the page we're writing to. This will wipe the existing
	text sprite image buffers. */
void CLineBuffer2::setPageSize(int width, int height) {
	this->width = width;
	this->height = height;
	clear();
	spriteBuffer.setSize(this->width, this->height ); 
	pageBuf.resize(width, height);
	pageOrthoView = glm::ortho<float>(0, (float)width , 0, (float)height ); //moves origin to top left
}

/** Clear the page of any existing text and reset it. */
void CLineBuffer2::clear() {
	spriteBuffer.clear();
	textSprites.clear();
	hotTexts.clear();
	finalFrag = {0, 0, 0, 0, 0, 0, 0, no, true};
	yPosTracker = 0;
	pageStart.unset = true;
	pageEnd.unset = true;
	insertAtTop = false;
}

/**	Add this text fragment as a text sprite. */
void CLineBuffer2::addTextSprite(TLineFragment& fragment) {
	updatePageStartEnd(fragment);
	std::unique_ptr<CTextSprite> sprite(createSprite(fragment));
	updateFinalFrag(*sprite);
	sprite->createTextImage(spriteBuffer.getBuffer());
	textSprites.push_back(std::move(sprite));
}

void CLineBuffer2::draw() {
	
}


/** Draw all the text sprites to an internal buffer representing the page. */
void CLineBuffer2::renderSprites(float dT) {
	updateHotTextPeriods(dT);

	pRenderer->rendertToTextureClear(pageBuf, glm::vec4(1,1, 1, 1));
	pRenderer->setShader(textSpriteShader.shader);
	pRenderer->attachTexture(0, spriteBuffer.getBuffer().handle);
	textSpriteShader.shader->setTextureUnit(textSpriteShader.hTextureUnit, 0);

	pRenderer->beginRenderToTexture(pageBuf);
	for (auto &sprite : textSprites) {
		sprite->draw();
	}
	pRenderer->endRenderToTexture();
}

/** Move all text sprites up by the given amount, deleting any that end up outside the page entirely.*/
int CLineBuffer2::scrollDown(int scrollAmount) {
	int overlap = getOverlap();
	int scrollAchieved = std::min(scrollAmount, overlap);
	for (auto sprite = textSprites.begin(); sprite != textSprites.end();) {
		(*sprite)->adjustYPos(-scrollAchieved);
		if ((*sprite)->positionOnPage.y + (*sprite)->size.y < 0) {
		 sprite = textSprites.erase(sprite);
		}
		else
			sprite++;
	}
	recalcPageState();
	return scrollAchieved;
}

/** Move all text sprites down by the given amount, deleting any that end up outside the page entirely.*/
int CLineBuffer2::scrollUp(int scrollAmount) {

	//spriteBuffer.tmpSave();

	int overlap = getTopOverlap();
	int scrollAchieved = std::min(scrollAmount, overlap);

	for (auto sprite = textSprites.begin(); sprite != textSprites.end();) {
		(*sprite)->adjustYPos(scrollAchieved);
		if ((*sprite)->positionOnPage.y > height) {
			sprite = textSprites.erase(sprite);
		}
		else
			sprite++;
	}
	recalcPageState();
	return scrollAchieved;
}

/** Return the distance in pixels between the bottom of the page and the bottom of the lowest
	text sprite.*/
int CLineBuffer2::getOverlap() {
	int overlap = 0;
	for (auto &sprite : textSprites) {
		overlap = std::max(overlap, sprite->positionOnPage.y + sprite->size.y);
	}
	overlap = overlap - height;
	return std::max(0, overlap);
}

/** Return the distance in pixels between the top of the page and the top of the highest
	text sprite.*/
int CLineBuffer2::getTopOverlap() {
	int overlap = INT_MAX;
	for (auto &sprite : textSprites) {
		overlap = std::min(overlap, sprite->positionOnPage.y);
	}
	return abs(overlap);
}

/** Return the texture we draw the sprites to. */
CRenderTexture* CLineBuffer2::getTextBuf() {
	return &pageBuf;
}

TLineFragment CLineBuffer2::getFinalFrag() {
	return finalFrag;
}

TCharacterPos CLineBuffer2::getPageStart() {
	return pageStart;
}

TCharacterPos CLineBuffer2::getPageEnd() {
	return pageEnd;
}

void CLineBuffer2::setAddFragmentsAtTop(bool onOff) {
	insertAtTop = onOff;
}

///////Private functions

void CLineBuffer2::initShader() {
	pRenderer = &CRenderer::getInstance();
	textSpriteShader.shader = pRenderer->textSpriteShader;
	textSpriteShader.hOrthoMatrix = textSpriteShader.shader->getUniformHandle("orthoMatrix");
	textSpriteShader.hTextureUnit = textSpriteShader.shader->getUniformHandle("textureUnit");
	textSpriteShader.hOffset = textSpriteShader.shader->getUniformHandle("blockOffset");
	textSpriteShader.hOffset2 = textSpriteShader.shader->getUniformHandle("blockOffset2");
	textSpriteShader.hSize = textSpriteShader.shader->getUniformHandle("blockSize");
	textSpriteShader.hAlpha = textSpriteShader.shader->getUniformHandle("alpha");
}

/* If necessary, update finalFrag, our record of the endmost page fragment, with the details of this sprite. 
	finalFrag is used to find the next line of text when scrolling down. */
void CLineBuffer2::updateFinalFrag(CTextSprite& sprite) {
	if (sprite.textObj > finalFrag.textObj) {
		finalFrag.textObj = sprite.textObj;
		finalFrag.textPos = sprite.textPos;
		finalFrag.textLength = sprite.textLength;
	}
	else if (sprite.textObj == finalFrag.textObj && sprite.textPos > finalFrag.textPos) {
		finalFrag.textPos = sprite.textPos;
		finalFrag.textLength = sprite.textLength;
	}
}

/** Calculate the various characteristics of the page, such as pageStart and pageEnd. */
void CLineBuffer2::recalcPageState() {
	int earliestTextObj = INT_MAX; int earliesTextPos;
	int latestTextObj = -1; int latestTextPos;
	yPosTracker = -1;
	finalFrag = { 0, 0, 0, 0, 0, 0, 0, no, true };
	std::set<unsigned int> hotFragTally;

	for (auto &sprite : textSprites) { 
		if (sprite->positionOnPage.y + sprite->size.y < 0 || sprite->positionOnPage.y > height)
			continue;

		//find the text closest to the top left corner of the page
		if (sprite->textObj < earliestTextObj) {
			earliestTextObj = sprite->textObj;
			earliesTextPos = sprite->textPos;
		}
		else if (sprite->textObj == earliestTextObj && sprite->textPos < earliesTextPos)
			earliesTextPos = sprite->textPos;

		//find the text closest to the bottom right corner of the page
		if (sprite->textObj > latestTextObj) {
			latestTextObj = sprite->textObj;
			latestTextPos = sprite->textEnd;
		}
		else if (sprite->textObj == latestTextObj && sprite->textEnd < latestTextPos)
			latestTextPos = sprite->textEnd;

		yPosTracker = std::max(yPosTracker, sprite->positionOnPage.y + sprite->size.y);
		updateFinalFrag(*sprite);
		hotFragTally.insert(sprite->getHotId());
	}
	pageStart = { earliestTextObj, earliesTextPos };
	pageEnd = { latestTextObj, latestTextPos };

	for (auto hotFrag = hotTexts.begin(); hotFrag != hotTexts.end(); ) {
		if (hotFragTally.find(hotFrag->first) == hotFragTally.end() ) {
			hotFrag = hotTexts.erase(hotFrag);
		}
		else 
			++hotFrag;		
	}
}

/** Update pageStart and pageEnd if the given fragment invalidates the existing values. */
void CLineBuffer2::updatePageStartEnd(TLineFragment& fragment) {
	if (pageStart.unset)
		pageStart.setPos(fragment.textObj, fragment.textPos);
	else
		if (TCharacterPos(fragment.textObj, fragment.textPos) < pageStart)
			pageStart = TCharacterPos(fragment.textObj, fragment.textPos);

	if (TCharacterPos(fragment.textObj, fragment.textPos + fragment.textLength) > pageEnd)
		pageEnd.setPos(fragment.textObj, fragment.textPos + fragment.textLength);
}

/** Return the y position on the page at which this sprite should be drawn. */
int CLineBuffer2::calcSpriteYpos(TLineFragment& fragment) {
	int spriteYpos = yPosTracker;
	if (insertAtTop) {
		spriteYpos = -getTopOverlap() - fragment.height;
	}
	else {
		if (fragment.causesNewLine)
			yPosTracker += fragment.height;
	}
	return spriteYpos;
}

CTextSprite* CLineBuffer2::createSprite(TLineFragment& fragment) {
	TRichTextRec textObj = pCallbackObj->getTexObjCallback(fragment.textObj);
	CTextSprite* sprite;
	if (textObj.hotId) {
		sprite = new CHotTextSprite();
		if (hotTexts.empty())
			hotTexts.insert({ textObj.hotId, 0 }); 
		else
			hotTexts.insert({ textObj.hotId, randomPeriod() }); 
		sprite->setHotId(textObj.hotId);
	}
	else
		sprite = new CTextSprite();
	sprite->setCallbackObj(this);

	//TO DO: textObj should carry an up-to-date pointer to font
	CFont* font = &CRenderer::getInstance().fontManager.getFont(textObj.style.font);
	//should only do this once
	std::string text = textObj.text.substr(fragment.textPos, fragment.textLength);
	sprite->makeTextQuads(text, font);


	sprite->setTextObjData(fragment.textObj, fragment.textPos, fragment.textLength);
	sprite->setPageOthoMatrix(&pageOrthoView);
	sprite->setPagePosition(fragment.renderStartX, calcSpriteYpos(fragment));
	sprite->setTextColour(textObj.style.colour);

	sprite->setPageTexture(&pageBuf);
	sprite->setShader(&textSpriteShader);

	return sprite;
}

glm::i32vec2 CLineBuffer2::reserveSpriteImageSpace(glm::i32vec2& size) {
	return spriteBuffer.reserve(size);
}

void CLineBuffer2::freeSpriteImageSpace(glm::i32vec2& bufId) {
	spriteBuffer.free(bufId);
}

glm::vec4 CLineBuffer2::getHotTextColour() {
	return pCallbackObj->getHotTextSelectedColour();
}


/** Update the animation period for each hot text. */
void CLineBuffer2::updateHotTextPeriods(float dT) {
	float numHotTexts = hotTexts.size() + 4;
	for (auto& hotText : hotTexts) {
		hotText.second += dT / (numHotTexts);
		hotText.second = glm::fract(hotText.second);
	}
}

/** Return the amimation period for the given hot text. */
float CLineBuffer2::getHotPeriod(unsigned int hotId) {
	
	float numHotTexts = hotTexts.size() + 4;

	float hotOnMultiplier = 1.5f;

	float period = hotTexts[hotId] * (numHotTexts  / hotOnMultiplier);

	period = std::clamp(period, 0.0f, 1.0f);
	
	period = period * 2.0f - 1.0f;

	float curve = 2.0f;


	return 1.0f - pow(abs(period), curve);

}

/** Return a random number between 0 and 1. */
float CLineBuffer2::randomPeriod() {
	std::uniform_real_distribution period{ 0.0f, 10.0f };
	return period(randEngine);
}





