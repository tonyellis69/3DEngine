#include "GUIdropdownMenu.h"
#include "GUIroot.h"

#include "UI/uiRender.h"

using namespace glm;

CGUIdropdownMenu::CGUIdropdownMenu(int x, int y, int w, int h) {
	setLocalPos(x, y);
	setWidth(w);
	setHeight(h);
	type = uiDropdownMenu;
	setBorderOn(true);
	setBackColour1(UIalmostWhite);
	setBackColour2(UIalmostWhite);

	int labelPad = 4;
	menuButton = new CGUIlabel2(labelPad, labelPad, w - labelPad*2, h- labelPad*2);
	menuButton->setBorderOn(true);
	//menuButton->mousePassthru = true;
	menuButton->setBorderOn(false);
	add(menuButton);

	//get here

	menu = new CGUImenu(0, 0 + h, w, 30);

	menu->setVisible(false);
	menu->setGUIcallback(this);
	menu->setBackColour1(backColour1);
	menu->setBackColour2(backColour2);
	
	//didn't get here
	menu->minItemsShown = 3;
	menu->resizeHorizontal = true;
	rootUI->add(menu);

}

void CGUIdropdownMenu::setText(const std::string & title) {
	menuButton->setText(title);
}




void CGUIdropdownMenu::DrawSelf() {
	uiDraw::drawRect(drawBox, (glm::vec4&)backColour1, (glm::vec4&) backColour2);

	if(MouseOver == this)
		uiDraw::drawBorder(drawBox, style::uiDarkGrey);
	else
		uiDraw::drawBorder(drawBox, style::uiLightGrey);

}

bool CGUIdropdownMenu::OnClick(const int mouseX, const int mouseY) {
	menu->setLocalPos(drawBox.pos.x, drawBox.pos.y + getHeight());
	menu->setVisible(true);
	menu->makeModal();
	return true;
}


void CGUIdropdownMenu::GUIcallback(CGUIbase* sender, CMessage& msg)  {
	if (sender == menu && msg.Msg == uiClickOutside) {
		menu->setVisible(false);
		menu->makeUnModal();
		return;
	}

	if (sender == menu && msg.Msg == uiClick && callbackObj) {
		callbackObj->GUIcallback(this, msg);
		menuButton->setText(menu->items[msg.value]->getText());
		menu->setVisible(false);
		menu->makeUnModal();
	}
}

void CGUIdropdownMenu::clear() {
	menu->clear();
}

std::string  CGUIdropdownMenu::getItemName(int itemNo) {
	return menu->getItemName(itemNo);
}

void CGUIdropdownMenu::addItem(std::initializer_list<std::string>  itemTexts) {
	menu->addItem(itemTexts);
}
