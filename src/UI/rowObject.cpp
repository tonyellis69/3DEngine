#include "rowObject.h"
#include "GUIbase.h"

CRowObject::CRowObject() {
	isActive = true;
	rowHeight = 20;
}

void CRowObject::addControl(CGUIbase * control) {
	controls.push_back(control);
	//if (control->drawBox.size.y > rowHeight)
		rowHeight = control->drawBox.size.y + 10;
}

void CRowObject::setName(const std::string & name) {
	this->name = name;
}
