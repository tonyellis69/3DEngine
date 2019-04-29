#pragma once

#include <vector>
#include <string>

/** A class for storing a pointers to one row of GUI controls. */
class CGUIbase;
class CRowObject {
public:
	CRowObject();
	void addControl(CGUIbase* control);
	void setName(const std::string& name);
	
	bool isActive;
	std::string name;
	std::vector<CGUIbase*> controls;
	int rowHeight;
};