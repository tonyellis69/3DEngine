#pragma once

/** Eventually, this should be the *only* gui header the 
	user needs to import. It will need to replace gui.h
	among many others, when it can be renamed.*
	For now it's mainly here to provide access to the 
	mark 2 gui root control. */

#include <unordered_map>
#include <string>

#include "guiBase2.h"

/** New GUI master-control, to eventually replace the old guiRoot. 
	Note that it is not a gui control itself. It manages gui controls,
	and provides useful functions related to them. It is not
	root and should not be called that. */

class CguiRoot2 { //: public CguiBase {
public:
	void init(CguiBase* root);
	void addToIndex(CguiBase* ctrl);
	void onMouseButton(int button, int action, int mod);
	void onMouseMove(double x, double y);
	void onWinEnter(int entered);
	//CguiBase* getControl(int id);
	CguiBase* getParent(int id);
	CguiBase* findControl(const std::string& name);
	bool mouseIn(const std::string& name);
	bool mouseNotIn(const std::string& name);
	glm::i32vec2 getMousePos();

	CguiBase* pRoot;
	CguiBase* hotControl;
	CguiBase* oldHotControl;

	std::vector<CguiBase*> ctrlIndex;

	//internal, for tracking mouseclicks
	bool lMouseDown = false;
	bool rMouseDown = false;
	bool mMouseDown = false;
};

extern CguiRoot2 gui;