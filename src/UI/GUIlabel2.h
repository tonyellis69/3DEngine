#pragma once

#include "GUIbetterBase.h"
#include "font.h"
#include "textBuf.h"

/** A class to render text to the screen. */
class CGUIlabel2 : public CGUIbetterBase {
public:
	CGUIlabel2(int x, int y, int w, int h);
	void setFont(CFont& font);
	void setText(std::string newText);
	void setTextColour(float r, float g, float b, float a);
	void setTextColour(UIcolour colour);
	void setMultiLine(bool onOff);
	void DrawSelf();

	CTextBuffer textBuf;
	glm::vec4 textColour;
};