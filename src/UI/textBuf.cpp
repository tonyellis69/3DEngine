#include "textBuf.h"

#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\matrix_access.hpp"


 float xAdj = 0;
 float yAdj = 0;;

CTextBuffer::CTextBuffer()  {
	pRenderer = &CRenderer::getInstance();
	textColour = glm::vec4(1, 1, 1, 1);
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
	glm::vec2 blCorner = glm::vec2(-1.0f, baseLine) + glm::vec2(0, font->table[0]->Rect.height/2.0f) ;

	int v = 0;
	for (int c = 0; c < text.size(); c ++) {
		Glyph* glyph = font->table[text[c]]; 
		//construct quads
		chars[v].v = blCorner; //A
		chars[v+1].v = blCorner + glm::vec2(glyph->Rect.width, 0.0f); //B
		chars[v+2].v = blCorner + glm::vec2(0.0f, -glyph->Rect.height); //C
		chars[v+3].v = blCorner + glm::vec2(glyph->Rect.width, -glyph->Rect.height); //D
		chars[v].tex = glm::vec2(glyph->Rect.Map.u, glyph->Rect.Map.v);
		chars[v+1].tex = glm::vec2(glyph->Rect.Map.s, glyph->Rect.Map.v);
		chars[v+2].tex = glm::vec2(glyph->Rect.Map.u, glyph->Rect.Map.t);
		chars[v+3].tex = glm::vec2(glyph->Rect.Map.s, glyph->Rect.Map.t);

		index.push_back(v + 2); index.push_back(v + 3); index.push_back(v);
		index.push_back(v); index.push_back(v + 3); index.push_back(v + 1);

		blCorner += glm::vec2(glyph->Rect.width, 0) ;
		v += 4;
	}

	CBuf buf;
	buf.storeVertexes(chars.data(), sizeof(vBuf::T2DtexVert) * chars.size(), chars.size());
	buf.storeIndex(index.data(), index.size()); 
	buf.storeLayout(2, 2, 0, 0);

	writeToTexture(buf);
}

/** Write the given series of text-quads to the storage texture. */
void CTextBuffer::writeToTexture(CBuf& glyphQuads) {
	glm::vec2 halfSize = glm::vec2(size) / 2.0f;
	glm::mat4 orthoMatrix = glm::ortho<float>(0, size.x, -halfSize.y, halfSize.y);

	pRenderer->setShader(pRenderer->textShader);
	pRenderer->attachTexture(0, font->textureNo + 1); //attach texture to textureUnit (0)
	pRenderer->texShader->setTextureUnit(pRenderer->hTextTexture, 0);
	pRenderer->texShader->setShaderValue(pRenderer->hTextColour, textColour);
	pRenderer->texShader->setShaderValue(pRenderer->hTextOrthoMatrix, orthoMatrix);

	pRenderer->renderToTextureTris(glyphQuads, textTexture);
}
