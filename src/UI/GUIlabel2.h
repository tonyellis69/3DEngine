#pragma once

#include "GUIbase.h"
#include "font.h"
#include "textBuf.h"

/** A class to render text to the screen. */
class CGUIlabel2 : public CGUIbase {
public:
	CGUIlabel2(int x, int y, int w, int h);
	bool OnLMouseDown(const  int mouseX, const  int mouseY, int key) { return false; };
	bool OnMouseMove(const  int mouseX, const  int mouseY, int key) { return false; };
	bool OnLMouseUp(const  int mouseX, const  int mouseY) { return false; };

	void setFont(CFont* font);
	void setFont(const std::string& fontName);
	void setText(std::string newText);
	//void setTextColour(float r, float g, float b, float a);
	void setTextColour(UIcolour colour);
	void setMultiLine(bool onOff);
	void setTextColour(glm::vec4 colour);
	void setTextAlignment(TTextAlign align);
	void setLeftAlignIndent(int indent);
	TTextAlign getJustification();
	void DrawSelf();
	//void setBorderOn(bool onOff);
	void setLocalDimensions(int x, int y, int w, int h);
	void updateAppearance();
	int getTextWidth();
	void calcLineOffset();

	void calcLineRenderedWidth();

	void renderText();
	int getNextLineStart(int lineStart);

	std::string& getText();


	CTextBuffer textBuf;
	//std::string text;
	glm::i32vec2 renderOffset;///<Due to centering, multiline, etc.
	int lineRenderedWidth = 0;
	TTextAlign textAlign; ///<Records whether the text is centred, etc, in its area.
	bool multiLine;

	textRec textData;

	
	int leftAlignIndent; ///<How much left-aligned text is indented.

	bool resizeToFit = true;

	};