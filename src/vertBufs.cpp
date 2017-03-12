#include "vertBufs.h" 


namespace vBuf {



/** Write the given quad data to a buffer as tri-drawing instructions. */
void drawBLQuad( T2DVertTri* buf, float x, float y,int w,int h) {
	buf->x1 = x; buf->y1 = y;
	buf->x2 = x + w; buf->y2 = y;
	buf->x3 = x + w; buf->y3 = y+h;

	buf->x4 = x + w; buf->y4 = y + h;
	buf->x5 = x; buf->y5 = y + h;
	buf->x6 = x; buf->y6 = y;
}

void colourBLQuad(TColTri* buf, rgba colour) {
	buf->a = buf->b = buf->c = colour;
	buf->d = buf->e = buf->f = colour;
}

void calcNormals(T3DcolTri& vert){
	glm::vec3 const & a = glm::vec3(vert.a.v);
	glm::vec3 const & b = glm::vec3(vert.b.v);
	glm::vec3 const & c =  glm::vec3(vert.c.v);
	vert.a.normal = glm::vec3(glm::normalize(glm::cross(c - a, c - b)));

	vert.b.normal = vert.c.normal = vert.a.normal;
}

void setColours(T3DcolTri& vert, glm::vec4 colour) {
	vert.a.colour = colour;
	vert.b.colour = vert.c.colour = vert.a.colour;
}


void T3DcolTri::setTri(glm::vec3& aTri, glm::vec3& bTri, glm::vec3& cTri) {
	a.v = aTri; b.v = bTri; c.v = cTri;
	calcNormals(*this);
}


void T3DcolTri::setNormals(glm::vec3& norm) {
	a.normal = b.normal = c.normal = glm::vec3(norm);
}


}
