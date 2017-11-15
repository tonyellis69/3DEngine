#pragma once

#include "GUIbase.h"

#include "glm\glm.hpp"

class guiRect {
public:
	glm::i32vec2 pos;
	glm::i32vec2 size;
};


/** A class to improve and eventually replace GUIbase. */
class CGUIbetterBase : public CGUIbase{
public:
	CGUIbetterBase();
	void setBorderColour(const UIcolour& colour);
	void setBackColour1(const UIcolour& colour);
	void setBackColour2(const UIcolour& colour);
	void updateAppearance();

	void SetPos(int x, int y, int w, int h);
	void setPos(int x, int y);

	unsigned int uniqueID; ///<Unique identifier for each control;
	guiRect drawBox; ///<Defines the dimensions of the control for drawing (replaces screenPos).
	guiRect clipBox; ///<Defines the drawable area of this control for child controls.

	UIcolour borderColour; ///<Colour for the border of this control.



protected:
	bool drawBorder; ///<true if we draw this control's border.

	
	};