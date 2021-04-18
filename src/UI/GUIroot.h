#pragma once
#include "GUIbase.h"




/** The class defining GUIroot, the parent UI control to which all other controls are attached. */
class CGUIroot : public CGUIbase {
public:
	CGUIroot();
	~CGUIroot();
	bool MouseMsg(unsigned int Msg, int mouseX, int mouseY, int key);
	bool MouseWheelMsg(const  int mouseX, const  int mouseY, int wheelDelta, int key);
	void setLocalDimensions(int x, int y, int w, int h);
	void recalculateClipbox();
	void KeyboardMsg(unsigned int Key, long Mod);
	void CharEntryMsg(unsigned int Key, long Mod);
	void setDrawFuncs(CDrawFuncs* drawFunc);
	void Draw(float dT);
	bool OnMouseMove(const int mouseX, const int mouseY, int key);
	bool OnLMouseDown(const  int mouseX, const  int mouseY, int key);
	void OnRMouseDown(const  int mouseX, const  int mouseY);
	//void onRMouseUp(const int mouseX, const int mouseY);
	void onLMouseUp(const  int mouseX, const  int mouseY);
	bool OnClick(const  int mouseX, const  int mouseY);
	void addModal(CGUIbase* control);
	void deleteModal();
	void removeModal();
	void setFocus(CGUIbase* control);
	void captureMouse(bool on);

	//CGUIbase* modalControl; ///<Pointer to the current modal control, if any.
	CGUIbase* focusControl; ///<The current control with focus, if any.


	//CThemeServer themeServer; ///<Stores the available text style collecctions
};

extern CGUIroot* pGUIroot; 
//TO DO: replace with a global GUIroot object.
