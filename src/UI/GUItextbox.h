#pragma once
#include "GUIbase.h"
#include "GUIlabel.h"


/** Simple textbox control. */
class CGUIsysTextbox :  public CGUIbase, public CGUIbaseLbl {
public:
	CGUIsysTextbox(void);
	void DrawSelf( );
	void OnClick(const  int mouseX, const  int mouseY);
	void onKeyPress(unsigned int Key, long Mod);
	void OnCharEntry(unsigned int Key, long Mod);
	virtual void dataEnteredAlert();
	virtual void message(CGUIbase& sender, CMessage& msg);

	int CursorPos; ///<Position of the cursor in the text.
};

class CGUItextbox : public CGUIsysTextbox {
public:
	CGUItextbox() {};
	CGUItextbox(int x, int y, int w, int h);

	static int Count;
};

const int textIndent = 5;