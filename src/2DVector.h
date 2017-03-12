#pragma once
#include <math.h>

const float  PI = 3.14159265358979f;

//math stuff:
#ifndef M_PI
#define M_PI 3.1415926536
#endif
#define DEG2RAD (M_PI/180.0)
#define RADIANS (180.0/M_PI)

class C2DVector
{
public:
	C2DVector(void);
	~C2DVector(void);
	C2DVector (float ix, float iy );
	void set(float ix, float iy);
	void SetAngle(float Angle);
	void AddVector(C2DVector* AddVec);
	void MultScalar(float Scalar);
	//float Length();

	///Return length of vector
	float Length () const;
	inline static float Length (const C2DVector& v) { 
		return v.Length(); 
	}
	
	///Multiply a vector and a scalar.
	inline friend C2DVector operator* (const C2DVector& v, float f)
	{ return C2DVector(v.x*f, v.y*f); }

	/// Multiply a vector and a scalar.
	inline friend C2DVector operator* (float f, const C2DVector& v)
	{ return C2DVector(v.x*f, v.y*f); }

	///Multiply two vectors.
	inline friend C2DVector operator* (const C2DVector& v1, const C2DVector& v2) {
		return C2DVector(v1.x * v2.x, v1.y * v2.y); }


	///Divide vector by a scalar:
	inline friend C2DVector operator/ (const C2DVector& v, float f)
	{
		f = 1.0f/f; 
		return C2DVector(v.x*f, v.y*f); 
	}

	///Divide two vectors;
	inline friend C2DVector operator/ (const C2DVector& v1, const C2DVector& v2)
	{ return C2DVector(v1.x/v2.x, v1.y/v2.y); }

	/// Add two vectors.
	inline friend C2DVector operator+ (const C2DVector& v1, const C2DVector& v2)
	{ return C2DVector(v1.x+v2.x, v1.y+v2.y); }

	inline void  operator+= (const C2DVector& v)
	{  C2DVector(x += v.x, y += v.y); }


	// Subtract two vectors.
	inline friend C2DVector operator- (const C2DVector& v1, const C2DVector& v2)
	{ return C2DVector(v1.x-v2.x, v1.y-v2.y); }


	///Return vector normalised to length 1.0
	C2DVector Norm () const { 
		return (*this)/(this->Length()); 
	}

	///Return dot product
	 float  dot( const C2DVector& v) {
		return x * v.x + y * v.y;
	}

	inline C2DVector TurnLeft() {
		return C2DVector(y,-x);
	}

	inline C2DVector TurnRight() {
		return C2DVector(-y,x);
	}



	void Rotate(float Angle); 

	float x;
	float y;
};



