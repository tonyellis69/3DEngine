#pragma once

#include <string>

#include <glm/glm.hpp>

#include "UI/fonts.h"
#include "buf2.h"


#include "vertBufs.h"

/** A simple class to encapsulate drawing text to the screen. */
class CDrawText {
public:
	CDrawText();
	void drawText(int x, int y, const std::string& text);
	void setPosition(int x, int y);
	void setText(const std::string& text);
	void setFont(const std::string& fontName);
	void setFont(CFont* font);
	void setColour(glm::vec4& colour);
	void draw();
	int getTextWidth();

	glm::mat4 orthoMatrix;
	CBuf2 templateQuad;
	CRenderTexture imageBuf; ///<The texture holding the rendered text.


private:
	void writeToBuffer();
	void updateBufferQuad();
	CFont* font;
	glm::vec2 pos;

	CBuf2 quadVerts;



	std::string text;
	glm::vec4 textColour = { 1,1,1,1 };

	glm::vec2 size;

};