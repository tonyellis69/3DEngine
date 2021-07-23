#include "drawText.h"

#include "glm\gtc\matrix_transform.hpp"

#include "vertBufs.h"
#include "renderer.h"

CDrawText::CDrawText() {
	font = fnt::get("sysFont");
	pos = { 0, 0 };
}

void CDrawText::drawText(int x, int y, const std::string& text) {
	setText(text);
	setPosition(x, y);
}

void CDrawText::setPosition(int x, int y) {
	
	if (x != pos.x || y != pos.y) {
		pos = { (float)x, (float)y };
		updateBufferQuad();
	}
}


void CDrawText::setText(const std::string& text) {
	if (this->text == text)
		return;

	this->text = text;

	unsigned int numChars = text.size();
	std::vector<vBuf::T2DtexVert> verts(numChars * 4);
	std::vector<unsigned short> index(numChars * 5);

	unsigned int v = 0; unsigned int i = 0;
	glm::vec2 blCorner = { 0,0 };

	for (unsigned int c = 0; c < numChars; c++) {
		glyph* glyph = font->table[text[c]];

		verts[v].v = blCorner; //A
		verts[v + 1].v = blCorner + glm::vec2(glyph->width, 0.0f); //B
		verts[v + 2].v = blCorner + glm::vec2(0.0f, glyph->height); //C
		verts[v + 3].v = blCorner + glm::vec2(glyph->width, glyph->height); //D
		verts[v].tex = glm::vec2(glyph->u, glyph->t);
		verts[v + 1].tex = glm::vec2(glyph->s, glyph->t);
		verts[v + 2].tex = glm::vec2(glyph->u, glyph->v);
		verts[v + 3].tex = glm::vec2(glyph->s, glyph->v);

		index[i] = v + 2; index[i + 1] = v; index[i + 2] = v + 3; index[i + 3] = v + 1;
	//	index[i] = v; index[i + 1] = v + 2; index[i + 2] = v + 1; index[i + 3] = v + 3;
		index[i + 4] = USHRT_MAX;

		v += 4; i += 5;
		blCorner += glm::vec2(glyph->width, 0);
	}

	quadVerts.storeVerts(verts,index, 2, 2);

	size = { blCorner.x  , font->lineHeight };

	writeToBuffer();
	updateBufferQuad();
}

void CDrawText::setFont(const std::string& fontName) {
	setFont(fnt::get(fontName));
}

void CDrawText::setFont(CFont* font) {
	if (this->font != font) {
		this->font = font;
		writeToBuffer();
		updateBufferQuad();
	}
}

void CDrawText::setColour(glm::vec4& colour) {
	textColour = colour;
	writeToBuffer();
	updateBufferQuad();
}


void CDrawText::draw() {
	renderer.setShader(renderer.texShader);
	renderer.attachTexture(0, imageBuf.handle);
	renderer.texShader->setTextureUnit(renderer.hTexTexture, 0);
	renderer.texShader->setShaderValue(renderer.hTexMVP, orthoMatrix);
	renderer.drawTriStripBuf(templateQuad);


}

int CDrawText::getTextWidth()
{
	return size.x;
}

/** Write the text to a single quad for optimisation. */
void CDrawText::writeToBuffer() {
	if (quadVerts.numVerts == 0)
		return;

	//render glyphs to a texture
	imageBuf.resize(size.x, size.y);
	orthoMatrix = glm::ortho<float>(0, (float)size.x, 0, (float)size.y);
	renderer.setShader(renderer.textShader);
	renderer.attachTexture(0, font->texture); //attach texture to textureUnit (0)
	renderer.textShader->setTextureUnit(renderer.hTextTexture, 0);
	renderer.textShader->setShaderValue(renderer.hTextColour, textColour);
	renderer.textShader->setShaderValue(renderer.hTextOrthoMatrix, orthoMatrix);

	renderer.renderToTextureTriStrip(quadVerts, imageBuf);
}

void CDrawText::updateBufferQuad() {
	if (quadVerts.numVerts == 0)
		return;

	//prepare a quad for displaying the texture.
	std::vector<vBuf::T2DtexVert> corners(4);

	corners[0].v = pos;  //A
	corners[1].v = pos + glm::vec2(size.x, 0); //B  1,0
	corners[2].v = pos + glm::vec2(0, size.y); //C  0,1
	corners[3].v = pos + glm::vec2(size.x, size.y); //D  1,1
	//NB the top-left origin used for 2D flips Y. By pre-flipping quads we ensure they end up facing the right way

	corners[0].tex = glm::vec2(0);
	corners[1].tex = glm::vec2(1, 0);
	corners[2].tex = glm::vec2(0, 1);
	corners[3].tex = glm::vec2(1, 1);

	std::vector<unsigned int> index = { 2,3,0,1 };
	templateQuad.storeVerts(corners, index, 2, 2);

	orthoMatrix = glm::ortho<float>(0, (float)renderer.Width , (float)renderer.Height, 0 );

}

