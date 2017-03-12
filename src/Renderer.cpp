// Renderer.cpp: implementation of the CRenderer class.
//
//////////////////////////////////////////////////////////////////////

//#include "Renderer.h"
#include <windows.h> 
#include <gl/gl.h>
#include "Macros.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRenderer::CRenderer() {
	//CurrentTexture = 0; //TO DO: set to a special (checkerboard) default texture
	setDrawColour(1,1,1,1);
	xScale = yScale = 1;
	textBuffPos = 0;
	for (int i=0;i<textBufSize;i++) {
		int v = i * 4; int ix = i * 6;
		textIndices[ix] = v; textIndices[ix+1] = v+1;  textIndices[ix+2] = v+2;
		textIndices[ix+3] = v+2; textIndices[ix+4] = v+3;  textIndices[ix+5] = v;
	}

}

/** Attach the given window to the renderer, so it can be drawn to. */
void CRenderer::attachWindow(void* hWnd){
	//Windows-OpenGL setup stuff:
	//Set the Windows device context pixel format for the given window
	int nPixelFormat;

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	//size of this structure
		1,								//version
		PFD_DRAW_TO_WINDOW |			//draw to window (not bitmap)
		PFD_SUPPORT_OPENGL |			//support OpenGL calls
		PFD_DOUBLEBUFFER,				//double-buffered mode
		PFD_TYPE_RGBA,					//RGBA colour mode
		32,								//32 bit colour
		0,0,0,0,0,						//not used
		0,0,0,0,0,0,0,					//not used
		32,								//size of depth buffer
		0,0,							//not used
		PFD_MAIN_PLANE,					//draw in main plane
		0,0,0,0							//not used
	};

	myhDC = ::GetDC((HWND)hWnd); //get the window's device context 

	//choose pixel format that best matches the one described:
	nPixelFormat = ChoosePixelFormat(myhDC, &pfd);
	SetPixelFormat(myhDC, nPixelFormat, &pfd);

	ASSERT_ALWAYS(nPixelFormat, "Can't Find A Suitable PixelFormat.");
	ASSERT_ALWAYS(SetPixelFormat(myhDC,nPixelFormat,&pfd),"Can't Set The PixelFormat.");
   
	 myhRC = wglCreateContext(myhDC); //create rendering context for OpenGL
	ASSERT_ALWAYS(myhRC,"Can't Create A GL Rendering Context.");
	ASSERT_ALWAYS(wglMakeCurrent(myhDC, myhRC),"Can't activate GL Rendering Context.");

	const unsigned char *version ;

	version = (const unsigned char *)glGetString(GL_VERSION);
	fprintf(stderr,"\nOpenGL version: %s.\n",version);

	if(	(NULL == strstr( (char const*)glGetString( GL_EXTENSIONS ),
				"GL_ARB_texture_non_power_of_two" ) )
			)
		{	/*	not there, flag the failure	*/
			fprintf(stderr,"No NPOT\n");

		} else {
			/*	it's there!	*/
			fprintf(stderr,"Has NPOT\n");
		}
}

/** Detach a connected window from our renderer. */
void CRenderer::detachWindow(){
	ASSERT_ALWAYS(wglMakeCurrent(myhDC, NULL),"Can't deactivate GL Rendering Context.");
	wglDeleteContext(myhRC);
}

/** Clear the frame using the current background colour. */
void CRenderer::clearFrame(){
	glClear(GL_COLOR_BUFFER_BIT );
}

/** Display the current frame. */
void CRenderer::ShowFrame() {
	SwapBuffers(myhDC);	
}

/** Set the background colour. */
void CRenderer::setBackColour(const rgba& colour){
	glClearColor(colour.r,colour.g,colour.b,colour.a); 
}


/** Establish an orthographical viewing area on our window, with the given dimensions.*/
void CRenderer::set2DView(int x, int y,int width,int height){
	glViewport(x,y,width,height);
	CRenderer::Width = width;
	CRenderer::Height = height;
	glMatrixMode(GL_PROJECTION); //Sets field of view...
	glLoadIdentity();
	glOrtho(0,width,0,height,-1000,1000); //...which we want to be orthographic

	glMatrixMode(GL_MODELVIEW);	//Because all following matrix modifications should be to the ModelView matrix.

	glShadeModel(GL_SMOOTH);
	glEnable(GL_SCISSOR_TEST);
}

/** Reset the modelview matrix to the identity matrix, ie, free of all transformations. */ 
void CRenderer::resetMatrix() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); 
}

/** Set up standard texture blending. */
void CRenderer::initBlending() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


/** Set the renderer to draw with textures rather than colours.*/
void CRenderer::setTextureMode(bool Enabled){
	if (Enabled)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);
}


/** Translate the origin to X,Y, so that all subsequent drawing operations are offset by this vector. */
void CRenderer::setPosition(float X, float Y){
	glMatrixMode(GL_MODELVIEW);	
	glTranslatef(X,Y,0);
}

/** Set the texture we're drawing with to the given texure. */
void CRenderer::setCurrentTexture(int textureHandle) {
	glBindTexture(GL_TEXTURE_2D,textureHandle);
}

/** Draw the given textured rectangle at the position x,y, using the current texture/colour etc. 
void CRenderer::drawRectOld(TRect* Rect,float x, float y) {
	glBegin(GL_QUADS);                                 
	glTexCoord2f(Rect->Map.u,Rect->Map.v); glVertex3f( x - (Rect->XOff  * xScale),  y - (Rect->YOff* yScale),0);
	glTexCoord2f(Rect->Map.s,Rect->Map.v); glVertex3f( x + (Rect->XOff * xScale), y - (Rect->YOff * yScale) ,0);
	glTexCoord2f(Rect->Map.s,Rect->Map.t); glVertex3f( x + (Rect->XOff * xScale) ,y + (Rect->YOff * yScale),0);
	glTexCoord2f(Rect->Map.u,Rect->Map.t); glVertex3f( x - ( Rect->XOff * xScale) ,y + (Rect->YOff * yScale),0);
	glEnd();
}
*/

/** Draw the given textured rectangle at the position x,y, using the current texture/colour etc. */
void CRenderer::drawRect(TRect* Rect,float x, float y) {
	glBegin(GL_QUADS);                                 
	glTexCoord2f(Rect->Map.u,Rect->Map.v); glVertex3f( x +  (-Rect->originX * xScale),  y + (-Rect->originY * yScale),0);
	glTexCoord2f(Rect->Map.s,Rect->Map.v); glVertex3f( x + ((-Rect->originX + Rect->width) * xScale), y + (-Rect->originY * yScale) ,0);
	glTexCoord2f(Rect->Map.s,Rect->Map.t); glVertex3f( x + ((-Rect->originX + Rect->width) * xScale) ,y + ((-Rect->originY + Rect->height) * yScale),0);
	glTexCoord2f(Rect->Map.u,Rect->Map.t); glVertex3f( x + (-Rect->originX * xScale) ,y + ((-Rect->originY + Rect->height) * yScale),0);
	glEnd();
}

/** Reset the text buffer. */
void CRenderer::initTextBuffer() {
	textBuffPos = 0;
}

/** Send text drawing data to a buffer for later drawing. If the buffer becomes full, draw the contents and reset it.*/
void CRenderer::drawToTextBuffer(TRect* Rect,float x, float y) {
	textVertBuf[textBuffPos].x1 = x +  (-Rect->originX * xScale); textVertBuf[textBuffPos].y1 = y + (-Rect->originY * yScale);
	textVertBuf[textBuffPos].x2 = x + ((-Rect->originX + Rect->width) * xScale); textVertBuf[textBuffPos].y2 = y + (-Rect->originY * yScale);
	textVertBuf[textBuffPos].x3 = x + ((-Rect->originX + Rect->width) * xScale); textVertBuf[textBuffPos].y3 = y + ((-Rect->originY + Rect->height) * yScale);
	textVertBuf[textBuffPos].x4 = x +  (-Rect->originX * xScale); textVertBuf[textBuffPos].y4 = y + ((-Rect->originY + Rect->height) * yScale);
	textUVBuf[textBuffPos].u1 = Rect->Map.u; textUVBuf[textBuffPos].v1 = Rect->Map.v;
	textUVBuf[textBuffPos].u2 = Rect->Map.s; textUVBuf[textBuffPos].v2 = Rect->Map.v;
	textUVBuf[textBuffPos].u3 = Rect->Map.s; textUVBuf[textBuffPos].v3 = Rect->Map.t;
	textUVBuf[textBuffPos].u4 = Rect->Map.u; textUVBuf[textBuffPos].v4 = Rect->Map.t;
	textBuffPos++;
	if (textBuffPos == textBufSize-1) { //buffer full, so draw contents.
		drawTextBuffer();
		textBuffPos = 0;
	} 		
}

/** Draw the contents of the text buffer. */
void CRenderer::drawTextBuffer() {
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//set up pointers
	glVertexPointer(2,GL_FLOAT,0,textVertBuf);
	glTexCoordPointer(2,GL_FLOAT,0,textUVBuf);
	//draw data
	glDrawElements(GL_TRIANGLES,textBuffPos * 6,GL_UNSIGNED_BYTE,textIndices);
	//release pointers
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}



/** Scale the modelview matrix by the given amount. */
void CRenderer::scale2D(float x, float y) {
	glMatrixMode(GL_MODELVIEW);
	glScalef(x,y,0);
}

/** Save the current state of the modelview matrix. */
void CRenderer::saveDisplayMatrix() {
	glGetFloatv(GL_MODELVIEW_MATRIX,DisplayMatrix.M);
}

/** Load the saved matrix, which will usually contain any transformations used by the engine to fit drawing onscreen. */
void CRenderer::useDisplayMatrix() {
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(DisplayMatrix.M);
}

/** Set the renderer's internal scaling factor. */
void CRenderer::setDrawScale(float x, float y) {
	xScale = x; yScale = y;
}


/** Draw a coloured rectangle where x,y is its bottom-left corner. */
void CRenderer::drawBLRect(int x, int y, int width, int height) {
	glBegin(GL_QUADS);   
	glTexCoord2f(0,1); glColor4fv((float*)&SecondColour ); glVertex3i( x, y,0);
	glTexCoord2f(0,0); glColor4fv((float*)&SecondColour ); glVertex3i( x + width,y,0);
	glTexCoord2f(1,0); glColor4fv((float*)&DrawColour ); glVertex3i( x + width, y + height,0);
	glTexCoord2f(1,1); glColor4fv((float*)&DrawColour ); glVertex3i( x, y + height ,0);
	glEnd();
}
//TO DO: get rid of tex coord calls!
/*
void CRenderer::tmpDrawBLRect(float x, float y, int width, int height) {
	glBegin(GL_QUADS);   
	glTexCoord2f(0,1); glColor4fv((float*)&SecondColour ); glVertex3f( x, y,0);
	glTexCoord2f(0,0); glColor4fv((float*)&SecondColour ); glVertex3f( x + width,y,0);
	glTexCoord2f(1,0); glColor4fv((float*)&DrawColour ); glVertex3f( x + width, y + height,0);
	glTexCoord2f(1,1); glColor4fv((float*)&DrawColour ); glVertex3f( x, y + height ,0);
	glEnd();
}
*/

/** Set the secondary colour register, used for gradients, etc.*/
void CRenderer::setSecondColour(float r, float g, float b, float a) {
	SecondColour.r = r; SecondColour.g = g; SecondColour.b = b; SecondColour.a = a; 
}


/** Sets the current drawing colour to the given value. Because drawing with a single colour is the default,
	and the user must explicitly set up gradients if he wants them, it also sets SecondColour to this value.*/
void CRenderer::setDrawColour(float r, float g, float b, float a) {
	glColor4f(r,g,b,a);
	DrawColour.r = r; DrawColour.g = g; DrawColour.b = b; DrawColour.a = a; 
	SecondColour.r = r; SecondColour.g = g; SecondColour.b = b; SecondColour.a = a; 
}

void CRenderer::setDrawColour( const rgba& Colour) {
	glColor4fv((const GLfloat*)&Colour);
	DrawColour = Colour;
	SecondColour = Colour;
}

/** Draw an outline rectangle, starting at the top left corner. */
void CRenderer::drawTLoutlineRect(float x, float y, float width, float height) {
	glLineWidth(1.0f);
	glBegin(GL_LINE_LOOP);
	glColor4fv((float*)&DrawColour ); glVertex3f( x, y,0);
	glColor4fv((float*)&SecondColour ); glVertex3f( x, y - height,0);
	glColor4fv((float*)&SecondColour ); glVertex3f( x + width, y - height,0);
	glColor4fv((float*)&DrawColour ); glVertex3f( x + width, y ,0);
	glEnd();
}

/** Create a clipping area, outside of which is drawn. */
void CRenderer::setClip(int x, int y, int width, int height) {
	glScissor(x,y,width,height);
}

/** Draw a single line. */
void CRenderer::drawLine(float x, float y, float x2, float y2) {
//	glTranslatef(0.5, 0.5, 0); //adustment for rounding error
	glBegin(GL_LINES);
	glColor4fv((float*)&DrawColour ); glVertex3f( x, y,0);
	glColor4fv((float*)&SecondColour ); glVertex3f( x2, y2,0);
	glEnd();
//	glTranslatef(-0.5, -0.5, 0);
}

/** Push a copy of the current matrix onto a stack for quick retrieval. */
void CRenderer::pushMatrix() {
	glPushMatrix();
}

/** Restore the matrix to the version saved to the top of the stack. */
void CRenderer::popMatrix() {
	glPopMatrix();
}

void CRenderer::dottedLineMode(bool on) {
	glLineStipple(1, 0xAAAA); //default dotted line stipple.
	if (on) 
		glEnable(GL_LINE_STIPPLE);
	else 
		glDisable(GL_LINE_STIPPLE);
}

/** Draw coloured triangles from the data in this buffer. */
void CRenderer::drawColourBufTri(vBuf::T2DVertTri* vertBuf, vBuf::TColTri* colourBuf, int size) {
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