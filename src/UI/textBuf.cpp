#include "textBuf.h"

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"



CTextBuffer::CTextBuffer() {
	// pRenderer = &renderer;
}

/** Set the absolute dimensions, in pixels, of the buffer texture.*/
void CTextBuffer::setSize(int w, int h) {
	size = glm::i32vec2(w, h);
	textTexture.resize(w, h);
	clearBuffer(); //this fixes some corruption that otherwise crops up
}


/** Render the given text fragment immediately, without waiting for more. */
void CTextBuffer::renderTextAt(int x, int y, TLineFragDrawRec& drawData) {
	renderFadeInTextAt(x, y, drawData, 1.0f);
}

/** Render the given text fragment immediately, using the given fade-in x. */
void CTextBuffer::renderFadeInTextAt(int x, int y, TLineFragDrawRec& drawData, float fadeInX) {
	if (!drawData.text->size())
		return;
	onSpawn(false);
	addFragment(x, y, drawData);
	render(fadeInX);
}

void CTextBuffer::clearBuffer() {
	
	//	return;
	renderer.rendertToTextureClear(textTexture, glm::vec4(0, 0, 0, 0));
	renderer.endRenderToTexture(); 
}


void CTextBuffer::writeToTexture2(CBuf2& glyphQuads, float fadeInX) {
	glm::mat4 orthoMatrix = glm::ortho<float>(0, (float)size.x, 0, size.y);

	renderer.setShader(renderer.textShader);
	renderer.attachTexture(0, mDrawData->font->texture); //attach texture to textureUnit (0)
	renderer.texShader->setTextureUnit(renderer.hTextTexture, 0);
	renderer.texShader->setShaderValue(renderer.hTextColour, textColour);
	renderer.texShader->setShaderValue(renderer.hTextOrthoMatrix, orthoMatrix);
	renderer.texShader->setShaderValue(renderer.hFadeInX, fadeInX);

	renderer.renderToTextureTris(glyphQuads, textTexture);
}

/** Prepare to compose a buffer-full of text for rendering. */
void CTextBuffer::onSpawn(int clearBuf) {
	if (clearBuf == CLEAR_EXISTING_IMAGE)
		clearBuffer(); 
	textQuads.clear();
	textQuadsIndex.clear();
}

/** add this to the list of text quads to eventually draw. Return the x-coordinate we finish on.  */
int CTextBuffer::addFragment(int x, int y, TLineFragDrawRec& drawData) {
	textColour = drawData.textColour;
	this->mDrawData = &drawData;

	glm::vec2 blCorner = glm::vec2(x, y);

	int v = textQuads.size();
	glyph* glyph;
	unsigned int numChars = drawData.text->size();
	textQuads.resize(v + numChars * 4);
	std::string text = *drawData.text;
	for (unsigned int c = 0; c < numChars; c++) { 
		if (text[c]  != '\n')
		{
			glyph = drawData.font->table[(unsigned char) text[c]];
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
	render(1.0f);
}

void CTextBuffer::render(float fadeInX) {
	//buf.storeVertexes(textQuads.data(), sizeof(vBuf::T2DtexVert) * textQuads.size(), textQuads.size());
	//buf.storeIndex(textQuadsIndex.data(), textQuadsIndex.size());
	//buf.storeLayout(2, 2, 0, 0);

	buf.storeVerts(textQuads, textQuadsIndex, 2, 2);

	writeToTexture2(buf, fadeInX);

}

/** Returns true if buffer contains text yet to be rendered. */
bool CTextBuffer::notEmpty() {
	return textQuads.size();
}
