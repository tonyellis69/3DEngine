#include "textSprite.h"

#include "../vertBufs.h"
#include "../renderer/renderer.h"

#include "../utils/log.h"

#include "glm\gtc\matrix_transform.hpp"

CTextSprite::CTextSprite() {
	causesNewLine = false;
}

/** Store data about where this sprite's text is found in its textObj. */
void CTextSprite::setTextObjData(int textObj, int textPos, int textLength) {
	textEnd = textPos + textLength;
	this->textLength = textLength;
	this->textPos = textPos;
	this->textObj = textObj;
}

/** Set sprite's position on page.*/
void CTextSprite::setPagePosition(int posX, int posY) {
	positionOnPage = glm::i32vec2(posX, posY);
	updateMatrix();
}

/** Set the colour of the text.*/
void CTextSprite::setTextColour(glm::vec4& colour) {
	textColour = colour;
}


/** Create the temporary polygons required to render this text. */
void CTextSprite::makeTextQuads( const std::string& text, CFont* font) {
	std::vector<vBuf::T2DtexVert> textQuads;
	std::vector<unsigned int> textQuadsIndex;
	int v = 0;
	glm::vec2 blCorner(0, 0);
	glyph* glyph;
	unsigned int numChars = text.size();
	textQuads.resize(numChars * 4);
	for (unsigned int c = 0; c < numChars; c++) {
		if (text[c] != '\n') {
			glyph = font->table[(unsigned char)text[c]];
			textQuads[v].v = blCorner; //A
			textQuads[v + 1].v = blCorner + glm::vec2(glyph->width, 0.0f); //B
			textQuads[v + 2].v = blCorner + glm::vec2(0.0f, glyph->height); //C
			textQuads[v + 3].v = blCorner + glm::vec2(glyph->width, glyph->height); //D
			textQuads[v].tex = glm::vec2(glyph->u, glyph->t);
			textQuads[v + 1].tex = glm::vec2(glyph->s, glyph->t);
			textQuads[v + 2].tex = glm::vec2(glyph->u, glyph->v);
			textQuads[v + 3].tex = glm::vec2(glyph->s, glyph->v);

			textQuadsIndex.push_back(v); textQuadsIndex.push_back(v + 3); textQuadsIndex.push_back(v + 2);
			textQuadsIndex.push_back(v + 1); textQuadsIndex.push_back(v + 3); textQuadsIndex.push_back(v);
			v += 4;
			blCorner += glm::vec2(glyph->width, 0);
		}
	}

	tmpQuadBuf.storeVerts(textQuads, textQuadsIndex, 2, 2);

	this->font = font;
	size = glm::i32vec2(blCorner.x, font->lineHeight);
}

/** Use the given orthographic matrix when drawing this sprite's cached text image to the page. */
void CTextSprite::setPageOthoMatrix(glm::mat4* pageOrthoViewMatrix) {
	this->pageOrthoViewMatrix = pageOrthoViewMatrix;
}

void CTextSprite::draw() {
	renderer.attachTexture(0, font->texture); //attach texture to textureUnit (0)
	renderer.texShader->setTextureUnit(renderer.hTextTexture, 0);
	renderer.texShader->setShaderValue(renderer.hTextColour, textColour);
	renderer.texShader->setShaderValue(renderer.hTextOrthoMatrix, matrix2);
	renderer.drawTrisBuf(tmpQuadBuf, 0, tmpQuadBuf.numElements);
}

/** Modify the vertical position of this sprite on the page by the given amount. */
bool CTextSprite::adjustYPos(int change) {
	positionOnPage.y += change;
	updateMatrix();
	return true;
}


/** Recalculate the matrix that defines this sprite's size and position in page space. */
void CTextSprite::updateMatrix() {
	matrix2 = glm::translate(glm::mat4(1), glm::vec3(positionOnPage, 0));
	matrix2 = *pageOrthoViewMatrix * matrix2;
}


//////////////////////////Hot text sprite stuff
//
//CHotTextSprite::~CHotTextSprite() {
//	if (bufId.x != INT_MAX) {
//		//callbackObj->freeSpriteImageSpace(bufId);
//		//callbackObj->freeSpriteImageSpace(bufId2);
//		callbackObj->freeHotTextSprite(this);
//	}
//}
//
///** Draw the two alternating text images to the given storage texture. */
//void CHotTextSprite::createTextImage(CRenderTexture& storageTexture) {
//	if (size.x == 0)
//		return;
//
//	bufId = callbackObj->reserveSpriteImageSpace(size);
//	renderTextQuads(storageTexture, bufId, textColour);
//
//	bufId2 = callbackObj->reserveSpriteImageSpace(size);
//	renderTextQuads(storageTexture, bufId2, hotTextColour);
//	freeQuadBuffer();
//}
//
//void CHotTextSprite::setTextColour(glm::vec4& colour) {
//	textColour = colour;
//	//hotTextColour = callbackObj->getHotTextColour();
//}
//
//
//void CHotTextSprite::draw() {
//	textSpriteShader->shader->setShaderValue(textSpriteShader->hOrthoMatrix, matrix);
//	textSpriteShader->shader->setShaderValue(textSpriteShader->hOffset, glm::vec2(bufId));
//	textSpriteShader->shader->setShaderValue(textSpriteShader->hOffset2, glm::vec2(bufId2));
//	textSpriteShader->shader->setShaderValue(textSpriteShader->hSize, glm::vec2(size));
//
//	
//	float alpha;
//	
//
//	//give the shader offsets for both sprite images
//	
//	//get period for hotId (getter can multiply it up
//	//get working as alpha
//	//then use as mix between two samples
//
//	alpha = callbackObj->getHotPeriod(hotId);
//
//	textSpriteShader->shader->setShaderValue(textSpriteShader->hAlpha, alpha);
//
//	renderer.drawTriStripBuf(renderer.screenQuad);
//}
//
//void CHotTextSprite::setHotId(unsigned int hotId)  {
//	this->hotId = hotId;
//}
//
//unsigned int CHotTextSprite::getHotId() {
//	return hotId;
//}

CHotTextSprite::CHotTextSprite(const std::string& msg) {
	isHotText = true;
	this->msg = msg;
}

void CHotTextSprite::draw() {
	if (highlighted) {
		renderer.attachTexture(0, font->texture);
		renderer.texShader->setTextureUnit(renderer.hTextTexture, 0);
		renderer.texShader->setShaderValue(renderer.hTextColour, glm::vec4(1, 1, 0.3, 1)); //temp!!!
		renderer.texShader->setShaderValue(renderer.hTextOrthoMatrix, matrix2);
		renderer.drawTrisBuf(tmpQuadBuf, 0, tmpQuadBuf.numElements);
	}
	else
		CTextSprite::draw();
}
