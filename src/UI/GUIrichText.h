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


struct TRichTextRec : textRec {
	void findNewlines();
	std::vector<int> newLines;
};

/** A partial or complete line of text, with its rendered (x) dimensions. */
struct TLineFragment {
	int textObj;
	int textPos;
	int textLength;
	int renderStartX;
	int renderEndX;
	bool causesNewLine;
	bool finalFrag;
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
	TTextChunk getNextTextChunk(TTextPos & textPos);
	TLineFragment getNextLineFragment(const TLineFragment & currentLineFragment);

	int overrun;

	

	std::vector<TRichTextRec> textObjs; ///<The complete text of this control.
	int currentTextObj;
	int firstVisibleObject; ///<Index of the first object we need to draw.
	int firstVisibleText; ///<Position in the top visible object of the first text to display.
};