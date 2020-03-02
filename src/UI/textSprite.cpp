#include "textSprite.h"

#include "../vertBufs.h"
#include "../renderer/renderer.h"

#include "../utils/log.h"

#include "glm\gtc\matrix_transform.hpp"

CTextSprite::CTextSprite() {
	pRenderer = &CRenderer::getInstance();
	bufId = { INT_MAX,INT_MAX };
	causesNewLine = false;
}

CTextSprite::~CTextSprite() {
	if (bufId.x != INT_MAX) {
		callbackObj->freeSpriteImageSpace(bufId);
		//callbackObj->freeHotTextSprite(this);
	}
}

/** Draw this sprite's text to the given texture. */
void CTextSprite::createTextImage(CRenderTexture& texture) {
	if (size.x == 0)
		return;
	bufId = callbackObj->reserveSpriteImageSpace(size);
	renderTextQuads(texture, bufId , textColour);
	freeQuadBuffer();
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

	tmpQuadBuf.storeVertexes(textQuads.data(), sizeof(vBuf::T2DtexVert) * textQuads.size(), textQuads.size());
	tmpQuadBuf.storeIndex(textQuadsIndex.data(), textQuadsIndex.size());
	tmpQuadBuf.storeLayout(2, 2, 0, 0);

	this->font = font;
	size = glm::i32vec2(blCorner.x, font->lineHeight);
}

/** Render our temporary text quads to a texture for storage. */
void CTextSprite::renderTextQuads(CRenderTexture& storageTexture, glm::i32vec2& storageId, glm::vec4& colour) {
	glm::mat4 storageLocation = glm::translate(glm::mat4(1), glm::vec3( storageId, 0));
	glm::mat4 storageMatrix = glm::ortho<float>(0, (float)storageTexture.width, 0, storageTexture.height)
		* storageLocation;

	pRenderer->setShader(pRenderer->textShader);
	pRenderer->attachTexture(0, font->texture); //attach texture to textureUnit (0)
	pRenderer->texShader->setTextureUnit(pRenderer->hTextTexture, 0);
	pRenderer->texShader->setShaderValue(pRenderer->hTextColour, colour);
	pRenderer->texShader->setShaderValue(pRenderer->hTextOrthoMatrix, storageMatrix);
	pRenderer->renderToTextureTris(tmpQuadBuf, storageTexture);
}

/** The texture representing the finished page. */
void CTextSprite::setPageTexture(CRenderTexture* pageTexture) {
	this->pageTexture = pageTexture;
}

/** Use the given orthographic matrix when drawing this sprite's cached text image to the page. */
void CTextSprite::setPageOthoMatrix(glm::mat4* pageOrthoViewMatrix) {
	this->pageOrthoViewMatrix = pageOrthoViewMatrix;
}

/** Use the given shader to draw this sprite's cached text image to the page. */
void CTextSprite::setShader(TTextSpriteShader* shader) {
	this->textSpriteShader = shader;
}

/** Draw this sprite's cached text image to the page texture.*/
void CTextSprite::draw() {
	if (bufId.y == INT_MAX)
		return; //TO DO: do this more elegantly with a virtual func?

	textSpriteShader->shader->setShaderValue(textSpriteShader->hOrthoMatrix, matrix);
	textSpriteShader->shader->setShaderValue(textSpriteShader->hOffset, glm::vec2(bufId));
	textSpriteShader->shader->setShaderValue(textSpriteShader->hSize, glm::vec2(size));

	textSpriteShader->shader->setShaderValue(textSpriteShader->hAlpha, 0.0f);

	pRenderer->drawTriStripBuf(*pRenderer->screenQuad);
}

/** Modify the vertical position of this sprite on the page by the given amount. */
bool CTextSprite::adjustYPos(int change) {
	positionOnPage.y += change;
	updateMatrix();
	return true;
}

/** Once we've cached this sprite's text image, we don't need the quads that created it. */
void CTextSprite::freeQuadBuffer() {
	tmpQuadBuf.freeMem();
}

/** Recalculate the matrix that defines this sprite's size and position in page space. */
void CTextSprite::updateMatrix() {
	glm::vec3 spriteOriginDist = glm::vec3(size, 0) * 0.5f;
	glm::vec3 spriteTranslation = spriteOriginDist + glm::vec3(positionOnPage, 0);

	matrix = glm::translate(glm::mat4(1), spriteTranslation);
	matrix = glm::scale(matrix, spriteOriginDist);
	matrix = *pageOrthoViewMatrix * matrix;
}


//////////////////////////Hot text sprite stuff

CHotTextSprite::~CHotTextSprite() {
	if (bufId.x != INT_MAX) {
		//callbackObj->freeSpriteImageSpace(bufId);
		//callbackObj->freeSpriteImageSpace(bufId2);
		callbackObj->freeHotTextSprite(this);
	}
}

/** Draw the two alternating text images to the given storage texture. */
void CHotTextSprite::createTextImage(CRenderTexture& storageTexture) {
	if (size.x == 0)
		return;

	bufId = callbackObj->reserveSpriteImageSpace(size);
	renderTextQuads(storageTexture, bufId, textColour);

	bufId2 = callbackObj->reserveSpriteImageSpace(size);
	renderTextQuads(storageTexture, bufId2, hotTextColour);
	freeQuadBuffer();
}

void CHotTextSprite::setTextColour(glm::vec4& colour) {
	textColour = colour;
	hotTextColour = callbackObj->getHotTextColour();
}


void CHotTextSprite::draw() {
	textSpriteShader->shader->setShaderValue(textSpriteShader->hOrthoMatrix, matrix);
	textSpriteShader->shader->setShaderValue(textSpriteShader->hOffset, glm::vec2(bufId));
	textSpriteShader->shader->setShaderValue(textSpriteShader->hOffset2, glm::vec2(bufId2));
	textSpriteShader->shader->setShaderValue(textSpriteShader->hSize, glm::vec2(size));

	
	float alpha;
	

	//give the shader offsets for both sprite images
	
	//get period for hotId (getter can multiply it up
	//get working as alpha
	//then use as mix between two samples

	alpha = callbackObj->getHotPeriod(hotId);

	textSpriteShader->shader->setShaderValue(textSpriteShader->hAlpha, alpha);

	pRenderer->drawTriStripBuf(*pRenderer->screenQuad);
}

void CHotTextSprite::setHotId(unsigned int hotId)  {
	this->hotId = hotId;
}

unsigned int CHotTextSprite::getHotId() {
	return hotId;
}
