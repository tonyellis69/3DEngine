#include "GUImenu.h"

using namespace glm;

CGUImenu::CGUImenu(int x, int y, int w, int h) {
	localPos = glm::i32vec2(x, y); width = w; height = h;
	drawBox.pos = i32vec2(x, y); drawBox.size = i32vec2(w, h);
	itemFont = defaultFont;
	textColour = UIblack;
	selectedColour = uiLightGrey;
	type = uiMenu;
	vItemPad =  4;
	hItemPad = 1;
	itemWidth = width;
	maxTextWidth = 0;// itemWidth;
	itemHeight = itemFont->lineHeight + vItemPad;
	clear();
	
	pDrawFuncs->registerControl(*this);
	itemTextAlignment = tleft;
	leftAlignIndent = 10;
	hItemInteriorPadding = 4;
	highlightStyle = menuHighlightBar;

	setBackColour1(uiWhite);
	setBackColour2(uiWhite);
	minItemsShown = 8;
	maxItemsShown = 1000;
	resizeHorizontal = false;
	drawBorder = false;
}

/** Set the font used by the menu items. */
void CGUImenu::setFont(CFont * newFont) {
	itemFont = newFont;
	itemHeight = itemFont->lineHeight + vItemPad;
}

/** Set colour of menu items. */
void CGUImenu::setTextColour(UIcolour  colour) {
	textColour = colour;
}

void CGUImenu::setSelectedColour(glm::vec4 colour) {
	selectedColour = colour;
}


void CGUImenu::DrawSelf() {
	//if (drawBorder) {
	//	pDrawFuncs->drawCtrlBorder(*this);
	//}
	CGUIpanel::DrawSelf();
	if (selected >= 0 && highlightStyle == menuHighlightBar) {
		pDrawFuncs->drawRect2(items[selected]->drawBox, selectedColour, selectedColour);

	}
}

void CGUImenu::addItem( std::initializer_list<std::string>  itemTexts) {
	for (auto itemText : itemTexts) {
		CGUIlabel2* item = new CGUIlabel2(hItemPad, nextItemPos, itemWidth, itemHeight);
		item->setMultiLine(false);
		item->setFont(itemFont);
		item->borderOn(false);
		item->textAlign = itemTextAlignment;
		item->setLeftAlignIndent(leftAlignIndent);

		item->setTextColour(textColour);
		item->setText(itemText);
		nextItemPos += itemHeight + vItemPad;
		item->id = nItems++;
		if (maxTextWidth < item->getTextWidth()) {
			maxTextWidth = item->getTextWidth();
			//if (itemTextAlignment == tleft)
			//	maxItemWidth += leftAlignIndent ;
		}
		if (resizeHorizontal) {
			itemWidth = maxTextWidth + 2 * hItemInteriorPadding;
			if (itemTextAlignment == tleft)
				itemWidth += leftAlignIndent;
		}

		item->mousePassthru = true;
		Add(item);
		items.push_back(item);
	}
	resizeToFit();
}

/** Resize to fit current items. */
void CGUImenu::resizeToFit() {
	width = itemWidth + 2 * hItemPad;
	unsigned int visibleItems = std::max(items.size(), minItemsShown);
	visibleItems = std::min(visibleItems, maxItemsShown);
	height = (itemHeight + vItemPad) * visibleItems + vItemPad;
	drawBox.size = i32vec2(width, height);
	for (auto item : items)
		item->resize(itemWidth, itemHeight);
	updateAppearance();
	CMessage msg;
	msg.Msg = uiMsgChildResize;
	parent->message(this, msg);
}



void CGUImenu::onMouseOff(const  int mouseX, const  int mouseY, int key) {
		selected = -1;
};


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
	if (highlightStyle == menuHighlightText)
		items[selected]->setTextColour(selectedColour);
	return true;
}

/** If the mouse moves over a menu item, select it. */
void CGUImenu::OnMouseMove(const  int mouseX, const  int mouseY, int key) {
	if (items.size() == 0)
		return;
	i32vec2 mouse = getLocalPos(mouseX, mouseY);
	if (selected >=0 && highlightStyle == menuHighlightText)
		items[selected]->setTextColour(textColour);
	selected = -1;
	if (mouse.x > 0 && mouse.x < drawBox.size.x && mouse.y > 0 && mouse.y < drawBox.size.y) {
		selected = mouse.y / (itemHeight + vItemPad);
		if (selected > nItems - 1)
			selected = nItems - 1;
		if (selected >= 0 && highlightStyle == menuHighlightText)
			items[selected]->setTextColour(selectedColour);
		if (callbackObj) {
			CMessage msg;
			msg.value = selected;
			msg.Msg = uiMsgMouseMove;
			callbackObj->GUIcallback(this, msg);
		}
	}
		
}

/** User pressed left-mouse, affirming the currently selected item. */
void CGUImenu::OnLMouseDown(const  int mouseX, const  int mouseY, int key) {
	CMessage msg;
	msg.Msg = uiMsgLMdown;
	msg.value = selected;
	pDrawFuncs->handleUImsg(*this, msg);
}

/** User pressed left-mouse, affirming the currently selected item. */
void CGUImenu::OnClick(const  int mouseX, const  int mouseY) {
	CMessage msg;
	if (IsOnControl(*this,mouseX,mouseY))
		msg.Msg = uiClick;
	else
		msg.Msg = uiClickOutside;
	msg.value = selected;
	//parent->message(this, msg);
	pDrawFuncs->handleUImsg(*this, msg);
	if (callbackObj) {
		callbackObj->GUIcallback(this, msg);
	}
}


void CGUImenu::clear() {
	nItems = 0;
	nextItemPos = vItemPad;
	selected = -1;
	items.clear();
	for (size_t i = 0; i < Control.size(); i++)
		delete Control[i];
	Control.clear();
}

std::string & CGUImenu::getItemName(int itemNo) {
	if (itemNo < items.size())
		return items[itemNo]->getText();
	return string("");
}


