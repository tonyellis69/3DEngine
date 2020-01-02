#include "textSprite.h"

#include "../vertBufs.h"
#include "../renderer/renderer.h"

#include "../utils/log.h"

#include "glm\gtc\matrix_transform.hpp"

CTextSprite::CTextSprite() {
	pRenderer = &CRenderer::getInstance();
	bufId = INT_MAX;
}

CTextSprite::~CTextSprite() {
	callbackObj->freeSpriteMemory(bufId);
}

void CTextSprite::createText(CRenderTexture& texture) {
	if (size.x == 0)
		return;
	bufId = callbackObj->reserveImageSpace(size);
	writeTextToBuffer(texture, textColour);
	freeVertBuffer();
}

void CTextSprite::setPosition(int posX, int posY) {
	positionOnPage = glm::i32vec2(posX, posY);
	updateMatrix();
}


/** Create the verts for the series of quads that rendering this text will require. */
int CTextSprite::makeTextVerts( const std::string& text, CFont* font) {
	
	std::vector<vBuf::T2DtexVert> textQuads;
	std::vector<unsigned int> textQuadsIndex;
	int v = 0;
	glm::vec2 blCorner(0, -size.y);
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

	tmpVertBuf.storeVertexes(textQuads.data(), sizeof(vBuf::T2DtexVert) * textQuads.size(), textQuads.size());
	tmpVertBuf.storeIndex(textQuadsIndex.data(), textQuadsIndex.size());
	tmpVertBuf.storeLayout(2, 2, 0, 0);

	this->font = font;
	return blCorner.x;
}

/** Render our text vertexes to our reserved block on the buffer. */
void CTextSprite::writeTextToBuffer(CRenderTexture& buf, glm::vec4& colour) {

	glm::mat4 posM = glm::translate(glm::mat4(1), glm::vec3(0,bufId, 0));
	

	glm::mat4 orthoMatrix = glm::ortho<float>(0, (float)buf.width, 0, buf.height);

	orthoMatrix = orthoMatrix * posM;

	pRenderer->setShader(pRenderer->textShader);
	pRenderer->attachTexture(0, font->texture); //attach texture to textureUnit (0)
	pRenderer->texShader->setTextureUnit(pRenderer->hTextTexture, 0);
	pRenderer->texShader->setShaderValue(pRenderer->hTextColour, colour);
	pRenderer->texShader->setShaderValue(pRenderer->hTextOrthoMatrix, orthoMatrix);

	pRenderer->renderToTextureTris(tmpVertBuf, buf);
	
}

void CTextSprite::setPageBuf(CRenderTexture* pageBuf) {
	this->pageBuf = pageBuf;
}

void CTextSprite::setOthoMatrix(glm::mat4* orthoMatrix) {
	this->orthoMatrix = orthoMatrix;
}

void CTextSprite::setShader(TLineBufferShader* shader) {
	this->shader = shader;
}

void CTextSprite::draw() {
	if (bufId == INT_MAX)
		return; //TO DO: do this more elegantly with a virtual func?

	shader->shader->setShaderValue(shader->hOrthoMatrix, matrix);
	shader->shader->setShaderValue(shader->hOffset, glm::vec2(0, bufId));
	shader->shader->setShaderValue(shader->hSize, glm::vec2(size));

	//pRenderer->renderToTextureTriStrip(*pRenderer->screenQuad, *pageBuf);
	pRenderer->drawTriStripBuf(*pRenderer->screenQuad);
}

bool CTextSprite::adjustYPos(int change) {
	positionOnPage.y += change;
	updateMatrix();
	return true;
}

void CTextSprite::freeVertBuffer()
{
	tmpVertBuf.freeMem();
}

void CTextSprite::updateMatrix() {
	glm::vec3 spriteOriginDist = glm::vec3(size, 0) * 0.5f;
	glm::vec3 spriteTranslation = spriteOriginDist + glm::vec3(positionOnPage, 0);

	matrix = glm::translate(glm::mat4(1), spriteTranslation);
	matrix = glm::scale(matrix, spriteOriginDist);
	matrix = *orthoMatrix * matrix;
}


//////////////////////////Hot text sprite stuff

CHotTextSprite::~CHotTextSprite() {
	callbackObj->freeSpriteMemory(bufId);
	callbackObj->freeSpriteMemory(bufId2);
}

void CHotTextSprite::createText(CRenderTexture& texture) {
	if (size.x == 0)
		return;

	bufId = callbackObj->reserveImageSpace(size);
	writeTextToBuffer(texture, textColour);

	bufId2 = callbackObj->reserveImageSpace(size);
	write2ndTextToBuffer(texture, hotTextColour);
	freeVertBuffer();
}

/** Render the text vertices to a second reserved block in the buffer. This is used
	for pulsing hot text, etc. */
void CHotTextSprite::write2ndTextToBuffer(CRenderTexture& spriteBuffer, glm::vec4& colour){
	glm::mat4 posM = glm::translate(glm::mat4(1), glm::vec3(0, bufId2, 0));


	glm::mat4 orthoMatrix = glm::ortho<float>(0, (float)spriteBuffer.width, 0, spriteBuffer.height);

	orthoMatrix = orthoMatrix * posM;

	pRenderer->setShader(pRenderer->textShader);
	pRenderer->attachTexture(0, font->texture); //attach texture to textureUnit (0)
	pRenderer->texShader->setTextureUnit(pRenderer->hTextTexture, 0);
	pRenderer->texShader->setShaderValue(pRenderer->hTextColour, colour);
	pRenderer->texShader->setShaderValue(pRenderer->hTextOrthoMatrix, orthoMatrix);

	pRenderer->renderToTextureTris(tmpVertBuf, spriteBuffer);
}

void CHotTextSprite::draw() {
	shader->shader->setShaderValue(shader->hOrthoMatrix, matrix);
	shader->shader->setShaderValue(shader->hOffset, glm::vec2(0, bufId2));
	shader->shader->setShaderValue(shader->hSize, glm::vec2(size));

	//give the shader offsets for both sprite images
	//and a transition value saying how far to blend between the two.

	//pRenderer->renderToTextureTriStrip(*pRenderer->screenQuad, *pageBuf);
	pRenderer->drawTriStripBuf(*pRenderer->screenQuad);
}
