#pragma once

#include "GUIbetterBase.h"
#include "textBuf.h"

/** Simple textbox control using shader and render-to-texture. */
class CGUItextbox2 : public CGUIbetterBase {
public:
	CGUItextbox2(int x, int y, int w, int h);

	void setFont(CFont* font);
	void setText( std::string newText);
	void setTextColour(float r, float g, float b, float a);
	void setTextColour(UIcolour colour);
	void DrawSelf();
	CBuf * getCursorPos();
	void calcCursorPosition();
	void calcLineOffset();
	void OnClick(const  int mouseX, const  int mouseY);
	void onKeyPress(unsigned int Key, long Mod);
	void moveCursor(int x, int y);
	void backSpace();
	void insert(std::string inText);
	void OnCharEntry(unsigned int Key, long Mod);
	void message(CGUIbase& sender, CMessage& msg);
	void renderText();

	CTextBuffer textBuf;
	glm::vec4 textColour;

	std::string text;

private:
	void dataEnteredAlert();

	CFont* font;
	
	glm::i32vec2 lineOffset;///<Due to centering, multiline, etc.

	int cursorTextPos; ///<String index of character cursor precedes.
	CBuf cursor; ///<Holds verts for the cursor position and size.

};