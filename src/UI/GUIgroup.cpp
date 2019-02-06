#include "GUIgroup.h"

const int defaultHeaderHeight = 30;
const int defaultHeaderIndent = 10;
//TO DO: these should be based on font size, not magic numbers. Sort when I make UI
//more flexible vis-a-vis text size.

int CGUIgroup::Count = 1;
CGUIgroup::CGUIgroup(int x, int y, int w, int h){
	GenName("Group",Count++);
	type = group;
	//localPos = glm::i32vec2(x, y); 
	setPos(x, y);
	//width = w; height = h;
	setWidth(w);
	setHeight(h);

	isDrawn = true;
	headerHeight = defaultHeaderHeight;
	headerIndent = defaultHeaderIndent + 5;
	header = new CGUIlabel2(headerIndent,0,w-headerIndent,headerHeight);
	Add(header);
	SetText(Name);
}

/** Set the text of the group's header label. */
void CGUIgroup::SetText(const std::string& TextStr) {
	header->setText(TextStr);
//	pDrawFuncs->setFont(header->TextFont);
	headerWidth = header->getTextWidth();
}

/** User may want to change header height from default if they change font. */
void CGUIgroup::setHeaderHeight(int h) {
	headerHeight = h;
	header->setLocalDimensions(headerIndent,0,getWidth()-headerIndent,h);
}

/** Toggles whether the control draws its header and frame. */
void CGUIgroup::setDrawn(bool drawn) {
	isDrawn = drawn;
	header->visible = drawn;
}



void CGUIgroup::DrawSelf( ) {
/*	if (!isDrawn)
		return;

	//header draws itself, so just draw border.
	int halfHeaderHeight = headerHeight >> 1;
	UIrect border; //to avoid clipping border through OpenGL's rounding error, shrink it by 1 pixel
	border.x = screenPos.x +1; border.y = screenPos.y + halfHeaderHeight;
	border.width = screenPos.x + width -1; border.height= screenPos.y + height - 1;

//	pDrawFuncs->setDrawColours(UIblack,UIblack);

	pDrawFuncs->drawLine(border.x + defaultHeaderIndent,border.y,border.x,border.y);

	pDrawFuncs->drawLine(border.x,border.y,border.x,border.height );
	pDrawFuncs->drawLine(border.x,border.height,border.width, border.height);
	pDrawFuncs->drawLine(border.width,  border.height,border.width,border.y);
	pDrawFuncs->drawLine(border.width,border.y,border.x +  headerWidth + headerIndent +2  ,border.y);
	*/
}



