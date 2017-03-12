#pragma once

#include "2DVector.h"



/** Defines a 2D grid, to be used as visual aid. */
class CGrid {
public:
	CGrid() : size(32), origin(0,0), xOffset(0), yOffset(0), axesOn(true),
		enabled(true) {
		xColour[0] = 1; xColour[1] = 0; xColour[2] = 0; xColour[3] = 1;
		yColour[0] = 0; yColour[1] = 1; yColour[2] = 0; yColour[3] = 1;
		colour[0] = 0.5; colour[1] = 0.5; colour[2] = 0.5; colour[3] = 1;
		scale.set(1,1);
		};
	void SetPos(float x,float y);
	void snapToGrid(float& x, float& y);
	void setSize(int newSize);

	int size ; ///<Size of the grid's divisions, in pixels.
	C2DVector origin; ///<Origin of the grid, relative to the view axis.
	float xOffset; ///<Offset from the view origin, expressed as a proportion of grid size.
	float yOffset; ///<Offset from the view origin, expressed as a proportion of grid size.

	bool axesOn; ///<True if we're drawing the axes.
	float xColour[4]; ///<Colour of x axis.
	float yColour[4]; ///<colour of y axis.
	float colour[4]; ///<Colour of grid lines.

	bool enabled; ///<True if this grid is to be drawn.

	C2DVector scale; ///<Scaling factor to apply for snapToGrid, if any.
};