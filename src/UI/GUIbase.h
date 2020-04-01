#pragma once

#include <vector>
#include "Delegate.h"
#include "renderer\texManager.h"
#include "font.h"
#include "renderer\buf.h"
#include "rowObject.h"
#include "GUIdragDrop.h"
#include "GUIstyleSheet.h"
#include  "GUIposition.h"
#include <iostream> //for cerr

#include "glm\glm.hpp"

//#include <string_view>

#include "iGuiMsg.h"

class guiRect {
public:
	guiRect() {};
	guiRect(int x, int y, int w, int h) :pos(x, y), size(w, h) {};
	void setSize(int w, int h) {
		if (w < 8) w = 8;
		if (h < 8) h = 8;
		size = glm::i32vec2(w, h);
	}
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

//NEW colours in stylesheet (abandon the above)


class CGUIbase;

//TO DO: remove test
extern DelegateP<void,int> setFont; 

/////////////////////class CGUIbetterBase;
/** Contains member function pointers to the various drawing function the GUI needs. It is the user's responsibility
	to create an instance of this, initialise it and pass it to GUIroot. */
class CDrawFuncs {
public:

	//DelegatePP<void,const UIcolour&,const UIcolour&> setDrawColours;
	//DelegatePPP<void,int,int,int> drawIcon;
	DelegatePP<void,CGUIbase&, CMessage&> handleUImsg;
	//DelegatePPP<void,UIcoord&,int,int> drawRect;
	//DelegatePPPP<void,int,int,int,int> drawBorder;
	DelegateP<void,UIrect&> setClip;
//	DelegatePPPP<void,int,int,int,int> drawLine;
//	DelegateP<void,int> setIconset;
//	DelegatePP<void,int,UIrect&> drawTile;
//	DelegatePP<void,int,UIrect&> drawImage;
	DelegateP<void,bool> mouseCaptured;
//	DelegateP<void,int> setCursor;
//	DelegatePPP<void, UIcolour, UIcolour, bool> setDrawColoursConditional;
//	DelegatePPPP<void,int,int,int,int> drawDottedRect;
//	DelegatePP<void,float,float> setScale;

	//virtual void registerControl(CGUIbase& control) {};
	virtual void deregisterControl(CGUIbase & control) {};
	virtual void drawCtrlRect(CGUIbase& control) {};
	virtual void drawCtrlBorder(CGUIbase& control) {};
	virtual unsigned int getTextureHandle(const std::string & textureName) { return 0; };
	virtual void drawTexture(guiRect& drawBox, CBaseTexture& texture) {};
	virtual void updateScreenDimensions(CGUIbase& control) {};
	virtual void drawCursor(CGUIbase& control, CBuf& cursorPos) {};
	virtual CFont* getFont(std::string name) { return NULL; };
	virtual void drawTextureGradient(guiRect & drawBox, CBaseTexture& texture) {};
	virtual void drawRect2(guiRect& drawBox,const glm::vec4& colour1, const glm::vec4&  colour2) {};
	virtual void drawBorder2(guiRect& drawBox, const glm::vec4& colour) {};
	virtual void drawCursor2(guiRect & drawBox) {};

};


enum UItype {base,root,panel,label,button,radioButton,textbox,scrollbar,
		group,container,panelContainer,surface,imageGrid,iconButton,checkButton,
		dlgCtrl,
			uiImage,uiLabel,uiButton,uiTextbox, uiNumeric, uiMenu, uiPanel,
			uiRichTextPanel, uiPaletteBar, uiPaletteTab, uiDropdownMenu, uiSwatchGroup};


class Icallback {
public:
	virtual void GUIcallback(CGUIbase* sender, CMessage& msg) {};
};





/** A class defining the basis of all GUI controls. */
//class CGUIlabel;
class CGUIbutton;
class CGUIradio;
class CGUImouse;
class CGUIroot;
class CGUIbaseEngine;
class CRowObject;
class IPopupMenu;
class CGUIbase : public Icallback {
public:
	CGUIbase() ;
	CGUIbase(int x, int y, int w, int h);
	virtual ~CGUIbase(void);
	//CGUIbase* add(UItype ctrlType, std::string text);
	//virtual void setStyleSheet(CGUIstyleSheet* styleSheet);
	//virtual void applyStyleSheet() {};
	//void propagateStylesheet();
	void positionLogical(CGUIbase * control);
	glm::i32vec2 layoutControlsCoarse();
	void layoutFine();
	glm::i32vec4 calcCellSize(CGUIbase* cellControl);
	glm::i32vec4 calcCellSizeMin(CGUIbase* cellControl);
	void centreCtrlRow(std::vector<CGUIbase*>& rowCtrls);
	virtual void resizeToFit() {}
	virtual void position(CGUIbase* control);
	glm::i32vec2& getSize();
	void setLayout(CGUIlayout& layout);
	void setMessageObject(IGUImsg* msgObj);

	bool IsOnControl(const CGUIbase& Control,const  int mouseX, const  int mouseY);
	virtual bool MouseMsg(unsigned int Msg, int mouseX, int mouseY, int key);
	virtual bool OnMouseMove(const  int mouseX, const  int mouseY, int key) {return true; };
	virtual bool OnLMouseDown(const  int mouseX, const  int mouseY, int key) { return true; };
	virtual bool OnRMouseDown(const  int mouseX, const  int mouseY, int key) { return false; };
	virtual bool OnLMouseUp(const  int mouseX, const  int mouseY);
	virtual bool onRMouseUp(const  int mouseX, const  int mouseY) { return false; };
	virtual bool OnClick(const  int mouseX, const  int mouseY) { return false; };
	virtual bool onDoubleClick(const int mouseX, const int mouseY, int key) { return false; };
	virtual void onKeyPress(unsigned int Key, long Mod) {};
	virtual void OnCharEntry(unsigned int Key, long Mod) {};
	virtual bool MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key);
	virtual bool onMouseOff(const  int mouseX, const  int mouseY, int key) { return true; };
	virtual void onDrag(const  int mouseX, const  int mouseY) {};
	virtual void onDrop(const  int mouseX, const  int mouseY);
	void setLocalDimensions(int x, int y, int w, int h);
	virtual void setLocalPos(int x, int y);
	virtual void setPosX(int x);
	virtual void setPosY(int y);
	virtual void Add(CGUIbase* child);
	virtual void updateAppearance();
	void recalculateDiminsions();
	virtual void Draw();
	virtual void DrawSelf();
	virtual void recalculateClipbox();
	virtual void scrollMsg(int deviceID, int newValue) {};
	CGUIbase* getChild(int childID);
	CGUIbase* findControl(int childID);
	CGUIbase* findControl(CGUIbase* child);
	int getControlNo();
	virtual void message(CGUIbase* sender, CMessage& msg) {};
	virtual void setEnabled(bool onOff);
	virtual bool getEnabled();
	virtual void captureKeyboard();
	virtual void uncaptureKeyboard();
	virtual void setVisible(bool onOff);
	virtual bool getVisible();
	unsigned int getUniqueID();
	void makeModal();
	void makeUnModal();

	void setBackColour1(const UIcolour & colour);
	void setBackColour1(const glm::vec4 & colour);
	void setBackColour2(const UIcolour & colour);
	void setBackColour2(const glm::vec4 & colour);

	void setBorderColour(const UIcolour & colour);
	void setBorderColour(const glm::vec4& colour);
	void setBorderColourFocusColour(const glm::vec4& colour);

	void setBorderOn(bool onOff);

	void GenName(char* NameStr, int Count);
	void SetName(char* NameStr);

	void setDefaultFont(CFont* font);

	//CGUIstyleSheet* styleSheet; ///<The stylesheet used by this control.

	glm::i32vec2 localToScreenCoords(int x, int y);
	glm::i32vec2 screenToLocalCoords(int x, int y);
	glm::i32vec2& getLocalPos();
	int getWidth();
	int getHeight();
	virtual void setWidth(int w);
	virtual void setHeight(int h);
	const glm::i32vec2 getScreenPos();
	virtual void resize(int w, int h);
	void setGUIcallback(Icallback* callbackInstance);
	virtual void GUIcallback(CGUIbase* sender, CMessage& msg);
	
	void addToRow(const std::string& name, std::initializer_list<CGUIbase*> childControls);
	void autoArrangeRows(int offsetX, int offsetY);
	void hideAllRows();
	void activateRows(  std::initializer_list< std::string> rows);
	virtual void dropMethod() {};
	void deleteDragDropObj();
	void popupMenu(std::initializer_list<std::string> options, IPopupMenu* callback);
	void setHorizontalAlignment(unsigned int mode) {
		positionHint.hAlignment = mode;
	}
	void setVerticalAlignment(unsigned int mode) {
		positionHint.vAlignment = mode;
	}


	static	CMessage Message; ///<Any UI messages are returned here.

	
	glm::i32vec2 localPos; ///<Top left corner positionHint relative to parent;
	

	//UIcoord screenPos; ///<Top left in ui screen coordinates.

	UIcolour backColour1; ///<First background colour of this control.
	UIcolour backColour2; ///<Second background colour of this control.
	UIcolour foreColour1; ///<First foreground colour of this control;
	UIcolour foreColour2; ///<Second foreground colour of this control;

	glm::vec4 borderFocusColour;


	std::vector<CGUIbase*> controls; ///<Child ui elements owned by this element, eg, the buttons on a panel.

	static CDrawFuncs* pDrawFuncs; ///<Pointer to bundle of external drawing functions.
	
	//TO DO:  we can scrap below class
	static CGUIbaseEngine* pGUIeng; ///<Pointer to user-supplied GUI engine.

	UIrect Clipbox; ///<A clipping rectangle defining the drawable area of this control.
	
	CGUIbase* parent; ///<Points to the parent control of this control.
	
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
	bool anchorLeft; ///<If true, anchorRight resizes the control rather than move it.
	bool anchorTop; ///<If true, anchorBottom resizes the control rather than move it.


	bool needsUpdate; ///<If true, control will have updateAppearance run next opportunity.

	static CGUIroot* rootUI; ///<Points to root UI control.
	
	unsigned int uniqueID; ///<Unique identifier for each control; TO DO: make private

	guiRect drawBox; ///<Defines the dimensions of the control for drawing (replaces screenPos).
	guiRect clipBox; ///<Defines the drawable area of this control for child controls.

	glm::vec4 borderColour; ///<Colour for the border of this control.
	bool drawBorder; ///<true if we draw this control's border.
	Icallback * callbackObj;
	bool isModal;
protected:
	bool enabled; ///<False if this control has been deactivated.
	static CFont* defaultFont;
	IGUImsg* msgObj; ///<The object we send messages to.

	inline static glm::i32vec2 mousePos; ///<Current mouse screen position.


private:
	void defaultMouseMoveResponse(int mouseX, int mouseY, int key);

public:
	//static DelegateP<void,int> setFont;
	static std::vector<CGUIbase*> modalControls; ///<List of modal controls, if any.

	std::vector<CRowObject> rowObjects;

	static CGUIdragDrop* dragDropObj;

	glm::i32vec2 resizeMin; ///<Minimum size resizeToFit is allowed to go to.
	glm::i32vec2 resizeMax; ///<Maximum size resizeToFit is allowed to go to.
	CGUIposition positionHint; ///<How the control is to be positioned on its parent.
	CGUIlayout currentLayoutStyle; ///<Layout style to use for next child control added.

	typedef struct {
		void advance() {
			rowCol.x++;
			if (rowCol.x >= currentLayoutStyle.cols) {
				rowCol.x = 0;
				rowCol.y++;
				centreRowOn = false;
			}
		}
		void setCols(int c) {
			currentLayoutStyle.cols = c;
			if (rowCol.x > 0)
				rowCol = glm::i32vec2(0, rowCol.y + 1);
			centreRowOn = false;
		}
		void centreRow() {
			centreRowOn = true;
		}
		CGUIlayout currentLayoutStyle; ///<Layout style to use for next child control added.
		glm::i32vec2 rowCol;
		bool centreRowOn = false; 
	} TControlCursor;
	TControlCursor controlCursor; ///<Keeps track of where the next child control should go.

	glm::bvec2 resizesForChildren; ///<If true control shrinks/grows to fit children.

	/** Auto-position the given control. */
	CGUIbase* autoPosition(CGUIbase* ctrl ) {
		//set position arbitarily for now
		ctrl->setLocalPos(10, 10);
		positionLogical(ctrl);
		glm::i32vec2 maxSize = layoutControlsCoarse();

		//resize parent if requested
		if (resizesForChildren.x)
			setWidth(maxSize.x);
		if (resizesForChildren.y)
			setHeight(maxSize.y);

		layoutFine();

		return ctrl;
	}

	/** Create and add a child control of type T, using auto-positioning. */
	template <typename T, typename S>
	T* add(S const& s, int style) {
		if constexpr (std::is_convertible_v<S, std::string>) {
			T* ctrl = new T(s, style);
			Add(ctrl);
			autoPosition(ctrl);
			return ctrl;
		}

			T* ctrl = new T(s, style); //pretty damn sure this is just leftover code
			Add(ctrl);
			autoPosition(ctrl);
			return ctrl;

		return NULL;
	}

	/** Add a control supplying styling but no text.*/
	template <typename T>
	T* add(int style) {
		T* ctrl = new T(style);
		Add(ctrl);
		autoPosition(ctrl);
		return ctrl;

		return NULL;
	}

	virtual void setPositionStyles(unsigned int styleWord);


};



enum TTextAlign {tleft, tcentred, tright};


//default control ID numbers:
const int uiRootID = 1000;
const int uiMouseID = 1001;
const int uiGridMouseID = 1002;
const int uiScrollbarID = 1003;
const int uiContainerVbarID = 1004;
const int uiContainerHbarID = 1005;

const int uiOKid = 2000;
const int uiCancelId = 2001;



//message types
enum Messagetypes {
	change, userDraw, uiMsgDrop, uiMsgLMdown, uiMsgRMdown, uiMsgMouseMove,
	uiMsgLMouseUp, uiMsgRMouseUp, uiMouseWheel, uiClick, uiClickOutside, uiDataEntered, uiSpin, uiLostKeyboard,
	uiMsgHotTextClick, uiMsgHotTextChange, uiMsgChildResize, uiMsgSlide, uiMsgUpdate, uiMsgSave, uiMsgRestore,
	uiMsgMouseOff, uiMsgDoubleClick, uiMsgDelete, uiMsgDragging
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
#define GLFW_MOD_CONTROL			0x0002

//because GLFW doesn't have this
#define MY_DOUBLECLICK 0x00060006
