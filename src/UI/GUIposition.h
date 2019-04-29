#pragma once


enum TGUIhAlign {uiAlignHnone,uiAlignHcentred,uiAlignHleft,uiAlignHright};
enum TGUIvAlign { uiAlignVnone, uiAlignVcentred, uiAlignVtop, uiAlignVbottom };



class CGUIlayout {
public:
	CGUIlayout() : cols(1) {}
	int cols;

};


/** A class describing how a control is to be positioned on its
	parent control. */
class CGUIposition {
public:
	CGUIposition() : hAlignment(uiAlignHnone), vAlignment(uiAlignVnone) {}

	TGUIhAlign hAlignment; ///<Whether the control is centred, left-aligned, etc
	TGUIvAlign vAlignment; 
	CGUIlayout layoutstyle; ///<Two-column, three etc
	glm::i32vec2 rowCol;
};