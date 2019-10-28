#pragma once

#include "GUIbase.h"
#include "font.h"
#include "textBuf.h"

/** A class to render text to the screen. */
class CGUIlabel : public CGUIbase {
public:
	CGUIlabel(const std::string& text);
	CGUIlabel(const std::string& text, unsigned int styleWord);
	//CGUIlabel(int x, int y, int w, int h);
	void applyStyleSheet();
	void setFont(CFont* font);
	void setText(std::string newText);
	void setTextColour(UIcolour colour);
	void setMultiLine(bool onOff);
	void setTextColour(glm::vec4 colour);
	void setTextAlignment(TTextAlign align);
	void setLeftAlignIndent(int indent);
	TTextAlign getJustification();
	void DrawSelf();
	void updateAppearance();
	int getTextWidth();
	void calcLineOffset();

	void calcLineRenderedWidth();

	void renderText();
	int getNextLineStart(int lineStart);

	std::string& getText();
	void resizeToFit();

	CTextBuffer textBuf;
	//std::string text;
	glm::i32vec2 renderOffset;///<Due to centering, multiline, etc.
	int lineRenderedWidth;
	TTextAlign textAlign; ///<Records whether the text is centred, etc, in its area.
	bool multiLine;

	textRec textData;

	
	int leftAlignIndent; ///<How much left-aligned text is indented.

	static CGUIlabel* make(std::string txt) {
		return new CGUIlabel(txt);
		};


	//std::unique_ptr<CGUIlabel*> add2(const std::string& text, int style)   const override {
	//	return std::make_unique<CGUIlabel*>(*this);
		//return new CGUIlabel(text);
	//}


	~CGUIlabel() {};

	};