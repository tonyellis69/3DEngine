#pragma once
#include "GUIbase.h"
#include "GUIcontainer.h"


/** A scrollable container with separate panels for controls. */
class CGUIstackSurface;
class CGUIpanelContainer :  public CGUIsysContainer {
public:
	//CGUIpanelContainer(void) {};
	CGUIpanelContainer(int x, int y, int w, int h);
	void createSurface();
	void addPanel(int size);
	void add(unsigned int panel, CGUIbase* child);
	void movePanel(unsigned int panel, unsigned int slot);
	void assignPanelPositions();
	void deletePanel(unsigned int panel);
	CGUIpanel* getPanel(unsigned int panelNo);
	static int Count;
};

class CGUIstackSurface : public CGUIbaseSurface {
public:
//	CGUIstackSurface();
	void addPanel(CGUIpanel* panel);
	void assignPanelPositions();
};


