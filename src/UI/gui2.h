#pragma once

/** Eventually, this should be the *only* gui header the 
	user needs to import. It will need to replace gui.h
	among many others, when it can be renamed.*
	For now it's mainly here to provide access to the 
	mark 2 gui root control. */

#include <unordered_map>
#include <string>

#include "guiBase2.h"

	/** New GUI master-control, to eventually replace the old guiRoot. */

class CguiRoot2 : public CguiBase {
public:
	void init(CguiBase* root);
	void addToIndex(CguiBase* ctrl);
	void onEvent(CEvent& e);
	CguiBase* getControl(int id);
	CguiBase* getParent(int id);
	CguiBase* findControl(const std::string& name);
	bool mouseIn(const std::string& name);
	bool mouseNotIn(const std::string& name);

	CguiBase* pRoot;
	CguiBase* hotControl;
	CguiBase* oldFocusControl;

	std::vector<CguiBase*> ctrlIndex;
};

extern CguiRoot2 gui;