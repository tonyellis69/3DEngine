#include "2DPolygon.h"
#include <iostream> //for cerr
using namespace std;


/** Simple 2D matrix class. */
C2DMatrix::C2DMatrix() {
	M[0] = M[3] = 1;
	M[1] = M[2] = 0;
}

C2DMatrix::C2DMatrix(float m0, float m1, float m2, float m3 ) {
	M[0] = m0; M[1] = m1; M[2] = m2; M[3] = m3;	
}

C2DMatrix::~C2DMatrix() {}

/**	Multiply a vertex V by our matrix. */
C2DVector C2DMatrix::operator* (C2DVector &V){
	return C2DVector (M[0]*V.x + M[2]*V.y,
                    M[1]*V.x + M[3]*V.y );
}

/** Create setup this matrix to rotate 2D vectors. */
void C2DMatrix::SetRotation (float Angle) {
	Angle = Angle * (float)DEG2RAD;
	float c( cos( Angle ) );
	float s( sin( Angle ) );

	M[0] = c; M[1] = -s;
	M[2] = s; M[3] =  c;

}

/** Simple 2D polygon class. */
C2DPolygon::C2DPolygon(void){
}

C2DPolygon::~C2DPolygon(void){
}

/** Set the number of vertices in this polygon. */
void C2DPolygon::Resize(int Size) {
	V.resize(Size);
	Vt.resize(Size);
	VertCount = Size;
}

/** Transform the vertices of this polygon by its matrix - typically a rotation. */
void C2DPolygon::Transform(const C2DVector& Pos) {
	int Verts = V.size();
	for (int x=0;x<Verts;x++) {
		Vt[x] =  Matrix * V[x];
		Vt[x] = Vt[x] + Pos;
	}
}

/** Project this polygon's points onto the given axis using the dot product. 
	Return the min and max in the given paramaters. */
void C2DPolygon::ProjectToAxis( C2DVector& axis , float& min, float& max) {
	float DotProd = axis.dot(Vt[0]);
	min = DotProd; max = DotProd;
	for (int x=0;x<VertCount;x++) {
		DotProd = Vt[x].dot(axis);
		if (DotProd < min) {
			min = DotProd;
		} else {
			if (DotProd > max) {
				max = DotProd;
			}
		}
	}

}

/** Returns true if the edges of this polygon intersects with those of  polygon 2. If so,
	response contains the smallest vector necessary to separate this ploygon from poly 2. 
	Uses the Separating Axis Theorem. */
bool C2DPolygon::Intersects( C2DPolygon& Poly2, C2DVector& response) {
	C2DVector Normal, segment,diff;
	bool result = true;
	float smallestOverlap = -1000000.0f; //nice big negative number
	float gap;

	//For each segment of this poly
	C2DVector LastPoint = Vt.back();
	for (int x=0;x<VertCount;x++) {
		//find the normal
		segment = Vt[x] - LastPoint;
		Normal = segment.TurnLeft();
		Normal = Normal.Norm();
		if (C2DVector(Poly2.Pos - LastPoint).dot(Normal) >= 0) {
			//Use that as the axis, and project poly2 onto it
			float minA = 0; float minB = 0; float maxA = 0; float maxB = 0;
			ProjectToAxis(Normal,  minA,  maxA);
			Poly2.ProjectToAxis(Normal,  minB,  maxB);
			gap = IntervalDistance(minA, maxA, minB, maxB);
			if (gap > 0) 
				return false;
			if (gap > smallestOverlap ) {
				smallestOverlap = gap;
				response = Normal * (gap);
			}
		}
		LastPoint = Vt[x];
	}

	
	//For each segment of the second poly
	LastPoint = Poly2.Vt.back();
	for (int x=0;x<Poly2.VertCount;x++) {
		//find the normal
		segment = Poly2.Vt[x] - LastPoint;
		Normal = segment.TurnLeft();
		Normal = Normal.Norm();
		//only use segments that face poly 1
		if (C2DVector(Pos - LastPoint).dot(Normal) >= 0) {
			//Use the normal as the axis, and project both polys onto it
			float minA = 0; float minB = 0; float maxA = 0; float maxB = 0;
			Poly2.ProjectToAxis(Normal,  minA,  maxA);
			ProjectToAxis(Normal,  minB,  maxB);
			gap = IntervalDistance(minA, maxA, minB, maxB);
			if (gap > 0) 
				return false;	
			if (gap > smallestOverlap ) {
				smallestOverlap = gap;
				response = Normal * (-gap);
			}
		}
		LastPoint = Poly2.Vt[x];
	}
	return result;
}

/** Finds the gap, if any, beween two ranges of numbers. */
float IntervalDistance(float minA, float maxA, float minB, float maxB) {
    if (minA < minB) {
        return minB - maxA;
    } else {
        return minA - maxB;
	}
} 