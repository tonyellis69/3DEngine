#pragma once

#include "Window.h"
#include "Engine.h"
#include "UI\GUIbase.h"
#include "UI\GUIroot.h"
#include "UI\GUIbutton.h"
#include "UI\GUIradio.h"
#include "UI\GUItextbox.h"
#include "UI\GUIscrollbar.h"
#include "UI\GUIgroup.h"
#include "UI\GUIcontainer.h"
#include "UI\GUIpanelContainer.h"
#include "UI\GUIimageGrid.h"
#include <stdio.h>
#include "Reader.h"
#include "UIengine.h"

#include <iostream> //for cerr
using namespace std;



class CBaseApp
{
public:
	CBaseApp(void);
	~CBaseApp(void);

	void RegisterHandlers();
	void start();
	void SetWindow(int width, int height);
	void SetWindow(const std::string& Title);
	void SetWindow( int width, int height, const std::string& Title, int Style);
	void sizeViewToWindow();
	virtual void OnKeyDown(unsigned int wParam, long lParam) {};
	virtual void draw() {};
	virtual void Update() {};
	virtual void onResize(int width, int height) {};
	virtual void keyCheck() {};
	virtual void mouseMove(int x, int y, int key) {};
	void onWinResize( int w, int h);
	void OnKeyPress(unsigned int Key, long Mod);
	void OnKeyRelease(unsigned int Key, long Mod);
	void OnCharEntry(unsigned int Key, long Mod);
	void OnMouseMsg(unsigned int Msg, unsigned int wParam, long lParam);
	void OnMouseWheelMsg(unsigned int wParam, long lParam);
	virtual void OnEvent() {};
	void AppTasks();
	void FullScreen();
	void UnFullScreen();
	void EatKeys();
	void DrawUI();
	void RegisterUIfunctors();
	virtual void onStart() {};
	void drawSkyDome();
	
	void initWatches();
	void updateWatches();


	CEngine Engine;
	CWindow Window;
	CGUIengine UIeng;
	CSoundLib snd;
	CGUIroot GUIroot;

	FILE* ErrStream;	///<Filestream error messages are written to.

	CDrawFuncs* drawFuncs; ///<Handle for the collection of delegates we pass to the GUI system.

	TSpecificFunctor<CBaseApp> KeyDownFunc;
	TSpecificFunctor<CBaseApp> KeyUpFunc;
	TSpecificFunctor<CBaseApp> CharEntryFunc;
	TMouseMsgFunctor<CBaseApp> MouseMsgFunc;
	TSpecificFunctor<CBaseApp> MouseWheelMsgFunc;
	TSpecificFunctor<CBaseApp> OnWinResizeFunc;
	TPlainFunctor<CBaseApp>	AppTasksFunc;


	bool KeyDown[0x100]; 
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

	CGUIlabel* wLabel1;
	CGUIlabel* wLabel2;
	CGUIlabel* wLabel3;
	CGUIlabel* consoleLbl;


	};

