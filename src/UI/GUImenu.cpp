#include "GUImenu.h"

using namespace glm;

CGUImenu::CGUImenu(int x, int y, int w, int h) {
	xPos = x; yPos = y; width = w; height = h;
	drawBox.pos = i32vec2(x, y); drawBox.size = i32vec2(w, h);
	itemFont = defaultFont;
	textColour = UIblack;
	selectedColour = UIblue;
	type = uiMenu;
	vPad =  4;
	itemWidth = width;
	itemHeight = itemFont->lineHeight + vPad;
	clear();
	
	drawBorder = true;
	pDrawFuncs->registerControl(*this);
}

/** Set the font used by the menu items. */
void CGUImenu::setFont(CFont * newFont) {
	itemFont = newFont;
}

/** Set colour of menu items. */
void CGUImenu::setTextColour(UIcolour  colour) {
	textColour = colour;
}

void CGUImenu::setSelectedColour(UIcolour colour) {
	selectedColour = colour;
}


void CGUImenu::DrawSelf() {
	if (drawBorder) {
		pDrawFuncs->drawCtrlBorder(*this);
	}
}

void CGUImenu::addItem(std::string  itemText) {
	CGUIlabel2* item = new CGUIlabel2(0, nextItemPos, itemWidth, itemHeight);
	item->setMultiLine(false);
	item->setFont(itemFont);
	item->borderOn(false);
	if (selected == nItems)
		item->setTextColour(selectedColour);
	else
		item->setTextColour(textColour);
	item->setText(itemText);
	nextItemPos += itemHeight + vPad;
	item->id = nItems++;
	Add(item);
	items.push_back(item);
}

/** User rolling the mouse wheel, so scroll the selection. */
bool CGUImenu::MouseWheelMsg(const int mouseX, const int mouseY, int wheelDelta, int key) {
	items[selected]->setTextColour(textColour);
	if (wheelDelta > 0)
		selected--;
	if (wheelDelta < 0)
		selected++;
	if (selected >= nItems)
		selected = 0;
	if (selected < 0)
		selected = nItems - 1;
	items[selected]->setTextColour(selectedColour);
	return true;
}

/** User pressed left-mouse, affirming the currently selected item. */
void CGUImenu::OnLMouseDown(const  int mouseX, const  int mouseY, int key) {
	CMessage msg;
	msg.Msg = uiMsgLMdown;
	msg.value = selected;
	pDrawFuncs->handleUImsg(*this, msg);
}


void CGUImenu::clear() {
	nItems = 0;
	nextItemPos = 0;
	selected = 0;
	items.clear();
	for (size_t i = 0; i < Control.size(); i++)
		delete Control[i];
	Control.clear();
}

