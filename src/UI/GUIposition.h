#pragma once


enum TGUIalign {uiAlignNone,uiAlignCentred,uiAlignLeft,uiAlignRight};



class CGUIlayout {
public:
	CGUIlayout() : cols(1) {}
	int cols;

};


/** A class describing how a control is to be positioned on its
	parent control. */
class CGUIposition {
public:
	CGUIposition() : alignment(uiAlignNone) {}

	TGUIalign alignment; ///<Whether the control is centred, left-aligned, etc
	CGUIlayout layoutstyle; ///<Two-column, three etc
	glm::i32vec2 rowCol;
};