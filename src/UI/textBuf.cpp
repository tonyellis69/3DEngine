#include "textBuf.h"

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"



CTextBuffer::CTextBuffer() {
	pRenderer = &CRenderer::getInstance();
	textColour = glm::vec4(1, 1, 1, 1);
	multiLine = false;
	TextAlign = tleft;
	font = NULL;
	cursorTextPos = 0;
	drawBorder = glm::i32vec2(5,0);
}

/** Set the absolute dimensions, in pixels, of the buffer texture.*/
void CTextBuffer::setSize(int w, int h) {
	size = glm::i32vec2(w, h);
	textTexture.resize(w, h);
	drawSpace = size - drawBorder * 2;
}

void CTextBuffer::setText(std::string & str) {
	text = str; 
	cursorTextPos = text.size();
	renderText();
}

void CTextBuffer::setFont(CFont* newFont) {
	font = newFont;
	glyphHeight = (float)font->lineHeight; 
	renderText();
}

void CTextBuffer::setTextColour(glm::vec4 & newColour) {
	textColour = newColour;
}

/** Draw the current text to the buffer (texture). */
void CTextBuffer::renderText() {
	if (!font) {
		std::cerr << "\nAttempt to render to textbuffer before font assigned.";
		return;
	}
	if (!text.size())
		return;

	vector<vBuf::T2DtexVert> chars;
	vector<unsigned int> index;
	chars.resize(text.size() * 4);
	overrun = 0;

	glm::vec2 blCorner = glm::vec2(-1.0f, 0.0f);

	int v = 0; float lineWidth = 0; int nextLineStart = text.size();
	if (multiLine)
		nextLineStart = nextLineBreak(0);
	glyph* glyph;
	for (int c = 0; c < text.size(); c++) {
		if (c == nextLineStart) {
			lineWidth = 0;
			blCorner = glm::vec2(0, blCorner.y + glyphHeight);
			nextLineStart = nextLineBreak(c) ;
		}
		if (text[c] != '\n') {
			glyph = font->table[text[c]];
			//construct quads
			chars[v].v = blCorner; //A
			chars[v + 1].v = blCorner + glm::vec2(glyph->width, 0.0f); //B
			chars[v + 2].v = blCorner + glm::vec2(0.0f, -glyph->height); //C
			chars[v + 3].v = blCorner + glm::vec2(glyph->width, -glyph->height); //D
			chars[v].tex = glm::vec2(glyph->u, glyph->v);
			chars[v + 1].tex = glm::vec2(glyph->s, glyph->v);
			chars[v + 2].tex = glm::vec2(glyph->u, glyph->t);
			chars[v + 3].tex = glm::vec2(glyph->s, glyph->t);

			index.push_back(v + 2); index.push_back(v + 3); index.push_back(v);
			index.push_back(v); index.push_back(v + 3); index.push_back(v + 1);
			v += 4;
			blCorner += glm::vec2(glyph->width, 0);
			lineWidth += glyph->width;
		}
		
	
	}

	if (multiLine && blCorner.y + font->lineHeight  > drawSpace.y) {
		overrun = (blCorner.y + font->lineHeight) - drawSpace.y;
	}

	CBuf buf;
	buf.storeVertexes(chars.data(), sizeof(vBuf::T2DtexVert) * chars.size(), chars.size());
	buf.storeIndex(index.data(), index.size());
	buf.storeLayout(2, 2, 0, 0);

	textTexture.clear();
	writeToTexture(buf, lineWidth);

	calcCursorPosition();
}

/** Returns the point at which whitespace lets us wrap the text onto the next line. */
int CTextBuffer::nextLineBreak(int lineStart) {
	int breakDist = text.size(); float dist =  drawBorder.x;
	//while there are characters, when we reach a word break record it, until we go over the allotted width;
	int c = lineStart;
	while (  dist < drawSpace.x) {
		if (text[c] == '\n') {
			//text.erase(c, 1);
			return c+1;
		}

		if (isspace(text[c]))
			breakDist = c+1;
		dist += font->table[text[c]]->width;
		c++;
		//if (c == 83)
		//	int j = 0;
		if (c >= text.size())
			return c;
	}
	return breakDist;
}

void CTextBuffer::setMultiLine(bool onOff) {
	multiLine = onOff;
}

void CTextBuffer::setHorizontalAlignment(TTextAlign align) {
	TextAlign = align;
	renderText();
}

TTextAlign CTextBuffer::getJustification() {
	return TextAlign;
}

float CTextBuffer::getTextWidth() {
	float dist = drawBorder.x;
	for (int c=0; c < text.size(); c++) {
		dist += font->table[text[c]]->width;
	}
	return dist;
}


CBuf * CTextBuffer::getCursorPos() {
	return &cursor;
}

void CTextBuffer::calcCursorPosition() {
	//lazily assuming single line text for now
	float dist = drawBorder.x;
	for (int c = 0; c < cursorTextPos; c++) {
		dist += font->table[text[c]]->width;
	}
	float halfHeight = drawSpace.y / 2.0f;
	float yOffset = int(0 - (glyphHeight / 2.0f) + halfHeight);
	vBuf::T2DtexVert cursorVert[2];
	cursorVert[0].v = glm::vec2(dist, yOffset);
	cursorVert[1].v = glm::vec2(dist, yOffset + font->lineHeight);
	cursor.storeVertexes(cursorVert, sizeof(cursorVert), 2);
	cursor.storeLayout(2, 2, 0, 0);
}

/** For now, lazily assume one-line text. */
void CTextBuffer::moveCursor(int x, int y) {
	if (x < 0 && cursorTextPos > 0)
		cursorTextPos--;
	if (x > 0 && cursorTextPos < text.size())
		cursorTextPos++;

	calcCursorPosition();
}

/** Delete the character at the cursor position. */
void CTextBuffer::backSpace() {
	if (cursorTextPos > 0) {
		text.erase(cursorTextPos - 1, 1);
		cursorTextPos--;
		renderText();
	}
}

/** Insert the given string at the cursor position. */
void CTextBuffer::insert(std::string inText) {
	text.insert(cursorTextPos, inText);
	cursorTextPos++;
	renderText();
}

std::string CTextBuffer::getText() {
	return text;
}

/** Append newText to the currently rendered text. */
void CTextBuffer::appendText(std::string newText) {
	text += newText;
	renderText();
}

void CTextBuffer::setDrawBorder(int x, int y) {
	drawBorder = glm::i32vec2(x, y);
	drawSpace = size - drawBorder * 2;
	renderText();
}

int CTextBuffer::getOverrun() {
	return overrun;
}

/** Draw textLine at the offset x,y on the text buffer. */
void CTextBuffer::renderTextAt(int x, int y, std::string textLine) {
	if (!textLine.size())
		return;

	vector<vBuf::T2DtexVert> chars;
	vector<unsigned int> index;
	chars.resize(textLine.size() * 4);
	overrun = 0;

	glm::vec2 blCorner = glm::vec2(x, y);

	int v = 0; 
	//float lineWidth = 0; int nextLineStart = text.size();
	//if (multiLine)
	//	nextLineStart = nextLineBreak(0);
	glyph* glyph;
	for (int c = 0; c < textLine.size(); c++) {
		/*if (c == nextLineStart) {
			lineWidth = 0;
			blCorner = glm::vec2(0, blCorner.y + glyphHeight);
			nextLineStart = nextLineBreak(c);
		} */
		if (textLine[c] != '\n') {
			glyph = font->table[textLine[c]];
			//construct quads
			chars[v].v = blCorner; //A
			chars[v + 1].v = blCorner + glm::vec2(glyph->width, 0.0f); //B
			chars[v + 2].v = blCorner + glm::vec2(0.0f, -glyph->height); //C
			chars[v + 3].v = blCorner + glm::vec2(glyph->width, -glyph->height); //D
			chars[v].tex = glm::vec2(glyph->u, glyph->v);
			chars[v + 1].tex = glm::vec2(glyph->s, glyph->v);
			chars[v + 2].tex = glm::vec2(glyph->u, glyph->t);
			chars[v + 3].tex = glm::vec2(glyph->s, glyph->t);

			index.push_back(v + 2); index.push_back(v + 3); index.push_back(v);
			index.push_back(v); index.push_back(v + 3); index.push_back(v + 1);
			v += 4;
			blCorner += glm::vec2(glyph->width, 0);
			//lineWidth += glyph->width;
		}
	}

	/*if (multiLine && blCorner.y + font->lineHeight  > drawSpace.y) {
		overrun = (blCorner.y + font->lineHeight) - drawSpace.y;
	} */

	CBuf buf;
	buf.storeVertexes(chars.data(), sizeof(vBuf::T2DtexVert) * chars.size(), chars.size());
	buf.storeIndex(index.data(), index.size());
	buf.storeLayout(2, 2, 0, 0);

	writeToTexture2(buf);

	calcCursorPosition();
}

void CTextBuffer::clearBuffer() {
	textTexture.clear();
}


/** Write the given series of text-quads to the storage texture. */
void CTextBuffer::writeToTexture(CBuf& glyphQuads, float lineWidth) {
	glm::vec2 halfSize = glm::vec2(drawSpace) / 2.0f;
	float xOffset = 0; float yOffset = 0 - (glyphHeight/2.0f);
	if (TextAlign == tcentred) {
		xOffset = (drawSpace.x - lineWidth) / 2.0f;
	}
	if (TextAlign == tright) {
		xOffset = drawSpace.x - lineWidth;
	}

	if (multiLine) {
		yOffset = halfSize.y - glyphHeight;
	}

	yOffset = int(yOffset) - drawBorder.y; 
	xOffset = int(xOffset) + drawBorder.x;

	glm::mat4 orthoMatrix = glm::ortho<float>(-xOffset, size.x-xOffset, -halfSize.y + yOffset, halfSize.y + yOffset);
	
	pRenderer->setShader(pRenderer->textShader);
	pRenderer->attachTexture(0,font->texture); //attach texture to textureUnit (0)
	pRenderer->texShader->setTextureUnit(pRenderer->hTextTexture, 0);
	pRenderer->texShader->setShaderValue(pRenderer->hTextColour, textColour);
	pRenderer->texShader->setShaderValue(pRenderer->hTextOrthoMatrix, orthoMatrix);

	pRenderer->renderToTextureTris(glyphQuads, textTexture);
}

void CTextBuffer::writeToTexture2(CBuf& glyphQuads) {
	glm::vec2 halfSize = glm::vec2(size) / 2.0f;
	float xOffset = 0; float yOffset = 0 - (glyphHeight / 2.0f);
	
	glm::mat4 orthoMatrix = glm::ortho<float>(0, size.x, -halfSize.y, halfSize.y);

	glm::vec4 test =  orthoMatrix * glm::vec4(50,18,0,0);

	pRenderer->setShader(pRenderer->textShader);
	pRenderer->attachTexture(0, font->texture); //attach texture to textureUnit (0)
	pRenderer->texShader->setTextureUnit(pRenderer->hTextTexture, 0);
	pRenderer->texShader->setShaderValue(pRenderer->hTextColour, textColour);
	pRenderer->texShader->setShaderValue(pRenderer->hTextOrthoMatrix, orthoMatrix);

	pRenderer->renderToTextureTris(glyphQuads, textTexture);
}
