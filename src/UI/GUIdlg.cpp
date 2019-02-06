#include "GUIdlg.h"
#include "GUIroot.h"

CGUIdlg::CGUIdlg(int x, int y, int w, int h){
	type = dlgCtrl;
	//localPos = glm::i32vec2(x, y);
	setPos(x, y);
	//width = w; height = h;
	setWidth(w);
	setHeight(h);
	hFormat = hCentre; vFormat = vCentre;
	header = new CGUIlabel2(0,10,w,30);
	header->setText("Header");
	header->hFormat = hSpan;
	header->setHorizontalAlignment(tcentred);
	Add(header);

	backColour1 = oldbackColour1;
	backColour2 = oldbackColour2;
	drawBorder = true;
}

void CGUIdlg::setHeader(const string& text) {
	header->setText(text);
}



CGUIsysDlg::CGUIsysDlg() : CGUIdlg(0,0,defaultDlgWidth,defaultDlgHeight)  {
	body = new CGUIlabel2(20,50,getWidth() -20,70);
	body->setText("Body text");
	body->setMultiLine(true);
	body->hFormat = hCentre;
	Add(body);	
}


void CGUIsysDlg::setText(const string& text) {
	body->setText(text);
}



CGUIyesNoDlg::CGUIyesNoDlg() {
	yes = new CGUIbutton2(20,defaultDlgHeight-40,100,30);
	yes->setText("Yes");
	yes->id = dlgYesID;
	Add(yes);

	no = new CGUIbutton2(defaultDlgWidth - 120,defaultDlgHeight-40,100,30);
	no->setText("No");
	no->id = dlgNoID;
	Add(no);
}

CGUIdlgButton::CGUIdlgButton(int x, int y, int w, int h) : CGUIbutton2(x,y,w,h) {

}

void CGUIdlgButton::OnClick(const  int mouseX, const  int mouseY) {
	CMessage msg;
	msg.Msg = dlgOKID;
	parent->message(this,msg);

}


CGUIokDlg::CGUIokDlg(int w, int h)  : CGUIsysDlg()  {
	setWidth(w);
	setHeight(h);
	ok = new CGUIdlgButton(10,20,100,30);
	ok->setText("OK");
	ok->hFormat = hCentre;
	ok->setPosY(parent->getHeight() - 50);
	ok->id = dlgOKID;
	Add(ok);


	body->setWidth( w - 40);
	body->setHeight( h-110);
	body->setPosY(60);
}


void CGUIokDlg::message(CGUIbase* sender, CMessage& msg) {
	//mark this modal control for deleting
	rootUI->removeModal();
	//rootUI->killModal = true;
	//updateAppearance();

}

