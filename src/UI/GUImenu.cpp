#include "GUImenu.h"

using namespace glm;

CGUImenu::CGUImenu(int x, int y, int w, int h) {
	localPos = glm::i32vec2(x, y); width = w; height = h;
	drawBox.pos = i32vec2(x, y); drawBox.size = i32vec2(w, h);
	itemFont = defaultFont;
	textColour = UIblack;
	focusColour = uiLightGrey;
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
	focusStyle = menuHighlightBar;

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

void CGUImenu::setFocusColour(glm::vec4 colour) {
	focusColour = colour;
}


void CGUImenu::DrawSelf() {
	//if (drawBorder) {
	//	pDrawFuncs->drawCtrlBorder(*this);
	//}
	CGUIpanel::DrawSelf();
	//if (selected >= 0 && highlightStyle == menuHighlightBar) {
	//pDrawFuncs->drawRect2(items[selected]->drawBox, selectedColour, selectedColour);

	//}
}

void CGUImenu::addItem( std::initializer_list<std::string>  itemTexts) {
	for (auto itemText : itemTexts) {
		CGUImenuItem* item = new CGUImenuItem(hItemPad, nextItemPos, itemWidth, itemHeight);
		item->setMultiLine(false);
		item->setFont(itemFont);
		item->borderOn(false);
		item->textAlign = itemTextAlignment;
		item->setLeftAlignIndent(leftAlignIndent);
		item->focusColour = focusColour;

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
	setFocusItem(-1);
};


/** User rolling the mouse wheel, so scroll the selection. */
bool CGUImenu::MouseWheelMsg(const int mouseX, const int mouseY, int wheelDelta, int key) {
	/*items[focusItem]->setTextColour(textColour);
	int slot = focusItem;
	if (wheelDelta > 0)
		slot--;
	if (wheelDelta < 0)
		slot++;
	if (slot >= nItems)
		slot = 0;
	if (slot < 0)
		slot = nItems - 1;
	setFocusItem(slot);
	if (focusStyle == menuHighlightText)
		items[focusItem]->setTextColour(focusColour);
	return true;*/
	OnMouseMove(mouseX, mouseY, key); //cheap way to update focus if we scroll
	return false;// true;
}

/** If the mouse moves over a menu item, select it. */
void CGUImenu::OnMouseMove(const  int mouseX, const  int mouseY, int key) {
	if (items.size() == 0)
		return;
	i32vec2 mouse = getLocalPos(mouseX, mouseY);
	if (focusItem >=0 && focusStyle == menuHighlightText)
		items[focusItem]->setTextColour(textColour);
	setFocusItem(-1);
	if (mouse.x > 0 && mouse.x < drawBox.size.x && mouse.y > 0 && mouse.y < drawBox.size.y) {
		int slot = mouse.y / (itemHeight + vItemPad);
		if (slot > nItems - 1)
			setFocusItem(nItems - 1);
		else
			setFocusItem(slot);
		if (focusItem >= 0 && focusStyle == menuHighlightText)
			items[focusItem]->setTextColour(focusColour);
		if (callbackObj) {
			CMessage msg;
			msg.value = focusItem;
			msg.Msg = uiMsgMouseMove;
			callbackObj->GUIcallback(this, msg);
		}
	}
		
}

/** User pressed left-mouse, affirming the currently selected item. */
void CGUImenu::OnLMouseDown(const  int mouseX, const  int mouseY, int key) {
	CMessage msg;
	msg.Msg = uiMsgLMdown;
	msg.value = focusItem;
	pDrawFuncs->handleUImsg(*this, msg);
}

void CGUImenu::OnRMouseDown(const int mouseX, const int mouseY, int key) {
	CMessage msg;
	msg.Msg = uiMsgRMdown;
	msg.value = focusItem;
	if (callbackObj) {
		callbackObj->GUIcallback(this, msg);
	}
}

/** User pressed left-mouse, affirming the currently selected item. */
void CGUImenu::OnClick(const  int mouseX, const  int mouseY) {
	CMessage msg;
	if (IsOnControl(*this,mouseX,mouseY))
		msg.Msg = uiClick;
	else
		msg.Msg = uiClickOutside;
	msg.value = focusItem;
	//parent->message(this, msg);
	pDrawFuncs->handleUImsg(*this, msg);
	if (callbackObj) {
		callbackObj->GUIcallback(this, msg);
	}
}


void CGUImenu::clear() {
	nItems = 0;
	nextItemPos = vItemPad;
	setFocusItem(-1);
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

void CGUImenu::setHighlight(unsigned int itemNo, bool onOff) {
	if (itemNo > (nItems - 1))
		return;
	items[itemNo]->hasHighlight = onOff;
}



void CGUImenu::setFocusItem(int itemNo) {
	if (focusItem > -1)
		items[focusItem]->hasFocus = false;
	focusItem = itemNo;
	if (focusItem > -1)
		items[focusItem]->hasFocus = true;
}





CGUImenuItem::CGUImenuItem(int x, int y, int w, int h) : CGUIlabel2(x,y,w,h) {
	hasFocus = false;
	hasHighlight = false;
	highlightStyle = menuHighlightBar;
	focusStyle = menuHighlightBar;
	highlightColour = uiVeryLightGrey;
}

void CGUImenuItem::DrawSelf() {
	if (hasHighlight  && highlightStyle == menuHighlightBar) {
		pDrawFuncs->drawRect2(drawBox, highlightColour, highlightColour);
	}

	if (hasFocus  && focusStyle == menuHighlightBar) {
		pDrawFuncs->drawRect2(drawBox, focusColour, focusColour);
	}
	CGUIlabel2::DrawSelf();
}
