#include "rowObject.h"
#include "GUIbase.h"

CRowObject::CRowObject() {
	isActive = true;
	rowHeight = 30;
}

void CRowObject::addControl(CGUIbase * control) {
	controls.push_back(control);
}

void CRowObject::setName(const std::string & name) {
	this->name = name;
}
