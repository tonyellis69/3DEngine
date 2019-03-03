#pragma once

#include "GUIlabel2.h"

enum TMouseWheelMode {scroll,hotText};

struct TtextTheme {
	std::vector<TtextStyle> styles;

};

struct TCharacterPos {
	int textObj;
	int pos;
};


struct TRichTextRec : textRec {
	TRichTextRec();
	std::vector<int> newLines;
	//int hotMsgId;
	//int hotObjId;
	unsigned int hotId;
};

enum TNewLine {no, wordwrap, newline};
enum TOverrunMode {overScrollMode, overResizeMode};
enum TResizeMode {resizeByRatioMode, resizeByWidthMode };

/** A partial or complete line of text, with its rendered (x) dimensions. */
struct TLineFragment {
	int textObj;
	int textPos;
	int textLength;
	int renderStartX;
	int renderEndX;
	TNewLine causesNewLine;
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
	bool overrun;
};


/** A more versatile multiline text display class. */
class CGUIrichText : public CGUIlabel2 {
public:
	CGUIrichText(int x, int y, int w, int h);
	void DrawSelf();
	void setFont(CFont* newFont);
	CFont* getFont();
	void setTextColour(float r, float g, float b, float a);
	void setAppendStyleBold(bool isOn);
	void setAppendStyleHot(bool isOn, int msgId, int objId, unsigned int hotId);
	void setTextColour(UIcolour colour);
	void setHotTextColour(const glm::vec4& colour);
	void setHotTextHighlightColour(const glm::vec4& colour);
	void setTextStyle(TtextStyle& style);
	void setTextStyle(std::string styleName);
	void setTextStyles(std::vector<TtextStyle>* styles);
	void setText(std::string newText);
	void appendText(std::string newText);
	bool scrollDown();
	void renderText();
	void makeHotFragment(TLineFragment & lineFragment, glm::i32vec2& offset, std::string& renderLine);
	void checkOverrun(int yStart, int currentObjNo);
	TLineFragment getNextLineFragment(const TLineFragment & currentLineFragment);
	void appendHotText(std::string newText, int msgId, int objId);
	void OnMouseMove(const int mouseX, const int mouseY, int key);
	void highlight(int textObj);
	void OnLMouseDown(const  int mouseX, const  int mouseY, int key);
	void onMouseOff(const  int mouseX, const  int mouseY, int key);
	void unhighlight(int textObj);
	//void removeHotText(int hotMsgId);
	bool scrollUp();
	int getPrevCharacter(int & textObj, int & pos);
	TCharacterPos getPrevNewline(int textObj, int pos);
	void update(float dT);

	bool MouseWheelMsg(const int mouseX, const int mouseY, int wheelDelta, int key);
	bool attemptHotTextScroll(int direction);
	void smoothScroll(int pixels);
	void updateText();
	void setMouseWheelMode(TMouseWheelMode mode);
	void updateHotTextSelection();
	void hotTextScroll(int direction);
	void selectTopHotText();
	std::vector<unsigned int> purgeHotText();
	std::vector<unsigned int> purgeHotText(unsigned int id);
	void clear();
	void clearSelection();
	void appendMarkedUpText(string text);

	void updateAppearance();
	void resizeToFit();
	void resize(int w, int h);
	void resizeByRatio();
	void resizeByWidth();

	//bool isActiveHotText(int hotId);

	void setResizeMode(TResizeMode mode);

	~CGUIrichText();

	int overrun;
	int underrun;
	int maxHeight;
	int longestLine;

	std::vector<TRichTextRec> textObjs; ///<The complete text of this control.
	int currentTextObj;
	int firstVisibleObject; ///<Index of the first object we need to draw.
	int firstVisibleText; ///<Position in the top visible object of the first text to display.

	std::vector<THotTextFragment> hotTextFrags; ///<Currently visible hot text fragments
	int selectedHotObj; ///<Currently selected hot text oject, if any.
	//std::vector<int> hotTextObjs; ///<Currently visible hot text object

	float updateDt; ///<Elapsed seconds since last call to update().

	float correctOverrunDelay; ///<Delay before we next scroll
	bool overrunCorrect; ///<If true, scroll down to correct overrun
	int overrunHotTextObj;
	int yPixelOffset; ///<Used for scrolling by pixel.
	int scrollHeight; ///<Pixels to smoothscroll before actually scrolling the text one line.
	int smoothScrollStep;
	TMouseWheelMode mouseWheelMode; ///<Either scrolling or hot text selecting.

	//glm::vec4 hotTextColour; ///<colour for unselected hot text.
	//glm::vec4 hotTextHighlightColour;  ///<colour for selected hot text.

	//CFont* currentSetFont;

	TtextStyle defaultTextStyle; //The one we start with and fall back to
	TtextStyle currentTextStyle;

	bool mouseMode; ///<True for selection hot text with mouse pointer.

	TResizeMode resizeMode;

	std::vector<TtextStyle>* styles; ///<List of available styles
	TtextStyle hotTextStyle;
	TtextStyle selectedHotTextStyle;

	//TO DO: try to scrap
	int textureHeight; ///<Height of the texture being drawn to.
	int textureWidth; ///<Guess.

};