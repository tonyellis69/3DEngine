#include "2d.h"
#include <windows.h> 
//#include <gl/gl.h>
#include <glew.h>

C2dRenderer::C2dRenderer() {
	xScale = yScale = 1;
//	initText();
	setDrawColour(1,1,1,1);
}

/** Initialise the text writing system. The index array is set up to read any vertex array
	as quads.*/
/*
void C2dRenderer::initText() {
	textBuffPos = 0;
	for (int i=0;i<textBufSize;i++) {
		int v = i * 4; int ix = i * 6;
		textIndices[ix] = v; textIndices[ix+1] = v+1;  textIndices[ix+2] = v+2;
		textIndices[ix+3] = v+2; textIndices[ix+4] = v+3;  textIndices[ix+5] = v;
	}
}
*/

/** Translate the origin to X,Y, so that all subsequent drawing operations are offset by this vector. */
void C2dRenderer::setPosition(float x, float y){
	glMatrixMode(GL_MODELVIEW);	
	glTranslatef(x,y,0);
}


/** Draw the given textured rectangle at the positionHint x,y, using the current texture/colour etc. */
void C2dRenderer::drawRect(TRect* rect,float x, float y) {
	glBegin(GL_QUADS);                                 
	glTexCoord2f(rect->Map.u,rect->Map.v); glVertex3f( x +  (-rect->originX * xScale),  y + (-rect->originY * yScale),0);
	glTexCoord2f(rect->Map.s,rect->Map.v); glVertex3f( x + ((-rect->originX + rect->width) * xScale), y + (-rect->originY * yScale) ,0);
	glTexCoord2f(rect->Map.s,rect->Map.t); glVertex3f( x + ((-rect->originX + rect->width) * xScale) ,y + ((-rect->originY + rect->height) * yScale),0);
	glTexCoord2f(rect->Map.u,rect->Map.t); glVertex3f( x + (-rect->originX * xScale) ,y + ((-rect->originY + rect->height) * yScale),0);
	glEnd();
}

/** Reset the text buffer. */
/*
void C2dRenderer::initTextBuffer() {
	textBuffPos = 0;
}
*/
/** Send text drawing data to a buffer for later drawing. If the buffer becomes full, draw the contents and reset it.*/
/*
void C2dRenderer::drawToTextBuffer(TRect* rect,float x, float y) {
	textQuadBuf[textBuffPos].x1 = x +  (-rect->originX * xScale); textQuadBuf[textBuffPos].y1 = y + (-rect->originY * yScale);
	textQuadBuf[textBuffPos].x2 = x + ((-rect->originX + rect->width) * xScale); textQuadBuf[textBuffPos].y2 = y + (-rect->originY * yScale);
	textQuadBuf[textBuffPos].x3 = x + ((-rect->originX + rect->width) * xScale); textQuadBuf[textBuffPos].y3 = y + ((-rect->originY + rect->height) * yScale);
	textQuadBuf[textBuffPos].x4 = x +  (-rect->originX * xScale); textQuadBuf[textBuffPos].y4 = y + ((-rect->originY + rect->height) * yScale);
	textQuadBuf[textBuffPos].u1 = rect->Map.u; textQuadBuf[textBuffPos].v1 = rect->Map.v;
	textQuadBuf[textBuffPos].u2 = rect->Map.s; textQuadBuf[textBuffPos].v2 = rect->Map.v;
	textQuadBuf[textBuffPos].u3 = rect->Map.s; textQuadBuf[textBuffPos].v3 = rect->Map.t;
	textQuadBuf[textBuffPos].u4 = rect->Map.u; textQuadBuf[textBuffPos].v4 = rect->Map.t;
	textBuffPos++;
	if (textBuffPos == textBufSize-1) { //buffer full, so draw contents.
		drawTextBuffer();
		textBuffPos = 0;
	} 		
}
*/
/*
void C2dRenderer::drawToTextBuffer(TRect* rect,float x, float y) {
	textVertBuf[textBuffPos].x1 = x +  (-rect->originX * xScale); textVertBuf[textBuffPos].y1 = y + (-rect->originY * yScale);
	textVertBuf[textBuffPos].x2 = x + ((-rect->originX + rect->width) * xScale); textVertBuf[textBuffPos].y2 = y + (-rect->originY * yScale);
	textVertBuf[textBuffPos].x3 = x + ((-rect->originX + rect->width) * xScale); textVertBuf[textBuffPos].y3 = y + ((-rect->originY + rect->height) * yScale);
	textVertBuf[textBuffPos].x4 = x +  (-rect->originX * xScale); textVertBuf[textBuffPos].y4 = y + ((-rect->originY + rect->height) * yScale);
	textUVBuf[textBuffPos].u1 = rect->Map.u; textUVBuf[textBuffPos].v1 = rect->Map.v;
	textUVBuf[textBuffPos].u2 = rect->Map.s; textUVBuf[textBuffPos].v2 = rect->Map.v;
	textUVBuf[textBuffPos].u3 = rect->Map.s; textUVBuf[textBuffPos].v3 = rect->Map.t;
	textUVBuf[textBuffPos].u4 = rect->Map.u; textUVBuf[textBuffPos].v4 = rect->Map.t;
	textBuffPos++;
	if (textBuffPos == textBufSize-1) { //buffer full, so draw contents.
		drawTextBuffer();
		textBuffPos = 0;
	} 		
} */

/** Draw the contents of the text buffer. */
/*
void C2dRenderer::drawTextBuffer() {
	glBindVertexArray(0);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//set up pointers
//	glVertexPointer(2,GL_FLOAT,0,textVertBuf);
//	glTexCoordPointer(2,GL_FLOAT,0,textUVBuf);
	glVertexPointer(2,GL_FLOAT,16,textQuadBuf);
	glTexCoordPointer(2,GL_FLOAT,16,(char*)textQuadBuf + 8);
	//draw data
	glDrawElements(GL_TRIANGLES,textBuffPos * 6,GL_UNSIGNED_BYTE,textIndices);
	//release pointers
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

}
*/
/** Scale the modelview matrix by the given amount. */
void C2dRenderer::scale2D(float x, float y) {
	glMatrixMode(GL_MODELVIEW);
	glScalef(x,y,0);
}

/** Set the renderer's internal scaling factor. */
void C2dRenderer::setDrawScale(float x, float y) {
	xScale = x; yScale = y;
}

/** Draw a coloured rectangle where x,y is its bottom-left corner. */
void C2dRenderer::drawBLRect(float x, float y, float width, float height) {
	glBegin(GL_QUADS);   
	glColor4fv((float*)&SecondColour ); glVertex3f( x, y,0);
	glColor4fv((float*)&SecondColour ); glVertex3f( x + width,y,0);
	glColor4fv((float*)&DrawColour ); glVertex3f( x + width, y + height,0);
	glColor4fv((float*)&DrawColour ); glVertex3f( x, y + height ,0);
	glEnd();
}


/** Set the secondary colour register, used for gradients, etc.*/
void C2dRenderer::setSecondColour(float r, float g, float b, float a) {
	SecondColour.r = r; SecondColour.g = g; SecondColour.b = b; SecondColour.a = a; 
}


/** Sets the current drawing colour to the given value. Because drawing with a single colour is the default,
	and the user must explicitly set up gradients if he wants them, it also sets SecondColour to this value.*/
void C2dRenderer::setDrawColour(float r, float g, float b, float a) {
	glColor4f(r,g,b,a);
	DrawColour.r = r; DrawColour.g = g; DrawColour.b = b; DrawColour.a = a; 
	SecondColour = DrawColour;
}

void C2dRenderer::setDrawColour( const rgba& Colour) {
	glColor4fv((const GLfloat*)&Colour);
	DrawColour = Colour;
	SecondColour = Colour;
}

/** Draw an outline rectangle, starting at the top left corner. */
void C2dRenderer::drawTLoutlineRect(float x, float y, float width, float height) {
	glLineWidth(1.0f);
	glBegin(GL_LINE_LOOP);
	glColor4fv((float*)&DrawColour ); glVertex3f( x, y,0);
	glColor4fv((float*)&SecondColour ); glVertex3f( x, y - height,0);
	glColor4fv((float*)&SecondColour ); glVertex3f( x + width, y - height,0);
	glColor4fv((float*)&DrawColour ); glVertex3f( x + width, y ,0);
	glEnd();
}

/** Create a clipping area, outside of which is drawn. */
void C2dRenderer::setClip(int x, int y, int width, int height) {
	glScissor(x,y,width,height);
}

/** Draw a single line. */
void C2dRenderer::drawLine(float x, float y, float x2, float y2) {
	glBegin(GL_LINES);
	glColor4fv((float*)&DrawColour ); glVertex3f( x, y,0);
	glColor4fv((float*)&SecondColour ); glVertex3f( x2, y2,0);
	glEnd();
}

void C2dRenderer::dottedLineMode(bool on) {
	glLineStipple(1, 0xAAAA); //default dotted line stipple.
	if (on) 
		glEnable(GL_LINE_STIPPLE);
	else 
		glDisable(GL_LINE_STIPPLE);
}

/** Draw coloured triangles from the data in this buffer. */
void C2dRenderer::drawColourBufTri(vBuf::T2DVertTri* vertBuf, vBuf::TColTri* colourBuf, int size) {
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	//set up pointers
	glVertexPointer(2,GL_FLOAT,0,vertBuf);
	glColorPointer(4,GL_FLOAT,0,colourBuf);
	//draw data
	glDrawArrays(GL_TRIANGLES,0,size);
	//release pointers
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}
