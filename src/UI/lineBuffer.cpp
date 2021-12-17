
#include "lineBuffer.h"
#include "glm\gtc\matrix_transform.hpp"
#include "../renderer/renderer.h"
#include "../utils/log.h"
#include "UI/fonts.h"
#include <set>


CLineBuffer::CLineBuffer() {
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
	pageBuf.resize(width, height);
	pageOrthoView = glm::ortho<float>(0, (float)width , 0, (float)height ); //moves origin to top left
}

/** Clear the page of any existing text and reset it. */
void CLineBuffer::clear() {
	textSprites.clear();
	hotTexts.clear();
	//hotTextSprites.clear();
	insertAtTop = false;
	pageDirty = true;
}


bool CLineBuffer::addTextSprite(TFragment2& fragment) {
	std::unique_ptr<CTextSprite> sprite(createSprite(fragment));
	bool overrun = false;
	if (sprite->positionOnPage.y + sprite->size.y > height)
		overrun = true;
	if (insertAtTop) {
		textSprites.insert(textSprites.begin() + current1stSprite++, std::move(sprite));
	}
	else
		textSprites.push_back(std::move(sprite));
	pageDirty = true;
	return overrun;
}

void CLineBuffer::draw() {
	
}


/** Draw all the text sprites to an internal buffer representing the page. */
void CLineBuffer::renderSprites(float dT) {

	if (pageDirty) {
		updateHotTextPeriods(dT);
		renderer.rendertToTextureClear(pageBuf, glm::vec4(1, 0, 0, 0)); //600
		renderer.setShader(renderer.textShader);


		for (auto& sprite : textSprites) {
			sprite->draw();
		}
		renderer.endRenderToTexture();

	}
	else {




	}



	pageDirty = false;
}

/** Move all text sprites up by the given amount, deleting any that end up outside the page entirely.*/
int CLineBuffer::scrollDown3(int scrollAmount) {
	int overlap = getOverlap();
	int scrollAchieved = std::min(scrollAmount, overlap);
	if (scrollAchieved == 0) {
		return scrollAchieved;
	}

	for (auto& sprite = textSprites.begin(); sprite != textSprites.end();) {
		(*sprite)->adjustYPos(-scrollAchieved);
		if ((*sprite)->positionOnPage.y + (*sprite)->size.y < 0) {
			sprite = textSprites.erase(sprite);
		}
		else
			sprite++;
	}
	pageDirty = true;
	return scrollAchieved;
}

/** Move all text sprites down by the given amount, deleting any that end up outside the page entirely.*/
int CLineBuffer::scrollUp3(int scrollAmount) {
	int overlap = getTopOverlap();
	int scrollAchieved = std::min(scrollAmount, overlap);

	for (auto& sprite = textSprites.begin(); sprite != textSprites.end();) {
		(*sprite)->adjustYPos(scrollAchieved);
		if ((*sprite)->positionOnPage.y > height) {
			sprite = textSprites.erase(sprite);
		}
		else
			sprite++;
	}
	pageDirty = true;
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


void CLineBuffer::setaddFragmentsAtTop(bool onOff) {
	insertAtTop = onOff;
	if (insertAtTop)
		current1stSprite = 0;
}


std::string CLineBuffer::onMouseMove(glm::i32vec2& mousePos) {
	std::string msg;

	for (auto& sprite : textSprites) {
		if (sprite->isHotText) {
			CHotTextSprite* hotSprite = (CHotTextSprite*)sprite.get();
		

			//check for collision
			if (all(greaterThan(mousePos, hotSprite->positionOnPage))
			&& all(lessThan(mousePos, sprite->positionOnPage + sprite->size))) {		
				hotSprite->highlighted = true;
				pageDirty = true;
				msg = hotSprite->msg;
			}
			else if (hotSprite->highlighted) {
				hotSprite->highlighted = false;
				pageDirty = true;
			}
		}
	}
	
	return msg;
}

void CLineBuffer::onMouseOff() {
	for (auto& sprite : textSprites) {
		if (sprite->isHotText) {
			CHotTextSprite* hotSprite = (CHotTextSprite*)sprite.get();
			if (hotSprite->highlighted) {
				hotSprite->highlighted = false;
				pageDirty = true;
			}
		}
	}

}

/** Return body text position for the start of the page. */
int CLineBuffer::getPageTextStart() {
	int start = INT_MAX;
	for (auto& sprite : textSprites) {
		start = std::min(start, sprite->textStart);
	}
	return start;
}

/** Return body text position for the end of the page. */
int CLineBuffer::getPageTextEnd() {
	int end = INT_MIN;
	for (auto& sprite : textSprites) {
		end = std::max(end, sprite->textStart + sprite->textLength);
	}
	return end;
}




///////Private functions


CTextSprite* CLineBuffer::createSprite(TFragment2& fragment) {
	CTextSprite* sprite;
	if (fragment.style.hotText != "")
		sprite = new CHotTextSprite(fragment.style.hotText);
	else
		sprite = new CTextSprite();
	sprite->setCallbackObj(this);

	CFont* font = fnt::get(fragment.style.fontName.data());
	sprite->makeTextQuads(fragment.text, font);

	sprite->setPageOthoMatrix(&pageOrthoView);
	
	if (insertAtTop) {
		glm::i32vec2 pageStart = calcPageTextStart();
		sprite->setPagePosition(pageStart.x, pageStart.y - fragment.pixelHeight);
	}
	else {
		glm::i32vec2 pageEnd = calcPageTextEnd();
		sprite->setPagePosition(pageEnd.x, pageEnd.y);
	}

	sprite->setTextColour(fragment.style.colour);

	sprite->causesNewLine = fragment.causesNewline;
	sprite->textStart = fragment.textStart;
	sprite->textLength = fragment.text.size();
	return sprite;
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

///** Free whatever memory this hot text sprite is using. */
//void CLineBuffer::freeHotTextSprite(CHotTextSprite* sprite) {
////	freeSpriteImageSpace(sprite->bufId); //dealt with by base destructor!
//
//	freeSpriteImageSpace(sprite->bufId2);
//
//	for (auto hotRec = hotTextSprites.begin(); hotRec != hotTextSprites.end(); ) {
//		if (hotRec->sprite == sprite) {
//			hotTextSprites.erase(hotRec);
//			break;
//		}
//		else
//			hotRec++;
//	}
//}



/** Return the pixel position where text on this page ends. */
glm::i32vec2 CLineBuffer::calcPageTextEnd() {
	glm::i32vec2 endPos = { 0,0 };

	if (textSprites.empty())
		return endPos;

	//find lowest sprites
	//then rightmost of those
	auto lowest = textSprites.begin()->get();
	for (auto& sprite : textSprites) {
		if (sprite->positionOnPage.y > lowest->positionOnPage.y) { //new contender
			lowest = sprite.get();
			continue;
		}
		if (sprite->positionOnPage.y == lowest->positionOnPage.y) {
			if (sprite->positionOnPage.x > lowest->positionOnPage.x)
				lowest = sprite.get();
		}
	}

	if (lowest->causesNewLine)
		endPos = { 0, lowest->positionOnPage.y + lowest->size.y };
	else
		endPos = { lowest->positionOnPage.x + lowest->size.x, lowest->positionOnPage.y };

	return endPos;
}

/** Return the pixel position where text on this page starts. */
glm::i32vec2 CLineBuffer::calcPageTextStart() {
	glm::i32vec2 startPos = { 0,0 };
	if (textSprites.empty())
		return startPos;

	auto followingSprite = textSprites[current1stSprite].get();

	if (current1stSprite == 0) {
		startPos = { 0, followingSprite->positionOnPage.y };
	}
	else {
		auto prevSprite = textSprites[current1stSprite - 1].get();
		startPos = { prevSprite->positionOnPage.x + prevSprite->size.x,
			followingSprite->positionOnPage.y };
	}

	return startPos;
}






