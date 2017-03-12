#pragma once
#include "GUIbase.h"
#include "GUIpanel.h"
#include "GUIlabel.h"
#include "GUIbutton.h"


/** Base class for creating dialogue boxes. */

class CGUIdlg :  public CGUIpanel {
public:
//	CGUIdlg() {};
	CGUIdlg(int x, int y, int w, int h);
	void setHeader(const string& text);
	//void DrawSelf();

	CGUIlabel* header; ///<Dialogue heading.

};

class CGUIsysDlg : public CGUIdlg {
public:
	CGUIsysDlg();
	void setText(const string& text);

	CGUIlabel* body; ///<Dialogue body text.
};

class CGUIyesNoDlg : public CGUIsysDlg {
public:
	CGUIyesNoDlg();

	CGUIbutton* yes; //TO DO: do we even need these?
	CGUIbutton* no;
};

class CGUIdlgButton : public CGUIbutton {
public:
	CGUIdlgButton(int x, int y, int w, int h);
	void OnClick(const  int mouseX, const  int mouseY);

};


class CGUIokDlg : public CGUIsysDlg {
public:
	CGUIokDlg(int w, int h);

	void message(CGUIbase& sender, CMessage& msg);
	CGUIdlgButton* ok;
};

const int defaultDlgWidth = 320;
const int defaultDlgHeight = 240;

enum dlgYesNo {dlgYesID,dlgNoID};

const int dlgOKID = 1001;