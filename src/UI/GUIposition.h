#pragma once


//positioning styles
const unsigned int uiHnone = 0;
const unsigned int uiHcentred = 1;
const unsigned int uiHleft = 2;
const unsigned int uiHright = 4;
const unsigned int uiVnone = 0;
const unsigned int uiVcentred = 8;
const unsigned int uiVtop = 16;
const unsigned int uiVbottom = 32;

//may not need these, as context can set them at creation
const unsigned int ui1col = 64;
const unsigned int ui2col = 128;
const unsigned int uiNewRow = 256;



class CGUIlayout {
public:
	CGUIlayout() : cols(1) {}
	int cols;

};


/** A class describing how a control is to be positioned on its
	parent control. */
class CGUIposition {
public:
	CGUIposition() : free(false),  hAlignment(uiHnone), vAlignment(uiVnone) {}

	bool free; ///<Control is positioned freely, make no attempt to position it.
	unsigned int hAlignment; ///<Whether the control is centred, left-aligned, etc
	unsigned int vAlignment; 
	CGUIlayout layoutstyle; ///<Two-column, three etc
	glm::i32vec2 rowCol;
};