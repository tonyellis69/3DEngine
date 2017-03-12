#include "GUIiconButton.h"

CGUIsysIconButton::CGUIsysIconButton(int x, int y){
	SetPos(x,y,defaultIconButtonSize,defaultIconButtonSize);
	iconset = defaultIconset;
	icon = 0;
	type = iconButton;
}

void CGUIsysIconButton::DrawSelf( ) {
	pDrawFuncs->setClip(Clipbox);
	//Set the font
	//pDrawFuncs->setFont(TextFont);
	//Set the colour
	if (MouseDown == this)
		pDrawFuncs->setDrawColours(backColour2, backColour1);
	else 
		pDrawFuncs->setDrawColours(backColour1, backColour2);
	//draw button 
	pDrawFuncs->drawRect(screenPos,width,height);

	if (MouseOver == this)
		pDrawFuncs->setDrawColours(UIdarkGrey, UIdarkGrey);
	else
		pDrawFuncs->setDrawColours(UIlightGrey, UIlightGrey);
	pDrawFuncs->drawBorder(screenPos.x,screenPos.y,width,height);

	//draw icon
	pDrawFuncs->setIconset(iconset);
	pDrawFuncs->setDrawColoursConditional(UIwhite,UIwhite,enabled);
	pDrawFuncs->drawIcon(icon, screenPos.x + (width >> 1),screenPos.y + (height >> 1));
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