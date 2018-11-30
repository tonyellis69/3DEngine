#include "GUIdropdownMenu.h"
#include "GUIroot.h"

using namespace glm;

CGUIdropdownMenu::CGUIdropdownMenu(int x, int y, int w, int h) {
	localPos = glm::i32vec2(x, y);
	drawBox.size = i32vec2(w, h);
	type = uiDropdownMenu;
	borderOn(true);
	setBackColour1(UIalmostWhite);
	setBackColour2(UIalmostWhite);

	int labelPad = 4;
	menuButton = new CGUIlabel2(labelPad, labelPad, w - labelPad*2, h- labelPad*2);
	menuButton->borderOn(true);
	menuButton->mousePassthru = true;
	menuButton->borderOn(false);
	Add(menuButton);

	menu = new CGUImenu(0, 0 + h, w, 30);
	menu->setVisible(false);
	menu->setGUIcallback(this);
	menu->setBackColour1(backColour1);
	menu->setBackColour2(backColour2);
	menu->minItemsShown = 3;
	menu->resizeHorizontal = true;
	rootUI->Add(menu);
}

void CGUIdropdownMenu::setText(const std::string & title) {
	menuButton->setText(title);
}




void CGUIdropdownMenu::DrawSelf() {
	pDrawFuncs->drawRect2(drawBox, (glm::vec4&)backColour1, (glm::vec4&) backColour2);

	if(MouseOver == this)
		pDrawFuncs->drawBorder2(drawBox, uiDarkGrey);
	else
		pDrawFuncs->drawBorder2(drawBox, uiLightGrey);

}

void CGUIdropdownMenu::OnClick(const int mouseX, const int mouseY) {
	menu->setVisible(true);
	menu->makeModal();
}

/** Use this to ensure menu - which is attached to root, is in the right place.*/
void CGUIdropdownMenu::updateAppearance() {
	CGUIbase::updateAppearance();
	menu->setPos(drawBox.pos.x, drawBox.pos.y + drawBox.size.y);
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

std::string & CGUIdropdownMenu::getItemName(int itemNo) {
	return menu->getItemName(itemNo);
}

void CGUIdropdownMenu::addItem(std::initializer_list<std::string>  itemTexts) {
	menu->addItem(itemTexts);
}
