#pragma once

#include "Types.h"
#include <glm/glm.hpp>

/** Code and datatypes for drawing with vertex buffers. */

namespace vBuf {




/** Buffer structure for textured quads. */
struct T2DTexQuad {
	float x1,y1; float u1,v1;
	float x2,y2; float u2,v2;
	float x3,y3; float u3,v3;
	float x4,y4; float u4,v4;

};



/** Quad vertex buffer using triangles.*/
struct T2DVertTri {
	float x1,y1,x2,y2,x3,y3;
	float x4,y4,x5,y5,x6,y6;
};

/** Quad vertex colour buffer structure using triangles.*/
struct TColTri {
	rgba a,b,c;
	rgba d,e,f;
};

struct T3Dvert {
	glm::vec3 v; ///<Actual vertex
	glm::vec4 colour;
	glm::vec3 normal;
};

struct T3DnormVert {
	glm::vec3 v; 
	glm::vec3 normal;
};

struct T3DcolTri {
public:
	void setTri(glm::vec3& aTri, glm::vec3& bTri, glm::vec3& cTri); 
	void setNormals(glm::vec3& norm);
	T3Dvert a,b,c;
};


void drawBLQuad( T2DVertTri* buf, float x, float y,int w,int h);
void colourBLQuad(TColTri* buf, rgba colour);

void calcNormals(T3DcolTri& vert);
void setColours(T3DcolTri& vert, glm::vec4 colour);



}
