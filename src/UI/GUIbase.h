#pragma once

#include <vector>
#include "..\Delegate.h"
#include "..\renderer\texManager.h"
#include "font.h"
#include "..\renderer\buf.h"

//#include "GUIbetterBase.h"

#include <iostream> //for cerr

#include "glm\glm.hpp"


using namespace std;

class guiRect {
public:
	glm::i32vec2 pos;
	glm::i32vec2 size;
};

/** Simple message struct. */
class CMessage {
public:
	//char* Name;
	int Msg;
	int value;
	int value2;
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
const UIcolour UIfullblue = { 0,0,1,1 };
const UIcolour UIyellow = {1,1,0.4f,1};
const UIcolour UIred = {1,0,0,1};
const UIcolour UItransparent = {0,0,0,0};
const UIcolour UIbackgrey = {0.93f,0.93f,0.93f,1};
const UIcolour UIhiGrey = {0.80f,0.80f,0.80f,1};
const UIcolour UIalmostWhite = {0.964f,0.968f,0.972f,1};

const UIcolour UIlightGrey = {0.75f,0.75f,0.75f,1};// {0.86f,0.87f,0.87f,1};

const UIcolour oldbackColour1 = {0.99f,0.99f, 0.99f,1.0f};
const UIcolour oldbackColour2 = {0.75f, 0.75f, 0.75f,1.0f};

//NEW colours (abandon the above)
const glm::vec4 uiWhite = { 1,1,1,1 };
const glm::vec4 uiBlue = { 0,0,0.7f,1 };
const glm::vec4 uiBlack = { 0,0,0.0,1 };
const glm::vec4 uialmostBlack = { 0.1,0.1,0.1,1 };
const glm::vec4 uiDarkGrey = { 0.25,0.25,0.25,1 };

class CGUIbase;

//TO DO: remove test
extern DelegateP<void,int> setFont; 

/////////////////////class CGUIbetterBase;
/** Contains member function pointers to the various drawing function the GUI needs. It is the user's responsibility
	to create an instance of this, initialise it and pass it to GUIroot. */
class CDrawFuncs {
public:

	DelegatePP<void,const UIcolour&,const UIcolour&> setDrawColours;
	DelegatePPP<void,int,int,int> drawIcon;
	DelegatePP<void,CGUIbase&, CMessage&> handleUImsg;
	DelegatePPP<void,UIcoord&,int,int> drawRect;
	DelegatePPPP<void,int,int,int,int> drawBorder;
	DelegateP<void,UIrect&> setClip;
	DelegatePPPP<void,int,int,int,int> drawLine;
	DelegateP<void,int> setIconset;
	DelegatePP<void,int,UIrect&> drawTile;
	DelegatePP<void,int,UIrect&> drawImage;
	DelegateP<void,bool> mouseCaptured;
	DelegateP<void,int> setCursor;
	DelegatePPP<void, UIcolour, UIcolour, bool> setDrawColoursConditional;
	DelegatePPPP<void,int,int,int,int> drawDottedRect;
	DelegatePP<void,float,float> setScale;

	virtual void registerControl(CGUIbase& control) {};
	virtual void deregisterControl(CGUIbase & control) {};
	virtual void drawCtrlRect(CGUIbase& control) {};
	virtual void drawCtrlBorder(CGUIbase& control) {};
	virtual unsigned int getTextureHandle(const std::string & textureName) { return 0; };
	virtual void drawTexture(CGUIbase & control, CBaseTexture& texture) {};
	virtual void updateScreenDimensions(CGUIbase& control) {};
	virtual void drawCursor(CGUIbase& control, CBuf& cursorPos) {};
	virtual CFont* getFont(std::string name) { return NULL; };
	virtual void drawTextureGradient(CGUIbase & control, CBaseTexture& texture) {};
};


enum UItype {base,root,panel,label,button,radioButton,textbox,scrollbar,
		group,container,panelContainer,surface,imageGrid,iconButton,checkButton,
		dlgCtrl,
			uiImage,uiLabel,uiButton,uiTextbox, uiNumeric, uiMenu, uiPanel,
			uiRichTextPanel };



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
	virtual void MouseMsg(unsigned int Msg, int mouseX, int mouseY, int key);
	virtual void OnMouseMove(const  int mouseX, const  int mouseY, int key) {};
	virtual void OnLMouseDown(const  int mouseX, const  int mouseY, int key) {};
	virtual void OnRMouseDown(const  int mouseX, const  int mouseY) {};
	virtual void OnLMouseUp(const  int mouseX, const  int mouseY) ;
	virtual void onRMouseUp(const  int mouseX, const  int mouseY) ;
	virtual void OnClick(const  int mouseX, const  int mouseY) {};
	virtual void onKeyPress(unsigned int Key, long Mod) {};
	virtual void OnCharEntry(unsigned int Key, long Mod) {};
	virtual bool MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key);
	virtual void onMouseOff(const  int mouseX, const  int mouseY, int key) {};
	void SetPos(int x, int y, int w, int h);
	virtual void setPos(int x, int y);
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
	unsigned int getID();
	void makeModal(CGUIbase * control);

	void setBackColour1(const UIcolour & colour);
	void setBackColour1(const glm::vec4 & colour);
	void setBackColour2(const UIcolour & colour);
	void setBackColour2(const glm::vec4 & colour);

	void setBorderColour(const UIcolour & colour);

	void borderOn(bool onOff);

	void GenName(char* NameStr, int Count);
	void SetName(char* NameStr);

	void setDefaultFont(CFont* font);

	glm::i32vec2 getScreenCoords(int x, int y);
	int getWidth();
	int getHeight();
	virtual void resize(int w, int h);

	static	CMessage Message; ///<Any UI messages are returned here.

	
	glm::i32vec2 localPos; ///<Top left corner position relative to parent;
	
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
	//CGUIbetterBase* parent; ///<Points to the parent control of this control.

	std::string Name; ///<String identifying this control.

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

	unsigned int uniqueID; ///<Unique identifier for each control; TO DO: make private

	guiRect drawBox; ///<Defines the dimensions of the control for drawing (replaces screenPos).
	guiRect clipBox; ///<Defines the drawable area of this control for child controls.

	UIcolour borderColour; ///<Colour for the border of this control.
	bool drawBorder; ///<true if we draw this control's border.
protected:
	bool enabled; ///<False if this control has been deactivated.
	static CFont* defaultFont;
public:
	//static DelegateP<void,int> setFont;
	static std::vector<CGUIbase*> modalControls; ///<List of modal controls, if any.
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
enum Messagetypes {
	change, userDraw, uiMsgDrop, uiMsgLMdown, uiMsgRMdown, uiMsgMouseMove,
	uiMsgLMouseUp, uiMsgRMouseUp, uiMouseWheel, uiClick, uiDataEntered, uiSpin, uiLostKeyboard,
	uiMsgHotTextClick, uiMsgChildResize
};

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





//A few GLFW codes we use internally
#define GLFW_KEY_ESCAPE             256
#define GLFW_KEY_ENTER              257
#define GLFW_KEY_TAB                258
#define GLFW_KEY_BACKSPACE          259
#define GLFW_KEY_INSERT             260
#define GLFW_KEY_DELETE             261
#define GLFW_KEY_RIGHT              262
#define GLFW_KEY_LEFT               263
#define GLFW_KEY_DOWN               264
#define GLFW_KEY_UP                 265
