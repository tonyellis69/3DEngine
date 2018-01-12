#include "textBuf.h"

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"



CTextBuffer::CTextBuffer() {
	pRenderer = &CRenderer::getInstance();
	textData.textColour = glm::vec4(1, 1, 1, 1);
	textData.font = NULL;
	
}

/** Set the absolute dimensions, in pixels, of the buffer texture.*/
void CTextBuffer::setSize(int w, int h) {
	size = glm::i32vec2(w, h);
	textTexture.resize(w, h);
	
}

void CTextBuffer::setFont(CFont* newFont) {
	textData.font = newFont;
}

void CTextBuffer::setTextColour(glm::vec4 & newColour) {
	textData.textColour = newColour;
}




/** Draw textLine at the offset x,y on the text buffer. */
glm::i32vec2 CTextBuffer::renderTextAt(int x, int y, std::string textLine) {
	if (!textLine.size())
		return glm::i32vec2(0);

	vector<vBuf::T2DtexVert> chars;
	vector<unsigned int> index;
	chars.resize(textLine.size() * 4);

	glm::vec2 blCorner = glm::vec2(x, y);

	int v = 0; 
	glyph* glyph;
	for (unsigned int c = 0; c < textLine.size(); c++) {
		if (textLine[c] != '\n') {
			glyph = textData.font->table[textLine[c]];
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
		}
	}



	CBuf buf;
	buf.storeVertexes(chars.data(), sizeof(vBuf::T2DtexVert) * chars.size(), chars.size());
	buf.storeIndex(index.data(), index.size());
	buf.storeLayout(2, 2, 0, 0);

	writeToTexture2(buf);
	return blCorner;
}

void CTextBuffer::clearBuffer() {
	textTexture.clear();
}


void CTextBuffer::writeToTexture2(CBuf& glyphQuads) {
	glm::vec2 halfSize = glm::vec2(size) / 2.0f;
	float xOffset = 0; float yOffset = 0 - (textData.font->lineHeight / 2.0f);
	
	glm::mat4 orthoMatrix = glm::ortho<float>(0, (float)size.x, -halfSize.y, halfSize.y);

	orthoMatrix = glm::translate<float>(orthoMatrix,glm::vec3(0, -halfSize.y + textData.font->lineHeight, 0));

//	glm::vec4 test = orthoMatrix * glm::vec4(50, 18, 0, 0);

	pRenderer->setShader(pRenderer->textShader);
	pRenderer->attachTexture(0, textData.font->texture); //attach texture to textureUnit (0)
	pRenderer->texShader->setTextureUnit(pRenderer->hTextTexture, 0);
	pRenderer->texShader->setShaderValue(pRenderer->hTextColour, textData.textColour);
	pRenderer->texShader->setShaderValue(pRenderer->hTextOrthoMatrix, orthoMatrix);

	pRenderer->renderToTextureTris(glyphQuads, textTexture);
}
