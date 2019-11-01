#pragma once

#include <random>

#include "GUIlabel2.h"
#include "lineBuffer.h"
#include "..\3DEngine\src\utils\log.h"

#include "text.h"

enum TMouseWheelMode {scroll,hotText};



struct TCharacterPos {
	int textObj;
	int pos;
	bool operator==(const TCharacterPos& rhs) { return textObj == rhs.textObj && pos == rhs.pos; }
};

enum TTextDelivery { noDelivery, byClause, byCharacter };

enum TTempText {tempNone,tempOn,tempOff };
const unsigned int richSuspended = 2;
const unsigned int richMarked = 4;
const unsigned int richTemp = 8;
const unsigned int richFadeIn = 16;
const unsigned int richGap = 32;
const unsigned int richBookmark = 64;

struct TRichTextRec : textRec {
	TRichTextRec();
	unsigned int hotId;
	int gap;
	int lineRef;
	unsigned int flags;
	float period;

};


enum TOverrunMode {overScrollMode, overResizeMode};
enum TResizeMode {resizeByRatioMode, resizeByWidthMode, resizeNone };

;

/** Records the id and other details of a fragment used for special
	effects, such as hot text. */
struct TFXfragment {
	int fragId;
	std::string text;
	int textObj;
};

/** A more versatile multiline text display class. */
class CGUIrichText : public CGUIlabel2 {
public:
	CGUIrichText(int x, int y, int w, int h);
	void applyStyleSheet();
	void DrawSelf();
	void setFont(CFont* newFont);
	CFont* getFont();
	void setTextColour(float r, float g, float b, float a);
	void setAppendStyleBold(bool isOn);
	void setAppendStyleHot(bool isOn, bool unsuspended, unsigned int hotId);
	void setTextColour(UIcolour colour);
	void setTextColour(glm::vec4& colour);
	void setTextStyle(TtextStyle& style);
	bool setTextStyle(std::string styleName);
	void refreshCurrentTextStyles();
	void setTextTheme(const std::string& themeName);
	void setDefaultTextStyle(std::string styleName);
	void setText(std::string newText);
	void appendText(std::string newText);
	void addText(std::string newText);
	bool scrollDown2(int dist);
	void createPage();
	void updateFragmentPositions();
	void renderLineBuffer();
	void compileFragmentsToEnd(TLineFragment lineFragment);
	int processNextFragment(TLineFragment& fragment);
	TLine compileSingleLine(TLineFragment lineFragment);
	void checkLineOverrun(int yStart);
	TLineFragment getNextLineFragment(const TLineFragment & currentLineFragment);
	void OnMouseMove(const int mouseX, const int mouseY, int key);
	void msgHotTextChange(glm::i32vec2& adjustedMousePos);
	void msgHighlight();
	void OnLMouseDown(const  int mouseX, const  int mouseY, int key);
	void OnLMouseUp(const int mouseX, const int mouseY, int key);

	void onRMouseUp(const int mouseX, const int mouseY);

	bool onMouseOff(const  int mouseX, const  int mouseY, int key);


	TCharacterPos& getPreviousLine(TCharacterPos& startText);
	bool scrollUp3(int dist);
	TCharacterPos getPrevNewline(int textObj, int pos);
	void update(float dT);

	bool MouseWheelMsg(const int mouseX, const int mouseY, int wheelDelta, int key);
	bool attemptHotTextScroll(int direction);
	void smoothScroll2(int pixels);
	void updatePage();
	void removeScrolledOffText();
	void setMouseWheelMode(TMouseWheelMode mode);
	void updateHotTextSelection();
	void hotTextScroll(int direction);
	void selectTopHotText();
	std::vector<unsigned int> purgeHotText();
	std::vector<unsigned int> purgeHotText(unsigned int id);
	std::vector<unsigned int> getHotTextIds();
	void clear();
	void clearSelection();
	void appendMarkedUpText(std::string text);

	void updateAppearance();
	void resizeToFit();
	void resize(int w, int h);
	void resizeByRatio();
	void resizeByWidth();

	//bool isActiveHotText(int hotId);

	void setResizeMode(TResizeMode mode);

	bool setTempText(bool onOff);
	void setMarkedText(bool onOff);
	void setFadeText(bool onOff);
	int getStyleChangeTextObjAt(int objNo);
	void removeTempText();
	void insertBookmark();
	void suspend(bool isOn);

	void onDrag(const  int mouseX, const  int mouseY);
	//void onDrop(const  int mouseX, const  int mouseY);
	bool collapseTempText();
	bool solidifyTempText();
	void unhotDuplicates();
	void removeMarked();
	void insertGapObj(int beforeObj, int afterObj);
	void animateHotText(float dT);
	void animateFadeText(float dT);
	void animateLineFadeIn(float dT);
	void collapseGap(float dT);

	void deliverByCharacter(float dT);

	bool isOverrun();

	void requestLineFadeIn(bool onOff);

	bool clearToBookMark();

	bool isDisplayFinished();

	void setBusy(bool onOff) {
		busy = onOff;
	}
	bool isBusy() {
		return busy;
	}

	~CGUIrichText();

	int lineOverrun;
	int underrun;
	int maxHeight;
	int longestLine;
	int shortestSpaceBreak; ///<Only spacebreak after this many pixels.

	std::vector<TRichTextRec> textObjs; ///<The complete text of this control.
	int currentTextObj;
	int firstVisibleObject; ///<Index of the first object we need to draw.
	int firstVisibleText; ///<Position in the top visible object of the first text to display.

	int selectedHotObj; ///<Currently selected hot text oject, if any.

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

	std::map<std::string, TtextStyle>* styles; ///<List of available styles
	TtextStyle hotTextStyle;
	TtextStyle selectedHotTextStyle;

	//TO DO: try to scrap
	int textureHeight; ///<Height of the texture being drawn to.
	int textureWidth; ///<Guess.

	bool noScrollMode; ///<If true, text above the top line is thrown away.

	int insetX; ///<Little hack to enable left indent if > 0.

	CLog* transcriptLog; ///<If exists, send prerendered text here. 

	bool suspended; ///<If true, suspend activity such as highlighting.
	
		std::mt19937 randEngine; ///<Random number engine.
	static std::uniform_real_distribution<> randomPeriod;
	

	TTextDelivery deliveryMode;
	std::string deliveryBuffer;
	float charInterval;
	float charDelay;

	CLineBuffer lineBuffer; ///<Holds the current line fragments, organised into lines.

	std::vector<TFXfragment> hotFrags; ///<Indexes of fragments that are hot text.
	std::vector<TFXfragment> fadeFrags2; ///<Currently visible fade-in text fragments
	
	int gapObj; ///<Current gap obj if any.


	bool enableLineFadeIn; ///<False = line fade-in not allowed at all.
	bool lineFadeInOn; ///<Each line will be faded in instead of drawn instantly.
	float lineFadeSpeed; ///<Around 260. Smaller is slower

private:
	bool busy; ///<Indicates text should not be appended. True when engaged in smoothly collapsing text etc
	std::string currentTheme; ///<Name of the stylesheet theme to ask for styles.
};