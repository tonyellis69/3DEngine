#pragma once

#include <vector>
#include "math.h"
#include "2DVector.h"



/** Simple 2D matrix for 2D operations. */

//math stuff:
#ifndef M_PI
#define M_PI 3.1415926536
#endif
#define DEG2RAD (M_PI/180.0)
#define RADIANS (180.0/M_PI)

class C2DMatrix {
public:
	C2DMatrix();
	C2DMatrix (float m0, float m1, float m2, float m3 );
	virtual ~C2DMatrix();
	C2DVector C2DMatrix::operator* (C2DVector &V);
	void SetRotation (float Angle);


	float M[4];
};

/** Simple class for defining 2D polygons. */
class C2DPolygon
{
public:
	C2DPolygon(void);
	~C2DPolygon(void);

	void Transform(const C2DVector& Pos);
	void ProjectToAxis( C2DVector& axis, float& min, float& max);
	void Resize(int Size);
	bool Intersects( C2DPolygon& Poly2, C2DVector& response);

	int VertCount; ///<Number of vertexes that compose this polygon.
	std::vector<C2DVector> V;
	std::vector<C2DVector> Vt;

	C2DMatrix Matrix;
	C2DVector Pos; ///<Posistion.
	
};

float IntervalDistance(float minA, float maxA, float minB, float maxB);

