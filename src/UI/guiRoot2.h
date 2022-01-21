#pragma once

#include "guiBase2.h"

/** New GUI master-control, to eventually replace the old guiRoot. */

class CguiRoot2 : public CguiBase {
public:
	void init(CguiBase* root);
	void onEvent(CEvent& e);

	CguiBase* pRoot; 
	CguiBase* focusControl;
	CguiBase* oldFocusControl;
};