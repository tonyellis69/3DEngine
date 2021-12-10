#pragma once

#include "GUIbase.h"

#include "lineBuffer.h"

#include "..\VMtest\src\Ivm.h"

#include "glm\glm.hpp"

#include "richText.h"


enum TResizeMode {resizeByRatioMode, resizeByWidthMode, resizeNone };


/** A more versatile multiline text display class. */
class CGUIrichText :  public CGUIbase
{
public:
	CGUIrichText(int x, int y, int w, int h);
	void appendMarkedUpText(const std::string& text);
	void Draw(float dT);
	void DrawSelf();
	bool isEmpty();
	void setDefaultFont(const std::string& fontName);
	void setColour(glm::vec4& colour);
	void setAutoscrollDown(bool onOff);

	void createPage();
	bool OnMouseMove(const int mouseX, const int mouseY, int key);
	void msgHotTextChange(glm::i32vec2& adjustedMousePos);

	bool OnLMouseDown(const  int mouseX, const  int mouseY, int key);
	bool OnLMouseUp(const int mouseX, const int mouseY, int key);

	bool onRMouseUp(const int mouseX, const int mouseY);

	bool onMouseOff(const  int mouseX, const  int mouseY, int key);

	bool MouseWheelMsg(const int mouseX, const int mouseY, int wheelDelta, int key);
	void smoothScroll(int pixels);

	void clear();
	void clearSelection();
	
	void updateAppearance();
	void resizeToFit();
	void resize(int w, int h);
	void resizeByRatio();
	void resizeByWidth();

	void setResizeMode(TResizeMode mode);

	void onDrag(const  int mouseX, const  int mouseY);

	void setHotTextMouseoverHandler(std::function<void(const std::string&)> handler) {
		hotTextMouseoverHandler = handler;
	}
	void setHotTextClickHandler(std::function<void(const std::string&)> handler) {
		hotTextClickHandler = handler;
	}

	std::function<void(const std::string&)> hotTextMouseoverHandler;
	std::function<void(const std::string&)> hotTextClickHandler;

	int maxResizeHeight;
	int longestLine;
	int shortestSpaceBreak; ///<Only spacebreak after this many pixels.

	
	float updateDt; ///<Elapsed seconds since last call to update().

	float correctOverrunDelay; ///<Delay before we next scroll
	bool overrunCorrectionOn; ///<If true, scroll down to correct overrun

	int smoothScrollStep;
	
	TResizeMode resizeMode;

	CLineBuffer lineBuffer2;
private:
	void prepForFirstText();
	void prepForScrolling();

	void initialisePage();
	void writePageToLineBuffer();
	bool writeLineToLineBuffer();
	void writePrevLineToLineBuffer();
	void writeNextLineToLineBuffer();
	int findLastBreakableChar();

	void checkHotTextContact(const  int mouseX, const  int mouseY);

	bool scrollDown3(int dist);
	bool scrollUp3(int dist);

	void update(float dT);


	
	bool autoscrollingDown; ///<If true, we scroll down if there's any text below.

	float dT;

	Ivm* pVM; ///<Interface to virtual machine for hot text calls.

	CRichText bodyText;

	std::string defaultFontName;

};