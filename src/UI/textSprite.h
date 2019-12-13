#pragma once

#include <string>

#include <glm/glm.hpp>

#include "font.h"
#include "../renderer/buf.h"


/** Defines a rectangular block of pre-rendered text. */
class CTextSprite {
public:
	CTextSprite(int posX, int posY);
	int makeTextVerts(const std::string& text, CFont* font);
	void renderText(CRenderTexture& buf, glm::vec4& colour);

	unsigned int bufId;
	glm::i32vec2 positionOnPage;
	glm::i32vec2 size;
	int textObj;
	int textPos;

private:

	CBuf tmpVertBuf;
	CRenderer* pRenderer;
	CFont* font;

};