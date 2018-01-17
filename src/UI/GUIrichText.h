#pragma once

#include "GUIlabel2.h"


struct TRichTextRec : textRec {
	TRichTextRec();
	void findNewlines();
	std::vector<int> newLines;
	int hotTextId;
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

/** Records the position of a hot text fragment, for mouseover checks. */
struct THotTextFragment {
	int renderStartX;
	int renderStartY;
	int renderEndX;
	int renderEndY;
	int textObj;
	std::string text;
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
	void renderText();
	TLineFragment getNextLineFragment(const TLineFragment & currentLineFragment);
	void appendHotText(std::string newText, int idNo);
	void OnMouseMove(const int mouseX, const int mouseY, int key);
	void highlight(int textObj);
	void OnLMouseDown(const  int mouseX, const  int mouseY, int key);
	void onMouseOff(const  int mouseX, const  int mouseY, int key);

	void unhighlight(int textObj);

	int overrun;

	std::vector<TRichTextRec> textObjs; ///<The complete text of this control.
	int currentTextObj;
	int firstVisibleObject; ///<Index of the first object we need to draw.
	int firstVisibleText; ///<Position in the top visible object of the first text to display.

	std::vector<THotTextFragment> hotTextFrags; ///<Currently visible hot text fragments
	int selectedHotObj; ///<Currently selected hot text oject, if any.
};