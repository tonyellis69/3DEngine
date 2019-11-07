#pragma once
#include "GUIbase.h"
#include "GUIscrollbar.h"
#include "GUIpanel.h"

/** Basic scrollable container for other controls. It uses a viewBox and a 
	'surface' - see below. */
class CGUIbaseSurface;
class CGUIsysContainer :  public CGUIbase {
public:
	CGUIsysContainer(void) {};
	CGUIsysContainer(int x, int y, int w, int h);
	void createScrollbars();
	void createSurface();
	void Add(CGUIbase* child);

	void message(CGUIbase* sender, CMessage& msg);
	bool MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key);
	

	void DrawSelf( );
	void updateAppearance();
	//bool verticalBarIsNeeded();
	bool verticalBarCheck();
	bool horizontalBarCheck();
	CGUIbase* getChild(int childID);
	void setControlMargin(int newMargin);
	void vScroll(int scroll);

	int findControlPanel(CGUIbase* child);

	void fitViewBoxToContainer();
	void adaptToContents();

	int wheelMultiplier; ///<Number of pixels slider moves for each mousewheel roll.
	CGUIbaseSurface* surface; ///<The actual control on which the container's child controls are placed.
	int spaceAroundControls; ///< Space to allow beyond controls when calculating surface extent.

	CGUIsysScrollbar* verticalBar; ///<Built-in scrolling control.
	CGUIsysScrollbar* horizontalBar; ///<Built-in scrolling control.
	bool horizontalBarActive; ///<True if we want the horizontal scrollbar, which mostly we don't.
};

/** The surface is the container's child control upon which all the contained
	controls are placed. It grows to fit all the contained controls. Its contents
	are visible within the viewBox - which defines a clipped view of the surface.
	By adjusting the positionHint of the surface, different controls are brought
	into view. */
class CGUIbaseSurface :public CGUIbase {
public:
	CGUIbaseSurface();
	void recalculateClipbox();
	void encompassChildControls();
	virtual void addPanel(CGUIpanel* panel) {};
	virtual void assignPanelPositions() {};
	void setViewbox(int x, int y, int w, int h);
	void message(CGUIbase* sender, CMessage& msg);
	bool onDoubleClick(const int mouseX, const int mouseY, int key);

	UIrect viewBox; ///<Rectangle describing the window through which this control is viewed.
	int spaceAroundControls; ///<Space allowed around outlying controls when calculating dimensions of surface.
};


class CGUIsurface : public CGUIbaseSurface {
public:
	
};

class CGUIcontainer : public CGUIsysContainer {
public:
	CGUIcontainer(int x, int y, int w, int h);
	static int Count;
};
