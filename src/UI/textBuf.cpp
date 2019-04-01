#include "textBuf.h"

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"



CTextBuffer::CTextBuffer() {
	pRenderer = &CRenderer::getInstance();
}

/** Set the absolute dimensions, in pixels, of the buffer texture.*/
void CTextBuffer::setSize(int w, int h) {
	size = glm::i32vec2(w, h);
	textTexture.resize(w, h);
	clearBuffer(); //this fixes some corruption that otherwise crops up
}




/** Render the given text fragment immediately, without waiting for more. */
void CTextBuffer::renderTextAt(int x, int y, TLineFragDrawRec& drawData) {
	if (!drawData.text->size())
		return;

	init(false);


	addFragment(x, y, drawData);
	render();
}

void CTextBuffer::clearBuffer() {
	pRenderer->rendertToTextureClear(textTexture, glm::vec4(0, 0, 0, 0));
}


void CTextBuffer::writeToTexture2(CBuf& glyphQuads) {
	
	glm::vec2 halfSize = glm::vec2(size) / 2.0f;
	//float xOffset = 0; float yOffset = 0 - (textData.font->lineHeight / 2.0f);
	
	//glm::mat4 orthoMatrix = glm::ortho<float>(0, (float)size.x, -halfSize.y, halfSize.y);
	glm::mat4 orthoMatrix = glm::ortho<float>(0, (float)size.x, 0, size.y);

//	orthoMatrix = glm::translate<float>(orthoMatrix,glm::vec3(0, -halfSize.y + textData.font->lineHeight, 0));



	//glm::vec4 textColour = textData.style.colour;
	//textColour.a = 0.5;

	pRenderer->setShader(pRenderer->textShader);
	pRenderer->attachTexture(0, mDrawData->font->texture); //attach texture to textureUnit (0)
	pRenderer->texShader->setTextureUnit(pRenderer->hTextTexture, 0);
	pRenderer->texShader->setShaderValue(pRenderer->hTextColour, textColour);
	pRenderer->texShader->setShaderValue(pRenderer->hTextOrthoMatrix, orthoMatrix);
	
	pRenderer->renderToTextureTris(glyphQuads, textTexture);

}

/** Prepare to compose a buffer-full of text for rendering. */
void CTextBuffer::init(bool clearBuf) {
	if (clearBuf)
		clearBuffer(); 
	textQuads.clear();
	textQuadsIndex.clear();
}

/** Add this to the list of text quads to eventually draw. Return the x-coordinate we finish on.  */
int CTextBuffer::addFragment(int x, int y, TLineFragDrawRec& drawData) {
	textColour = drawData.textColour;
	this->mDrawData = &drawData;

	glm::vec2 blCorner = glm::vec2(x, y);

	int v = textQuads.size();
	glyph* glyph;
	unsigned int numChars = drawData.text->size();
	textQuads.resize(v + numChars * 4);
	string text = *drawData.text;
	for (unsigned int c = 0; c < numChars; c++) { 
		if (text[c]  != '\n')
		{
			glyph = drawData.font->table[text[c]];
			//construct quads
			textQuads[v].v = blCorner; //A
			textQuads[v + 1].v = blCorner + glm::vec2(glyph->width, 0.0f); //B
			textQuads[v + 2].v = blCorner + glm::vec2(0.0f, glyph->height); //C
			textQuads[v + 3].v = blCorner + glm::vec2(glyph->width, glyph->height); //D
			textQuads[v].tex = glm::vec2(glyph->u, glyph->t);
			textQuads[v + 1].tex = glm::vec2(glyph->s, glyph->t);
			textQuads[v + 2].tex = glm::vec2(glyph->u, glyph->v);
			textQuads[v + 3].tex = glm::vec2(glyph->s, glyph->v);

			textQuadsIndex.push_back(v ); textQuadsIndex.push_back(v + 3); textQuadsIndex.push_back(v + 2);
			textQuadsIndex.push_back(v+1); textQuadsIndex.push_back(v + 3); textQuadsIndex.push_back(v);
			v += 4;
			blCorner += glm::vec2(glyph->width, 0);
		}
	}
	return blCorner.x;
}

/** Render the currently accumulated text quads to our buffer. */
void CTextBuffer::render() {
	//CBuf buf;
	buf.storeVertexes(textQuads.data(), sizeof(vBuf::T2DtexVert) * textQuads.size(), textQuads.size());
	buf.storeIndex(textQuadsIndex.data(), textQuadsIndex.size());
	buf.storeLayout(2, 2, 0, 0);
	writeToTexture2(buf);

}

/** Returns true if buffer contains text yet to be rendered. */
bool CTextBuffer::notEmpty() {
	return textQuads.size();
}
