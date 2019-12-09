#include "textSprite.h"

#include "../vertBufs.h"
#include "../renderer/renderer.h"

#include "glm\gtc\matrix_transform.hpp"

CTextSprite::CTextSprite(int posX, int posY) {
	this->positionOnPage = glm::i32vec2(posX,posY);
	pRenderer = &CRenderer::getInstance();
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
void CTextSprite::renderText(CRenderTexture& buf, glm::vec4& colour) {

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



