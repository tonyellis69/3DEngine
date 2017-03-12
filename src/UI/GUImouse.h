#pragma once
#include "GUIbase.h"



/** Simple base mouse control for dragging operations. Can be subclassed
to display whatever is being dragged.*/

class CGUImouse :  public CGUIbase {
public:
	CGUImouse() : dragging(false) { id = uiMouseID; dragData = -1;};
	virtual void sendMessage() {};

	bool dragging; ///<True if the mouse is currently in a dragging operation.	
	int dragData; ///<A reference to whatever the mouse may be dragging.
};

