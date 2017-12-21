#pragma once

#include "GUIlabel2.h"

struct TTextPos {
	int textObj;
	int textStart;
	int renderStartX;

};

struct TTextChunk {
	int textLength;
	bool causesNewLine; 

	TTextPos nextTextPos;
};


/** A more versatile multiline text display class. */
class CGUIrichText : public CGUIlabel2 {
public:
	CGUIrichText(int x, int y, int w, int h);
	void DrawSelf();
	void setFont(CFont* newFont);
	void setTextColour(float r, float g, float b, float a);
	void setTextColour(UIcolour colour);
	void setText(std::string newText);
	void appendText(std::string newText);
	void scroll(int direction);

	int getNextLineStart(int lineStart, int& textPos);

	void renderText();
//	void renderText2();
	TTextChunk getNextTextChunk(TTextPos & textPos);
	int overrun;

	

	std::vector<textRec> textObjs; ///<The complete text of this control.
	int currentTextObj;
	int firstVisibleObject; ///<Index of the first object we need to draw.
	int firstVisibleText; ///<Position in the top visible object of the first text to display.
};