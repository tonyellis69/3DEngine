#include "GUIdlg.h"
#include "GUIroot.h"

CGUIdlg::CGUIdlg(int x, int y, int w, int h){
	type = dlgCtrl;
	xPos = 0; yPos = 0; width = w; height = h;
	hFormat = hCentre; vFormat = vCentre;
	header = new CGUIlabel(0,10,width,30);
	header->hFormat = hSpan;
	header->TextAlign = tcentred;
	Add(header);

	backColour1 = oldbackColour1;
	backColour2 = oldbackColour2;
	drawBorder = true;
}

void CGUIdlg::setHeader(const string& text) {
	header->SetText(text);
}



CGUIsysDlg::CGUIsysDlg() : CGUIdlg(0,0,defaultDlgWidth,defaultDlgHeight)  {
	body = new CGUIlabel(20,50,width -20,70);
	body->multiLine = true;
	body->hFormat = hCentre;
	Add(body);	
}


void CGUIsysDlg::setText(const string& text) {
	body->SetText(text);
}



CGUIyesNoDlg::CGUIyesNoDlg() {
	yes = new CGUIbutton(20,defaultDlgHeight-40,100,30);
	yes->SetText("Yes");
	yes->id = dlgYesID;
	Add(yes);

	no = new CGUIbutton(defaultDlgWidth - 120,defaultDlgHeight-40,100,30);
	no->SetText("No");
	no->id = dlgNoID;
	Add(no);
}

CGUIdlgButton::CGUIdlgButton(int x, int y, int w, int h) : CGUIbutton(x,y,w,h) {

}

void CGUIdlgButton::OnClick(const  int mouseX, const  int mouseY) {
	CMessage msg;
	msg.Msg = dlgOKID;
	parent->message(*this,msg);

}


CGUIokDlg::CGUIokDlg(int w, int h)  : CGUIsysDlg()  {
	width = w;
	height = h;
	ok = new CGUIdlgButton(10,20,100,30);
	ok->SetText("OK");
	ok->hFormat = hCentre;
	ok->yPos = parent->height - 50;
	ok->id = dlgOKID;
	Add(ok);


	body->width = w - 40;
	body->height = h-110;
	body->yPos = 60;
}


void CGUIokDlg::message(CGUIbase& sender, CMessage& msg) {
	//mark this modal control for deleting
	rootUI->removeModal();
	//rootUI->killModal = true;
	//updateAppearance();

}

