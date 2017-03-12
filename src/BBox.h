#pragma once
#include "2DVector.h"

/** A 2D axis-aligned bounding box. */

class CBBox {
public:
	CBBox() {};
	void set(float blx, float bly, float trx, float itry) {
		BL.set(blx,bly); TR.set( trx, itry);
		maxX = fabs((fabs(blx) > fabs(trx)) ? blx : trx);
		maxY = fabs( (fabs(bly) > fabs(itry)) ? bly : itry);

	}

	void transform(C2DVector& pos);

	C2DVector BL; ///<Bottom left corner.
	C2DVector TR; ///<Top right corner.

	C2DVector tBL; ///<Bottom left corner, transformed.
	C2DVector tTR; ///<Top right corner, transformed.

	float maxX; ///<Furthest x;
	float maxY;	///<Furthest y;
};


