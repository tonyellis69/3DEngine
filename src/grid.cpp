#include "grid.h"
#include <math.h>

int intRound( double r ) {
    return (int) ((r > 0.0) ? (r + 0.5) : (r - 0.5)); 
}

/** Position the origin of the grid at the given point. */
void CGrid::SetPos(float x,float y) {
	origin.set(x,y);
	xOffset = fmod(origin.x,(float)size);
	yOffset = fmod(origin.y,(float)size);
}

/** Snape the given coordinate to the nearest on-grid point. */
void CGrid::snapToGrid(float& x, float& y) {
	float sx = (float)size;// * scale.x;
	float sy = (float)size;// * scale.y;
	x = intRound(x / sx) * sx;
	y = intRound(y / sy) * sy;
}

void  CGrid::setSize(int newSize) {
	size = newSize;

	xOffset = fmod(origin.x,(float)size);
	yOffset = fmod(origin.y,(float)size);
}