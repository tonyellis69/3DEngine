#include "GUIiconButton.h"

using namespace glm;

CGUIsysIconButton::CGUIsysIconButton(int x, int y){
	setLocalDimensions(x,y,defaultIconButtonSize,defaultIconButtonSize);
	iconset = defaultIconset;
	icon = 0;
	type = iconButton;
	//deprecated!!!!
}

CGUIsysIconButton::CGUIsysIconButton(int x, int y, int w, int h) {
	setLocalPos(x, y);
	setWidth(w);
	setHeight(h);
//	pDrawFuncs->registerControl(*this);
	iconset = defaultIconset;
	icon = 0;
	type = iconButton;
	setBackColour1(oldbackColour1);
	setBackColour2(oldbackColour2);
}

void CGUIsysIconButton::DrawSelf( ) {
	//pDrawFuncs->setClip(Clipbox);

	if (MouseDown == this) {
		//pDrawFuncs->setDrawColours(backColour2, backColour1);
		//setBackColour1(oldbackColour2);
		//setBackColour2(oldbackColour1);
		pDrawFuncs->drawRect2(drawBox, (glm::vec4&)backColour2, (glm::vec4&)backColour1);
	}
	else {
		//pDrawFuncs->setDrawColours(backColour1, backColour2);
		//setBackColour1(oldbackColour1);
		//setBackColour2(oldbackColour2);
		pDrawFuncs->drawRect2(drawBox, (glm::vec4&)backColour1, (glm::vec4&)backColour2);

	}
	//draw button 
	//pDrawFuncs->drawRect(screenPos,width,height);
	//pDrawFuncs->drawCtrlRect(*this);

	if (MouseOver == this)
		//pDrawFuncs->setDrawColours(UIdarkGrey, UIdarkGrey);
		//setBorderColour(UIdarkGrey);
		pDrawFuncs->drawBorder2(drawBox, uiDarkGrey);
	else
		//pDrawFuncs->setDrawColours(UIlightGrey, UIlightGrey);
		//setBorderColour(UIlightGrey);
		pDrawFuncs->drawBorder2(drawBox, uiLightGrey);
	//pDrawFuncs->drawBorder(screenPos.x,screenPos.y,width,height);
	//pDrawFuncs->drawCtrlBorder(*this);

	//draw icon
//	pDrawFuncs->setIconset(iconset);
//	pDrawFuncs->setDrawColoursConditional(UIwhite,UIwhite,enabled);
//	pDrawFuncs->drawIcon(icon, screenPos.x + (width >> 1),screenPos.y + (height >> 1));
}

int CGUIiconButton::Count = 1;
CGUIiconButton::CGUIiconButton(int x, int y) : CGUIsysIconButton(x,y) {
	GenName("iconButton",Count++);
}

void CGUIiconButton::OnClick(const  int mouseX, const  int mouseY) {
	CMessage msg;
	msg.Msg = uiClick;
	pDrawFuncs->handleUImsg(*this,msg);
}

