#include "BBox.h"


void CBBox::transform(C2DVector& pos) {
	tBL = BL + pos;
	tTR = TR + pos;

}