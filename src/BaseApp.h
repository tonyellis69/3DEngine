#pragma once


#define _USE_MATH_DEFINES //for cmath
#define GLM_ENABLE_EXPERIMENTAL

#include <windows.h>  //for file dialogues
#include <glew.h>

#include "UI\GUIbase.h"
#include "UI\GUIroot.h"
#include "UI\GUIradio.h"
#include "UI\GUIscrollbar.h"
#include "UI\GUIgroup.h"
#include "UI\GUIcontainer.h"
#include "UI\GUIpanelContainer.h"
#include "UI\GUIimageGrid.h"
#include "UI\GUIlabel2.h"
#include "UI\GUIrichText.h"
#include <stdio.h>

#include "UI\font.h"

#include "SysFnt.h"
#include "smallsysf.h"

//#include "..\VMtest\src\vmApp.h"

#include "win\win.h"

#include "utils/log.h"

#include "importer/importer.h"


//#include <iostream> //for cerr
//using namespace std;

#include "UI\iGuiMsg.h"

#include "physics2/physEng.h"

#include "messaging/messenger.h"

#include "timer.h"

#include "UI/guiRoot2.h"


//class CVMapp;
class CBaseApp : public Icallback, ILogCallback, IGUImsg, public CMessenger {
public:
	CBaseApp(void);
	~CBaseApp(void);

	void loadSystemFonts();
	void start();
	void SetWindow( int width, int height, const std::string Title);
	virtual void onKeyDown( int key, long mod) {};
	virtual void onKeyUp(int key, long mod) {};
	virtual void draw() {};
	virtual void Update() {};
	virtual void onResize(int width, int height) {};
	virtual void keyCheck() {};
	virtual void mouseMove(int x, int y, int key) {};
	virtual void onMouseButton(int button, int action, int mods) {};
	void onWinResize( int w, int h);
	void onKeyPress(int key, long Mod);
	void OnKeyRelease(unsigned int Key, long Mod);
	void OnCharEntry(unsigned int Key, long Mod);
	void onWinMouseMove(int x, int y);
	void onWinMouseButton(int button, int action, int mods);
	virtual bool OnMouseWheelMsg(float xoffset, float yoffset);
	virtual void OnEvent() {};
	void AppTasks();
	void DrawUI();
	//void RegisterUIfunctors();
	virtual void onStart() {};

	void initLogWindow();
	void logCallback(std::stringstream& logStream);
	void logAlertCallback();
	void logFatalCallback(std::stringstream& logStream);

	std::string getExePath();


	void initialiseSystemStylesheet();

	CGUIrichText* logWindow; ///<Points to the system log window.

	CGUIroot GUIroot;
	CWin win;

	FILE* ErrStream;	///<Filestream log error messages are written to.


	bool BuiltInFullScrn; ///<If true, Tab toggles fullscreen.

	double Time;
	double LastTime;
	double dT; ///<Elapsed time in milliseconds since last frame.
	bool Paused; ///<If true, app is paused. Update functions will not be called.
	bool quitOnEsc; ///<If true, Escape key instant-quits.
	std::string homeDir; ///<Pathname of our home directory, typically the .exe directory.

	virtual void HandleUImsg(CGUIbase& Control, CMessage& Message) {};

	void mouseCaptured(bool capture);
	std::string getFilenameDlg(const std::string& title, const char* path);
	std::string saveFilenameDlg(const std::string& title, const char* path);
	void toggleLogWindow();
	void exit();
	void showMouse(bool on);
	void setMousePos(int x, int y);
	glm::i32vec2 getMousePos();
	bool keyNow(int vKey);
	bool mouseButtonNow(int button);
	void GUImsg(int ctrlId, TGUImessage& msg);

	glm::i32vec2 lastMousePos = { 0,0 };
	int mouseX,mouseY;
	int dMouseX,dMouseY;
	int mouseKey;

	int viewWidth, viewHeight;

	CFont* sysFont; ///<Default font
	CFont* smallSysFont; ///<Default font for GUI


	std::string dataPath; //<Path to app data folder

	CImporter importer; ///<Object for importing 3D scenes.

	CPhysEng physEng;

	CTimer timer;

	CguiRoot2 guiRoot2; ///<To replace GUIroot as basis of all gui.
	};

