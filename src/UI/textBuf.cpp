#include "textBuf.h"

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"



CTextBuffer::CTextBuffer() {
	pRenderer = &CRenderer::getInstance();
	textColour = glm::vec4(1, 1, 1, 1);
	multiLine = false;
	TextAlign = tleft;
}

void CTextBuffer::setSize(int w, int h) {
	size = glm::i32vec2(w, h);

	//resize texture
	textTexture.resize(w, h);

}

void CTextBuffer::setText(std::string & str) {
	text = str;
	renderText();
}

void CTextBuffer::setFont(CTexFont & newFont) {
	font = &newFont;
	glyphHeight = font->table[0]->Rect.height; //TO DO: kludge, do beter
	renderText();
}

void CTextBuffer::setTextColour(glm::vec4 & newColour) {
	textColour = newColour;
}

/** Draw the current text to the buffer (texture). */
void CTextBuffer::renderText() {
	
	vector<vBuf::T2DtexVert> chars;
	vector<unsigned int> index;
	chars.resize(text.size() * 4);

	float baseLine = 0;

	glm::vec2 blCorner = glm::vec2(-1.0f, 0.0f);

	int v = 0; float lineWidth = 0; int nextBreak = text.size();
	if (multiLine)
		nextBreak = nextLineBreak(0);
	
	for (int c = 0; c < text.size(); c++) {
		Glyph* glyph = font->table[text[c]];
		//construct quads
		chars[v].v = blCorner; //A
		chars[v + 1].v = blCorner + glm::vec2(glyph->Rect.width, 0.0f); //B
		chars[v + 2].v = blCorner + glm::vec2(0.0f, -glyph->Rect.height); //C
		chars[v + 3].v = blCorner + glm::vec2(glyph->Rect.width, -glyph->Rect.height); //D
		chars[v].tex = glm::vec2(glyph->Rect.Map.u, glyph->Rect.Map.v);
		chars[v + 1].tex = glm::vec2(glyph->Rect.Map.s, glyph->Rect.Map.v);
		chars[v + 2].tex = glm::vec2(glyph->Rect.Map.u, glyph->Rect.Map.t);
		chars[v + 3].tex = glm::vec2(glyph->Rect.Map.s, glyph->Rect.Map.t);

		index.push_back(v + 2); index.push_back(v + 3); index.push_back(v);
		index.push_back(v); index.push_back(v + 3); index.push_back(v + 1);

		if (c == nextBreak) {
			lineWidth = 0;
			blCorner = glm::vec2(0, blCorner.y + glyphHeight);
			nextBreak = nextLineBreak(c+1);
		}
		else {
			blCorner += glm::vec2(glyph->Rect.width, 0);
			lineWidth += glyph->Rect.width;
		}
		v += 4;
	}

	CBuf buf;
	buf.storeVertexes(chars.data(), sizeof(vBuf::T2DtexVert) * chars.size(), chars.size());
	buf.storeIndex(index.data(), index.size());
	buf.storeLayout(2, 2, 0, 0);

	writeToTexture(buf, lineWidth);
}

/** Returns the point at which whitespace lets us wrap the text onto the next line. */
int CTextBuffer::nextLineBreak(int lineStart) {
	int breakDist = text.size(); float dist = 0;
	//while there are characters, when we reach a break record it, until we go over the allotted width;
	int c = lineStart;
	while (c < text.size() && dist < size.x) {
		
		if (isspace(text[c]))
			breakDist = c;
		dist += font->table[text[c]]->Rect.width;
		c++;
	}
	return breakDist;
}

void CTextBuffer::setMultiLine(bool onOff) {
	multiLine = onOff;
}

/** Write the given series of text-quads to the storage texture. */
void CTextBuffer::writeToTexture(CBuf& glyphQuads, float lineWidth) {

	glm::vec2 halfSize = glm::vec2(size) / 2.0f;
	float xOffset = 0; float yOffset = 0 - (glyphHeight/2.0f);
	if (TextAlign == tcentred) {
		xOffset = (size.x - lineWidth) / 2.0f;
	}
	if (TextAlign == tright) {
		xOffset = size.x - lineWidth;
	}

	if (multiLine) {
		yOffset = halfSize.y - glyphHeight;
	}

	glm::mat4 orthoMatrix = glm::ortho<float>(-xOffset, size.x-xOffset, -halfSize.y + yOffset, halfSize.y + yOffset);
	
	pRenderer->setShader(pRenderer->textShader);
	pRenderer->attachTexture(0, 0 + 1); //attach texture to textureUnit (0)
	pRenderer->texShader->setTextureUnit(pRenderer->hTextTexture, 0);
	pRenderer->texShader->setShaderValue(pRenderer->hTextColour, textColour);
	pRenderer->texShader->setShaderValue(pRenderer->hTextOrthoMatrix, orthoMatrix);

	pRenderer->renderToTextureTris(glyphQuads, textTexture);
}
