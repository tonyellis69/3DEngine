
#include "lineBuffer.h"
#include "glm\gtc\matrix_transform.hpp"
#include "../renderer/renderer.h"
#include "../utils/log.h"

CLineBuffer::CLineBuffer() {
	initShader();
}

CLineBuffer::~CLineBuffer() {

}

void CLineBuffer::setCallbackObj(ILineBuffer* obj) {
	pCallbackObj = obj;
}

/** Set the dimensions of the page we're writing to. This will wipe the existing
	text sprite image buffers. */
void CLineBuffer::setPageSize(int width, int height) {
	this->width = width;
	this->height = height;
	clear();
	spriteBuffer.setSize(this->width, this->height ); 
	pageBuf.resize(width, height);
	pageOrthoView = glm::ortho<float>(0, (float)width , 0, (float)height ); //moves origin to top left
}

/** Clear the page of any existing text and reset it. */
void CLineBuffer::clear() {
	spriteBuffer.clear();
	textSprites.clear();
	hotTexts.clear();
	hotTextSprites.clear();
	pageStart.screenPos = { 0,0 };
	pageStart.textPos = { 0,0 };
	pageEnd.screenPos = { 0,0 };
	pageEnd.textPos = { 0,0 };
	insertAtTop = false;
	mousedHotText = -1;
	prevMousedHotText = -1;
}

/**	add this text fragment as a text sprite. */
void CLineBuffer::addTextSprite(TLineFragment& fragment) {
	std::unique_ptr<CTextSprite> sprite(createSprite(fragment));
	sprite->createTextImage(spriteBuffer.getBuffer());
	textSprites.push_back(std::move(sprite));
	recalcPageState();
}

void CLineBuffer::draw() {
	
}


/** Draw all the text sprites to an internal buffer representing the page. */
void CLineBuffer::renderSprites(float dT) {
	updateHotTextPeriods(dT);

	pRenderer->rendertToTextureClear(pageBuf, glm::vec4(0,1, 1, 0));
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
int CLineBuffer::scrollDown(int scrollAmount) {
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
int CLineBuffer::scrollUp(int scrollAmount) {

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
	text sprite only if it's an overflow. */
int CLineBuffer::getOverlap() {
	int overlap = 0;
	for (auto &sprite : textSprites) {
		overlap = std::max(overlap, sprite->positionOnPage.y + sprite->size.y);
	}
	overlap = overlap - height;
	return std::max(0, overlap);
}

/** Return the distance in pixels between the bottom of the page and the bottom of the lowest
	text sprite only if it's an underflow.*/
int CLineBuffer::getUnderrun() {
	int underrun = 0;
	for (auto& sprite : textSprites) {
		underrun = std::max(underrun, sprite->positionOnPage.y + sprite->size.y);
	}
	underrun = height - underrun;
	return std::max(0, underrun);
}

/** Return the distance in pixels between the top of the page and the top of the highest
	text sprite.*/
int CLineBuffer::getTopOverlap() {
	int overlap = INT_MAX;
	for (auto &sprite : textSprites) {
		overlap = std::min(overlap, sprite->positionOnPage.y);
	}
	return abs(overlap);
}

/** Return the texture we draw the sprites to. */
CRenderTexture* CLineBuffer::getTextBuf() {
	return &pageBuf;
}


TPagePos CLineBuffer::getPageStart() {
	return pageStart;
}

TPagePos CLineBuffer::getPageEnd() {
	return pageEnd;
}

void CLineBuffer::setaddFragmentsAtTop(bool onOff) {
	insertAtTop = onOff;
}


void CLineBuffer::onMouseMove(glm::i32vec2& mousePos) {
	mousedHotText = -1;
	for (auto hotTextSprite : hotTextSprites) {
		CHotTextSprite* sprite = hotTextSprite.sprite;
		if (all(greaterThan(mousePos, sprite->positionOnPage))
			&& all(lessThan(mousePos, sprite->positionOnPage + sprite->size))) {		
			mousedHotText = hotTextSprite.hotId;
			break;
		}
	}
	
	if (mousedHotText != prevMousedHotText)
		onMousedHotTextChange();
}

///////Private functions

void CLineBuffer::initShader() {
	pRenderer = &CRenderer::getInstance();
	textSpriteShader.shader = pRenderer->textSpriteShader;
	textSpriteShader.hOrthoMatrix = textSpriteShader.shader->getUniformHandle("orthoMatrix");
	textSpriteShader.hTextureUnit = textSpriteShader.shader->getUniformHandle("textureUnit");
	textSpriteShader.hOffset = textSpriteShader.shader->getUniformHandle("blockOffset");
	textSpriteShader.hOffset2 = textSpriteShader.shader->getUniformHandle("blockOffset2");
	textSpriteShader.hSize = textSpriteShader.shader->getUniformHandle("blockSize");
	textSpriteShader.hAlpha = textSpriteShader.shader->getUniformHandle("alpha");
}

/** Calculate the various characteristics of the page, such as pageStart and pageEnd. */
void CLineBuffer::recalcPageState() {
	pageStart.textPos = { INT_MAX, 0 };
	pageEnd.textPos = { -1, 0 };
	pageEnd.screenPos.y = -1;
	pageEndNewline = false;
	std::set<unsigned int> hotFragTally;

	//pageStart.setScreenX(0); //temp! Mostly true but better to calcualte

	for (auto &sprite : textSprites) { 
		if (sprite->positionOnPage.y + sprite->size.y < 0 /*|| sprite->positionOnPage.y > height*/)
				continue;

		//find the text closest to the top left corner of the page
		if (sprite->textObj < pageStart.textPos.textObj) {
			pageStart.textPos = { sprite->textObj, sprite->textPos };
			pageStart.setScreenY(sprite->positionOnPage.y);
		}
		else if (sprite->textObj == pageStart.textPos.textObj && sprite->textPos < pageStart.textPos.pos) {
			pageStart.textPos.pos = sprite->textPos;
			pageStart.setScreenY(sprite->positionOnPage.y);
		}

		//find the text closest to the bottom right corner of the page
		if (sprite->textObj > pageEnd.textPos.textObj) {
			pageEnd.textPos = { sprite->textObj,sprite->textEnd };
			pageEnd.setScreenX(sprite->positionOnPage.x + sprite->size.x);
			pageEndNewline = sprite->causesNewLine;
		}
		else if (sprite->textObj == pageEnd.textPos.textObj && sprite->textEnd > pageEnd.textPos.pos) {
			pageEnd.textPos.pos = sprite->textEnd;
			pageEnd.setScreenX(sprite->positionOnPage.x + sprite->size.x);
			pageEndNewline = sprite->causesNewLine;
		}

		if (sprite->positionOnPage.y >= pageEnd.screenPos.y) {
			pageEnd.screenPos.y = sprite->positionOnPage.y;
			if (sprite->causesNewLine) 
				pageEnd.screenPos.y += sprite->size.y;
		}

		if (sprite->causesNewLine) {
			pageEnd.setScreenX(0);
		}

		hotFragTally.insert(sprite->getHotId());
	}

	for (auto hotFrag = hotTexts.begin(); hotFrag != hotTexts.end(); ) {
		if (hotFragTally.find(hotFrag->first) == hotFragTally.end() ) {
			hotFrag = hotTexts.erase(hotFrag);
		}
		else 
			++hotFrag;		
	}
}


CTextSprite* CLineBuffer::createSprite(TLineFragment& fragment) {
	TRichTextRec* textObj = pCallbackObj->getTexObjCallback(fragment.textObj);
	CTextSprite* sprite;
	if (textObj->hotId) {
		sprite = new CHotTextSprite();
		if (hotTexts.empty())
			hotTexts.insert({ textObj->hotId, {0} });
		else
			hotTexts.insert({ textObj->hotId, {randomPeriod(0)} });
		hotTextSprites.push_back({ (CHotTextSprite*) sprite, textObj->hotId });
		sprite->setHotId(textObj->hotId);
	}
	else
		sprite = new CTextSprite();
	sprite->setCallbackObj(this);

	//TO DO: textObj should carry an up-to-date pointer to font
	CFont* font = &CRenderer::getInstance().fontManager.getFont(textObj->style.font);
	//should only do this once
	std::string text = textObj->text.substr(fragment.textPos, fragment.textLength);
	sprite->makeTextQuads(text, font);

	sprite->setTextObjData(fragment.textObj, fragment.textPos, fragment.textLength);
	sprite->setPageOthoMatrix(&pageOrthoView);

	if (insertAtTop) {
		sprite->setPagePosition(0, pageStart.screenPos.y - fragment.height);
	} else
		sprite->setPagePosition(pageEnd.screenPos.x, pageEnd.screenPos.y);

	sprite->setTextColour(textObj->style.colour);

	sprite->setPageTexture(&pageBuf);
	sprite->setShader(&textSpriteShader);

	sprite->causesNewLine = fragment.causesNewLine;

	return sprite;
}

glm::i32vec2 CLineBuffer::reserveSpriteImageSpace(glm::i32vec2& size) {
	return spriteBuffer.reserve(size);
}

void CLineBuffer::freeSpriteImageSpace(glm::i32vec2& bufId) {
	spriteBuffer.free(bufId);
}

glm::vec4 CLineBuffer::getHotTextColour() {
	return pCallbackObj->getHotTextSelectedColour();
}


/** Update the animation period for each hot text. */
void CLineBuffer::updateHotTextPeriods(float dT) {
	float numHotTexts = hotTexts.size() + 4;
	for (auto& hotText : hotTexts) {
		if (hotText.second.selectionState == selLit)
			continue;
		hotText.second.period += dT / (numHotTexts);
		hotText.second.period = glm::fract(hotText.second.period);
	}
}

/** Return the amimation period for the given hot text. */
float CLineBuffer::getHotPeriod(unsigned int hotId) {
	//if (hotTexts[hotId].selectionState == selLit)
	//	return 0.5f;
	

	float numHotTexts = hotTexts.size() + 4;
	float hotOnMultiplier = 1.5f;
	float period = hotTexts[hotId].period * (numHotTexts  / hotOnMultiplier);

	period = std::clamp(period, 0.0f, 1.0f);	
	period = period * 2.0f - 1.0f;

	float curve = 2.0f;

	period = 1.0f - pow(abs(period), curve);

	if (hotTexts[hotId].selectionState == selWarmup && period >= 0.9f) {
		hotTexts[hotId].selectionState = selLit;
		return 1.0f;
	}

	if (hotTexts[hotId].selectionState == selWarmdown && period <= 0.1f) {
		hotTexts[hotId].selectionState = selNone;
		hotTexts[hotId].period = randomPeriod(hotTexts[hotId].period);
	}

	return period;
}

/** Return a random number between 0 and 1. */
float CLineBuffer::randomPeriod(float start) {
	std::uniform_real_distribution period{ start, 10.0f };
	return period(randEngine);
}

/** Free whatever memory this hot text sprite is using. */
void CLineBuffer::freeHotTextSprite(CHotTextSprite* sprite) {
//	freeSpriteImageSpace(sprite->bufId); //dealt with by base destructor!

	freeSpriteImageSpace(sprite->bufId2);

	for (auto hotRec = hotTextSprites.begin(); hotRec != hotTextSprites.end(); ) {
		if (hotRec->sprite == sprite) {
			hotTextSprites.erase(hotRec);
			break;
		}
		else
			hotRec++;
	}
}

void CLineBuffer::onMousedHotTextChange() {
	if (prevMousedHotText != -1) {
		hotTexts[prevMousedHotText].selectionState = selWarmdown;
	}

	prevMousedHotText = mousedHotText;
	hotTexts[mousedHotText] = { 0,selWarmup };

	pCallbackObj->hotTextMouseOver(mousedHotText);
}





