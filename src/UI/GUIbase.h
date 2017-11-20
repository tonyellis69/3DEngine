#pragma once

#include <vector>
#include "..\Delegate.h"
#include "..\renderer\texManager.h"
#include "font.h"
#include "..\renderer\buf.h"

//#include "GUIbetterBase.h"

#include <iostream> //for cerr

using namespace std;

/** Simple message struct. */
class CMessage {
public:
	char* Name;
	int Msg;
	int value;
	float fValue;
	int x;
	int y;
};

struct UIcoord {
	int x,y;
};

struct UIshape {
	int width,height;
};

class UIrect {
public:
	int x,y;
	int width,height;
};



struct UIcolour {
	float r;
	float g;
	float b;
	float a;
};

enum hFormatType { hCentre, hRight, hLeft, hSpan, hNone };
enum vFormatType { vCentre, vTop, vBottom, vSpan, vNone };
enum anchorType { aTop, aBottom, aLeft, aRight, aNone };

const UIcolour UIwhite = {1,1,1,1};
const UIcolour UIblack = {0,0,0,1};
const UIcolour UIdarkGrey = {0.49f,0.49f,0.49f,1};
const UIcolour UIvLightGrey = {0.95f,0.95f,0.95f,1}; //too white
const UIcolour UIblue = {0,0,0.7f,1};
const UIcolour UIyellow = {1,1,0.4f,1};
const UIcolour UIred = {1,0,0,1};
const UIcolour UItransparent = {0,0,0,0};
const UIcolour UIbackgrey = {0.93f,0.93f,0.93f,1};
const UIcolour UIhiGrey = {0.80f,0.80f,0.80f,1};
const UIcolour UIalmostWhite = {0.964f,0.968f,0.972f,1};

const UIcolour UIlightGrey = {0.75f,0.75f,0.75f,1};// {0.86f,0.87f,0.87f,1};

const UIcolour oldbackColour1 = {0.99f,0.99f, 0.99f,1.0f};
const UIcolour oldbackColour2 = {0.75f, 0.75f, 0.75f,1.0f};


class CGUIbase;

//TO DO: remove test
extern DelegateP<void,int> setFont; 

class CGUIbetterBase;
/** Contains member function pointers to the various drawing function the GUI needs. It is the user's responsibility
	to create an instance of this, initialise it and pass it to GUIroot. */
class CDrawFuncs {
public:
	DelegateP<void,int> setFont;
	DelegatePP<void,const UIcolour&,const UIcolour&> setDrawColours;
	DelegateP<int,char*> getTextWidth;
	DelegatePPP<void,int,int,int> drawIcon;
	DelegatePP<void,CGUIbase&, CMessage&> handleUImsg;
	DelegatePPP<void,UIcoord&,int,int> drawRect;
	DelegatePPPP<void,int,int,int,int> drawBorder;
	DelegateP<void,UIrect&> setClip;
	DelegatePPPP<void,int,int,int,char*> drawText;
	DelegatePPPP<void,int,int,int,char*> drawTextMultiLine;
	DelegatePPPP<void,int,int,int,int> drawLine;
	DelegateP<void,int> setIconset;
	DelegatePP<void,int,UIrect&> drawTile;
	DelegatePP<void,int,UIrect&> drawImage;
	DelegateP<void,bool> mouseCaptured;
	DelegateP<void,int> setCursor;
	DelegatePPP<void, UIcolour, UIcolour, bool> setDrawColoursConditional;
	DelegatePPPP<void,int,int,int,int> drawDottedRect;
	DelegatePP<void,float,float> setScale;

	virtual void registerControl(CGUIbetterBase& control) {};
	virtual void drawCtrlRect(CGUIbetterBase& control) {};
	virtual void drawCtrlBorder(CGUIbetterBase& control) {};
	virtual unsigned int getTextureHandle(const std::string & textureName) { return 0; };
	virtual void drawTexture(CGUIbetterBase & control, CBaseTexture& texture) {};
	virtual void updateScreenDimensions(CGUIbetterBase& control) {};
	virtual void drawCursor(CGUIbetterBase& control, CBuf& cursorPos) {};
};


enum UItype {base,root,panel,label,button,radioButton,textbox,scrollbar,
		group,container,panelContainer,surface,imageGrid,iconButton,checkButton,
		dlgCtrl,
			uiImage,uiLabel,uiButton,uiTextbox, uiNumeric};



/** A class defining the basis of all GUI controls. */
class CGUIradio;
class CGUImouse;
class CGUIroot;
class CGUIbaseEngine;
class CGUIbase {
public:
	CGUIbase() ;
	//CGUIbase(int x, int y, int w, int h);
	virtual ~CGUIbase(void);
	bool IsOnControl(const CGUIbase& Control,const  int mouseX, const  int mouseY);
	void MouseMsg(unsigned int Msg, int mouseX, int mouseY, int key);
	virtual void OnMouseMove(const  int mouseX, const  int mouseY, int key) {};
	virtual void OnLMouseDown(const  int mouseX, const  int mouseY, int key) {};
	virtual void OnRMouseDown(const  int mouseX, const  int mouseY) {};
	virtual void OnLMouseUp(const  int mouseX, const  int mouseY)  ;
	virtual void onRMouseUp(const  int mouseX, const  int mouseY) ;
	virtual void OnClick(const  int mouseX, const  int mouseY) {};
	virtual void OnKeyPress(unsigned int Key, long Mod) {};
	virtual void OnCharEntry(unsigned int Key, long Mod) {};
	virtual bool MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key);
	void SetPos(int x, int y, int w, int h);
	virtual void Add(CGUIbase* child);
	virtual void updateAppearance();
	virtual void Draw();
	virtual void DrawSelf();
	virtual void CalculateClipbox();
	virtual void scrollMsg(int deviceID, int newValue) {};
	CGUIbase* getChild(int childID);
	CGUIbase* findControl(int childID);
	CGUIbase* findControl(CGUIbase* child);
	int getControlNo();
	virtual void message(CGUIbase& sender, CMessage& msg);
	virtual void setEnabled(bool onOff);
	virtual bool getEnabled();
	virtual void captureKeyboard();
	virtual void uncaptureKeyboard();
	virtual void setVisible(bool onOff);
	virtual bool getVisible();


	void GenName(char* NameStr, int Count);
	void SetName(char* NameStr);

	void setDefaultFont(CFont& font);

	static	CMessage Message; ///<Any UI messages are returned here.

	int xPos; ///<Top left corner x coordinate.
	int yPos; ///<Top left corner y coordinate.
	
	int width; ///<Width of control.
	int height; ///<Height of control.
	UIshape rect; ///<Width and height of control.

	UIcoord screenPos; ///<Top left in ui screen coordinates.

	UIcolour backColour1; ///<First background colour of this control.
	UIcolour backColour2; ///<Second background colour of this control.
	UIcolour foreColour1; ///<First foreground colour of this control;
	UIcolour foreColour2; ///<Second foreground colour of this control;


	std::vector<CGUIbase*> Control; ///<Child ui elements owned by this element, eg, the buttons on a panel.

	static CDrawFuncs* pDrawFuncs; ///<Pointer to bundle of external drawing functions.
	
	//TO DO:  we can scrap below class
	static CGUIbaseEngine* pGUIeng; ///<Pointer to user-supplied GUI engine.

	UIrect Clipbox; ///<A clipping rectangle defining the drawable area of this control.
	
	CGUIbase* parent; ///<Points to the parent control of this control.

	char* Name; ///<Points to the string identifying this control.

	static int Count; ///<Records how many of this control have been created.

	hFormatType hFormat; ///<States how this control should be horizontally justified, if at all.
	vFormatType vFormat; ///<States how this control should be vertically justified, if at all.
	static CGUIbase* MouseOver; ///<Points to whichever control the mouse is judged to be over.
	static CGUIbase* MouseDown; ///<Points to whichever control the mouse is clicked down on, if any.
	static CGUIbase* KeyCapture; ///<Points to whichever control has captured keyboard input.
	static CGUIbase* scrollbarHasMouse; ///<Points to whichever scrollbar has captured the mouse, if any.
	unsigned int id; ///<Holds a number by which this control can be identified.

	UItype type; ///<The type of this control.
	bool visible; ///<If false, this control isn't drawn or functional, neither are its children.

	int borderWidth; ///<Deducted from clipping and child positioning calculations to ensure border isn't overwritten.

	int anchorBottom; ///<Distance achored from bottom of parent control, if at all.
	int anchorRight; ///<Distance anchored from left of parent control, if any.
	
	bool needsUpdate; ///<If true, control will have updateAppearance run next opportunity.

	static CGUIroot* rootUI; ///<Points to root UI control.
	static CGUImouse* mouse; ///<Points to the current mouse object.

	bool mousePassthru;

protected:
	bool enabled; ///<False if this control has been deactivated.
	static CFont defaultFont;
public:
	//static DelegateP<void,int> setFont;
};






enum TTextAlign {tleft, tcentred, tright};


//default control ID numbers:
const int uiRootID = 1000;
const int uiMouseID = 1001;
const int uiGridMouseID = 1002;
const int uiScrollbarID = 1003;
const int uiContainerVbarID = 1004;
const int uiContainerHbarID = 1005;



//message types
enum Messagetypes {change,userDraw,uiMsgDrop,uiMsgLMdown,uiMsgRMdown,uiMsgMouseMove,
 uiMsgLMouseUp,uiMsgRMouseUp,uiMouseWheel,uiClick,uiDataEntered,uiSpin,uiLostKeyboard};

#define NONE -1

//my made-up windows msg for left-mouse-held-down
#define LM_HELD_DOWN					100

#define WM_MOUSEFIRST                   0x0200
#define WM_MOUSEMOVE                    0x0200
#define WM_LBUTTONDOWN                  0x0201
#define WM_LBUTTONUP                    0x0202
#define WM_LBUTTONDBLCLK                0x0203
#define WM_RBUTTONDOWN                  0x0204
#define WM_RBUTTONUP                    0x0205
#define WM_RBUTTONDBLCLK                0x0206
#define WM_MBUTTONDOWN                  0x0207
#define WM_MBUTTONUP                    0x0208
#define WM_MBUTTONDBLCLK                0x0209
#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
#define WM_MOUSEWHEEL                   0x020A
#endif
#if (_WIN32_WINNT >= 0x0500)
#define WM_XBUTTONDOWN                  0x020B
#define WM_XBUTTONUP                    0x020C
#define WM_XBUTTONDBLCLK                0x020D
#endif
#if (_WIN32_WINNT >= 0x0600)
#define WM_MOUSEHWHEEL                  0x020E
#endif

#define	VK_BACK			  0x08
#define VK_RETURN         0x0D
#define VK_LEFT           0x25
#define VK_UP             0x26
#define VK_RIGHT          0x27
#define VK_DOWN           0x28
#define VK_SELECT         0x29
#define VK_PRINT          0x2A
#define VK_EXECUTE        0x2B
#define VK_SNAPSHOT       0x2C
#define VK_INSERT         0x2D
#define VK_DELETE         0x2E

#ifndef NOKEYSTATES

/*
 * Key State Masks for Mouse Messages
 */
#define MK_LBUTTON          0x0001
#define MK_RBUTTON          0x0002
#define MK_SHIFT            0x0004
#define MK_CONTROL          0x0008
#define MK_MBUTTON          0x0010
#if(_WIN32_WINNT >= 0x0500)
#define MK_XBUTTON1         0x0020
#define MK_XBUTTON2         0x0040
#endif /* _WIN32_WINNT >= 0x0500 */

#endif /* !NOKEYSTATES */
