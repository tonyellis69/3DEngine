#pragma once

#include <random>

#include "GUIlabel2.h"
#include "lineBuffer.h"
#include "lineBuffer2.h"
#include "..\3DEngine\src\utils\log.h"

#include "text.h"

enum TMouseWheelMode {scroll,hotText};

enum TStyleChange {
	styleNone, styleBold, styleHotOn, styleHotOff, styleSuspendedHotOn, styleSuspendedHotOff, styleStyle,
	styleBookmark
};

struct TStyleRec {
	TStyleChange styleChange;
	unsigned int hotId;
	std::string styleName;
};

struct TCharacterPos {
	int textObj;
	int pos;
	bool operator==(const TCharacterPos& rhs) { return textObj == rhs.textObj && pos == rhs.pos; }
};

enum TTextDelivery { noDelivery, byClause, byCharacter };

enum TTempText {tempNone,tempOn,tempOff };



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
class CGUIrichText : public ILineBufferCallback, public CGUIlabel2 {
public:
	CGUIrichText(int x, int y, int w, int h);
	void applyStyleSheet();
	void DrawSelf();
	TRichTextRec& getTexObjCallback(int objNo);
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

	bool scrollDownSuccessful(int dist);
	void createPage();
	void updateFragmentPositions();
	void renderLineBuffer();
	void compileFragmentsToEnd(TLineFragment lineFragment);
	int processNextFragment(TLineFragment& fragment);
	TLine compileSingleLine(TLineFragment lineFragment);
	void checkLineOverrun(int yStart);
	TLineFragment getNextLineFragment(const TLineFragment & currentLineFragment);
	bool OnMouseMove(const int mouseX, const int mouseY, int key);
	void msgHotTextChange(glm::i32vec2& adjustedMousePos);
	void msgHighlight();
	bool OnLMouseDown(const  int mouseX, const  int mouseY, int key);
	bool OnLMouseUp(const int mouseX, const int mouseY, int key);

	bool onRMouseUp(const int mouseX, const int mouseY);

	bool onMouseOff(const  int mouseX, const  int mouseY, int key);


	TCharacterPos getPreviousLine(TCharacterPos& startText);
	bool scrollUpSuccessful(int dist);
	TCharacterPos getPrevNewline(int textObj, int pos);
	void update(float dT);

	bool MouseWheelMsg(const int mouseX, const int mouseY, int wheelDelta, int key);
	bool attemptHotTextScroll(int direction);
	void smoothScroll(int pixels);
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
	void appendMarkedUpText(const std::string& text);

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

	int overrunningPixels;
	int underrun;
	int maxResizeHeight;
	int longestLine;
	int shortestSpaceBreak; ///<Only spacebreak after this many pixels.

	std::vector<TRichTextRec> textObjs; ///<The complete text of this control.
	int currentTextObj;
	int firstVisibleObject; ///<Index of the first object we need to draw.
	int firstVisibleText; ///<Position in the top visible object of the first text to display.

	int selectedHotObj; ///<Currently selected hot text oject, if any.

	float updateDt; ///<Elapsed seconds since last call to update().

	float correctOverrunDelay; ///<Delay before we next scroll
	bool overrunCorrectionOn; ///<If true, scroll down to correct overrun
	int SCRAP_THISoverrunHotTextObj;

	int scrollHeight; ///<Pixels to smoothscroll before actually scrolling the text one line.
	int smoothScrollStep;
	TMouseWheelMode mouseWheelMode; ///<Either scrolling or hot text selecting.

	//TtextStyle defaultTextStyle; ///<The one we start with.
	TtextStyle currentTextStyle; ///<The one most recently set by the user, and to which we revert after any markup effects.

	
	TResizeMode resizeMode;

	std::map<std::string, TtextStyle>* styles; ///<List of available styles
	TtextStyle hotTextStyle;
	TtextStyle selectedHotTextStyle;

	//TO DO: try to scrap
	int textureHeight; ///<Height of the texture being drawn to.
	int textureWidth; ///<Guess.

	
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
	void prepForFirstText();
	void prepForScrolling();
	void initTextDelivery();

	std::string findNextTag( std::string& remainingTxt, TStyleRec& styleRec);
	void setStyleChange(TStyleRec& styleRec);
	void appendText(const std::string& newText);
	void addText(std::string newText);

	void initialisePage();
	void writePageToLineBuffer();

	void checkHotTextContact(const  int mouseX, const  int mouseY);

	bool busy; ///<Indicates text should not be appended. True when engaged in smoothly collapsing text etc
	std::string currentTheme; ///<Name of the stylesheet theme to ask for styles.

	CLineBuffer2 lineBuffer2;
};