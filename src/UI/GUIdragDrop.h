#pragma once

//#include "GUIbase.h" //for drawfuncs

/** A base class for encapsulating drag-drop behaviour. */
class CDrawFuncs;
class CGUIdragDrop {
public:
	virtual bool isDropType(CGUIbase* droppee) { return false; };
	virtual ~CGUIdragDrop() {};
	virtual void drawSelf(int mouseX, int mouseY) {};

	static CDrawFuncs* pDrawFuncs; ///<Pointer to bundle of external drawing functions.


};