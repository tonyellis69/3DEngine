#include "2DVector.h"


C2DVector::C2DVector(void)
{
}

C2DVector::~C2DVector(void)
{
}

///	Create a vector with this initial values. */
C2DVector::C2DVector (float ix, float iy ) { 
	x = ix; 
	y = iy;
}
	
void C2DVector::set(float ix, float iy) {
	x = ix; 
	y = iy;
}



void C2DVector::SetAngle(float Angle) {
	Angle = (float)(Angle * DEG2RAD);
	x = sin(Angle);
	y = cos(Angle);
}

void C2DVector::AddVector(C2DVector* AddVec) {
	x += AddVec->x;
	y += AddVec->y;

}

void C2DVector::MultScalar(float Scalar) {
	x *= Scalar;
	y *=Scalar;
}


float C2DVector::Length() const {
	return (float) sqrt (x*x + y*y ); 
}

/** Rotate vector by angle degrees. */
void C2DVector::Rotate(float Angle) {
	Angle = (float)( Angle * DEG2RAD);
	float c( cos( Angle ) );
	float s( sin( Angle ) );
	float newx = x*c + y*s;
	float newy = x*-s + y*c;

	x = newx; y = newy;
}
