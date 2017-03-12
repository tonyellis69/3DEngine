#if !defined(TYPES_H)
#define TYPES_H

//Shared types.

struct rgba {
	float r;
	float g;
	float b;
	float a;
};

class TMap {
public:
	TMap() {u = v = 0; s = t =1;} ;
	float u, v; ///<Texture coordinates, bottom left corner.
	float s,t; ///<Texture coordinates, top right corner.
};

/** The basic data structure passed to the renderer for drawing tiles, sprites etc. It says 'this textured quad 
	has this width and height, and uses this mapping. */
//TO DO: since mapping is dependent on the texture, it should include a texture identifier too.
//But if this is for *storing* rect data, it doesn't need to include the position at which it is drawn.
//That will tend to be calculated at runtime.
class TRect  {
public:
	TRect() { originX = originY = 0;} ;
	TMap Map;
	float width; ///<width of textured rectangle.
	float height; ///<height of textured rectangle.
	float originX,originY; ///<Offset from origin when drawn. Default = 0;
};

/** Simple class for  2D coordinate. */
class CPoint {
public:
	CPoint(float ix, float iy) { x = ix; y = iy;};
	float x;
	float y;
};

/** Simple class for 2D rectangle. */
//TO DO: do we want this to be float? */
class CRect {
public:
	float x,y;
	float width,height;
};

struct TMatrix {
	float M[16];
};


#endif