#pragma once

#include <windows.h>  //for file dialogues
#include <glew.h>
//#include "Window.h"
#include "Engine.h"
#include "UI\GUIbase.h"
#include "UI\GUIroot.h"
#include "UI\GUIradio.h"
#include "UI\GUIscrollbar.h"
#include "UI\GUIgroup.h"
#include "UI\GUIcontainer.h"
#include "UI\GUIpanelContainer.h"
#include "UI\GUIimageGrid.h"
#include "UI\GUIlabel2.h"
#include <stdio.h>
#include "Reader.h"
#include "UIengine.h"

#include "UI\renderDrawFuncs.h"
#include "UI\font.h"

#include "SysFnt.h"
#include "smallsysf.h"

#include "..\VMtest\src\vmApp.h"

#include "win\win.h"

#include <iostream> //for cerr
using namespace std;


//class CVMapp;
class CBaseApp
{
public:
	CBaseApp(void);
	~CBaseApp(void);

	void loadSystemFonts();
	void start();
	void SetWindow( int width, int height, const std::string Title);
	virtual void onKeyDown( int key, long mod) {};
	virtual void draw() {};
	virtual void Update() {};
	virtual void onResize(int width, int height) {};
	virtual void keyCheck() {};
	virtual void mouseMove(int x, int y, int key) {};
	void onWinResize( int w, int h);
	void onKeyPress(int key, long Mod);
	void OnKeyRelease(unsigned int Key, long Mod);
	void OnCharEntry(unsigned int Key, long Mod);
	void onMouseMove(int x, int y);
	void onMouseButton(int button, int action, int mods);
	void OnMouseWheelMsg(unsigned int wParam, long lParam);
	virtual void OnEvent() {};
	void AppTasks();
	void DrawUI();
	void RegisterUIfunctors();
	virtual void onStart() {};
	void drawSkyDome();
	
	void initWatches();
	void updateWatches();

	string getExePath();

	virtual void vmMessage(TVMmsg) {};


	CEngine Engine;
	//CWindow Window;
	CGUIengine UIeng;
	CSoundLib snd;
	CGUIroot GUIroot;
	CWin win;

	FILE* ErrStream;	///<Filestream error messages are written to.

	CRenderDrawFuncs* drawFuncs; ///<Handle for the collection of delegates we pass to the GUI system.


//	TSpecificFunctor<CBaseApp> KeyDownFunc;
//	TSpecificFunctor<CBaseApp> KeyUpFunc;
//	TSpecificFunctor<CBaseApp> CharEntryFunc;
//	TMouseMsgFunctor<CBaseApp> MouseMsgFunc;
//	TSpecificFunctor<CBaseApp> MouseWheelMsgFunc;
//	TSpecificFunctor<CBaseApp> OnWinResizeFunc;
//	TPlainFunctor<CBaseApp>	AppTasksFunc;


	//bool KeyDown[0x100]; 
	bool BuiltInFullScrn; ///<If true, Tab toggles fullscreen.

	double Time;
	double LastTime;
	double dT; ///<Elapsed time in milliseconds since last frame.
	bool Paused; ///<If true, app is paused. Update functions will not be called.
	bool quitOnEsc; ///<If true, Escape key instant-quits.
	string homeDir; ///<Pathname of our home directory, typically the .exe directory.

	virtual void HandleUImsg(CGUIbase& Control, CMessage& Message) {};

	void mouseCaptured(bool capture);
	string getFilenameDlg(const std::string& title, const char* path);
	string saveFilenameDlg(const std::string& title, const char* path);
	void exit();
	void showMouse(bool on);
	void setMousePos(int x, int y);
	void getMousePos(int& x, int& y);
	bool keyNow(int vKey);

	int mouseX,mouseY;
	int dMouseX,dMouseY;
	int mouseKey;

	int viewWidth, viewHeight;

	CGUIlabel2* wLabel1;
	CGUIlabel2* wLabel2;
	CGUIlabel2* wLabel3;
	CGUIlabel2* consoleLbl;


	unsigned int hTextureShader;
	unsigned int hTexture;
	unsigned int hTextureMVP;

	CFont sysFont; ///<Default font
	CFont smallSysFont; ///<Default font for GUI


	CVMapp vm; ///<The Tig virtual machine.


	};

