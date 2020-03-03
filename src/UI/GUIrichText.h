#pragma once

#include <random>

#include "font.h"

#include "GUIbase.h"

#include "lineBuffer.h"
#include "..\3DEngine\src\utils\log.h"

#include "text.h"


enum TStyleChange {
	styleNone, styleBold, styleHotOn, styleHotOff, styleSuspendedHotOn, styleSuspendedHotOff, styleStyle,
	styleBookmark
};

struct TStyleRec {
	TStyleChange styleChange;
	unsigned int hotId;
	std::string styleName;
};



enum TTextDelivery { noDelivery, byClause, byCharacter };
enum TTempText {tempNone,tempOn,tempOff };
enum TOverrunMode {overScrollMode, overResizeMode};
enum TResizeMode {resizeByRatioMode, resizeByWidthMode, resizeNone };


/** Records the id and other details of a fragment used for special
	effects, such as hot text. */
struct TFXfragment {
	int fragId;
	std::string text;
	int textObj;
};

/** A more versatile multiline text display class. */
class CGUIrichText : public ILineBuffer, public CGUIbase
{
public:
	CGUIrichText(int x, int y, int w, int h);
	void applyStyleSheet();
	void appendMarkedUpText(const std::string& text);
	void DrawSelf();
	TRichTextRec& getTexObjCallback(int objNo);
	void setFont(CFont* newFont);
	CFont* getFont();
	void setTextColour(float r, float g, float b, float a);
	void setAppendStyleHot(bool isOn, bool unsuspended, unsigned int hotId);
	void setTextColour(UIcolour colour);
	void setTextColour(glm::vec4& colour);
	void setTextStyle(TtextStyle& style);
	bool setTextStyle(std::string styleName);
	void refreshCurrentTextStyles();
	void setTextTheme(const std::string& themeName);
	void setDefaultTextStyle(std::string styleName);
	void setText(std::string newText);

	void createPage();
	void compileFragmentsToEnd(TPagePos fragmentStart);
	TLineFragment compileSingleLine(TLineFragment lineFragment);
	TLineFragment getNextLineFragment(const TLineFragment & currentLineFragment);
	bool OnMouseMove(const int mouseX, const int mouseY, int key);
	void msgHotTextChange(glm::i32vec2& adjustedMousePos);

	bool OnLMouseDown(const  int mouseX, const  int mouseY, int key);
	bool OnLMouseUp(const int mouseX, const int mouseY, int key);

	bool onRMouseUp(const int mouseX, const int mouseY);

	bool onMouseOff(const  int mouseX, const  int mouseY, int key);


	TCharacterPos getPreviousLine(TPagePos& startText);

	TCharacterPos getPrevNewline(int textObj, int pos);
	void update(float dT);

	bool MouseWheelMsg(const int mouseX, const int mouseY, int wheelDelta, int key);
	void smoothScroll(int pixels);
	void removeScrolledOffText();

	std::vector<unsigned int> purgeHotText();
	std::vector<unsigned int> purgeHotText(unsigned int id);
	std::vector<unsigned int> getHotTextIds();
	void clear();
	void clearSelection();
	

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
	
	void deliverByCharacter(float dT);

	bool clearToBookMark();

	bool isDisplayFinished();

	void setBusy(bool onOff) {
		busy = onOff;
	}
	bool isBusy() {
		return busy;
	}

	~CGUIrichText();


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

	int smoothScrollStep;
	

	TtextStyle currentTextStyle; ///<The one most recently set by the user, and to which we revert after any markup effects.

	
	TResizeMode resizeMode;

	std::map<std::string, TtextStyle>* styles; ///<List of available styles
	TtextStyle hotTextStyle;
	TtextStyle selectedHotTextStyle;
	
	CLog* transcriptLog; ///<If exists, send prerendered text here. 

	bool suspended; ///<If true, suspend activity such as highlighting.
	
	std::mt19937 randEngine; ///<Random number engine.
	static std::uniform_real_distribution<> randomPeriod;
	

	TTextDelivery deliveryMode;
	std::string deliveryBuffer;
	float charInterval;
	float charDelay;

	CLineBuffer lineBuffer2;
private:
	void prepForFirstText();
	void prepForScrolling();


	std::string findNextTag( std::string& remainingTxt, TStyleRec& styleRec);
	void setStyleChange(TStyleRec& styleRec);
	void appendText(const std::string& newText);

	TLineFragment findNextFragmentStart(const TLineFragment& fragment);
	int findNextUnbreakableChar(int textObj, CFont* font);
	int findNextFragmentXPos(const TLineFragment& fragment);
	TLineFragment findFragmentEnd( TLineFragment fragment);


	void initialisePage();
	void writePageToLineBuffer();

	void checkHotTextContact(const  int mouseX, const  int mouseY);

	bool scrollDown2(int dist);
	bool scrollUp2(int dist);

	glm::vec4& getHotTextColour();
	glm::vec4& getHotTextSelectedColour();

	void hotTextMouseOver(int hotId);


	bool busy; ///<Indicates text should not be appended. True when engaged in smoothly collapsing text etc
	std::string currentTheme; ///<Name of the stylesheet theme to ask for styles.

	
	bool autoscrollingDown; ///<If true, we scroll down if there's any text below.

	float dT;

	int oldHotText;
	int currentHotText;
};