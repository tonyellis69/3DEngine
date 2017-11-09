#pragma once

#include <string>

#include "glm\glm.hpp"

#include "..\TexFont.h"
#include "..\renderer\renderer.h"

#include "renderDrawFuncs.h"

/** A class that maintains a rectangular buffer of rendered text for displaying. */

class CTextBuffer {
public:
	CTextBuffer();
	void setSize(int w, int h);
	void setText(std::string& str);
	void setFont(CTexFont& newFont);
	void setTextColour(glm::vec4& newColour);
	void renderText();

	CRenderTexture textTexture; ///<The texture holding the rendered text.

private:
	void writeToTexture(CBuf& buf);

	glm::i32vec2 size;
	CRenderer* pRenderer;
	std::string text;
	CTexFont* font;
	glm::vec4 textColour;
};