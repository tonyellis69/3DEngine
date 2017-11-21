#include "..\Types.h"
#include "..\vertBufs.h"

//const int textBufSize = 40;

/** The 2D renderer class. */
class C2dRenderer {
public:
	C2dRenderer();
	//void initText();
	void setPosition(float x, float y);
	void drawRect(TRect* rect,float x, float y);
	//void initTextBuffer();
	//void drawToTextBuffer(TRect* rect,float x, float y);
	//void drawTextBuffer();
	void scale2D(float x, float y);
	void setDrawScale(float x, float y);
	void drawBLRect(float x, float y, float width, float height);
	void drawTLoutlineRect(float x, float y, float width, float height);
	void setClip(int x, int y, int width, int height);
	void drawLine(float x, float y, float x2, float y2);
	void dottedLineMode(bool on);
	void drawColourBufTri(vBuf::T2DVertTri* vertBuf, vBuf::TColTri* colourBuf, int size);
	void setSecondColour(float r, float g, float b, float a);
	void setDrawColour(float r, float g, float b, float a);
	void setDrawColour( const rgba& Colour);
		
	float xScale,yScale; ///<2D scaling applied to all basic drawing functions. Normally 1.0.

//	int textBuffPos; ///<Index of the next free element in the text buffer.
	TMatrix Base2DView; ///<The default 2D translation, usually the identity matrix.
//	vBuf::T2DTexQuad textQuadBuf[textBufSize]; ///<Vertex buffer for quads for drawing text.
	//unsigned char textIndices[textBufSize*6]; ///<Holds the indices for drawing texture vertex arrays.

	rgba DrawColour; ///<Always mirrors the current harware drawing colour.
	rgba SecondColour; ///<The secondary drawing colour, used in gradients etc.
};
