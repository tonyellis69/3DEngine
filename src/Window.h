#pragma once
#include <windows.h>
#include <string>
#include "Delegate.h"

using namespace std;


/** Standalone class for creating an MS window. Provides functors and 
// a handler registration method enabling window events to be handled
// externally. */

/** Functors to enable external app to pass methods as event-handlers. */

class TFunctor {
public:
	  	virtual void Call(const WPARAM wParam, const LPARAM lParam) {};
		virtual void Call() {};
		virtual void Call(unsigned int Msg, const WPARAM wParam, const LPARAM lParam) {};
};

template <class TClass> class TSpecificFunctor: public TFunctor {
private:
	void (TClass::*fpt) (const WPARAM, const LPARAM ); //pointer to member function
	TClass* pt2Object; //pointer to object

public:
    // constructor - takes pointer to an object and pointer to a member and stores
    // them in two private variables
	void Set(TClass* _pt2Object, void (TClass::*_fpt)(const WPARAM , const LPARAM ))
	{pt2Object = _pt2Object; fpt = _fpt; };

	//override function Call
	virtual void Call(const WPARAM wParam, const LPARAM lParam)
	{ (*pt2Object.*fpt)(wParam, lParam);};
};

template <class TClass> class TPlainFunctor: public TFunctor {
private:
	void (TClass::*fpt) (void); //pointer to member function
	TClass* pt2Object; //pointer to object

public:
    // constructor - takes pointer to an object and pointer to a member and stores them in two private variables
	void Set(TClass* _pt2Object, void (TClass::*_fpt)(void ))
	{pt2Object = _pt2Object; fpt = _fpt; };

	//override function Call
	virtual void Call(void)
	{ (*pt2Object.*fpt)();};
};

template <class TClass> class TMouseMsgFunctor: public TFunctor {
private:
	void (TClass::*fpt) (unsigned int Msg, const WPARAM , const LPARAM ); //pointer to member function
	TClass* pt2Object; //pointer to object

public:
    // constructor - takes pointer to an object and pointer to a member and stores them in two private variables
	void Set(TClass* _pt2Object, void (TClass::*_fpt)(unsigned int Msg, const WPARAM , const LPARAM ))
	{pt2Object = _pt2Object; fpt = _fpt; };

	//override function Call
	virtual void Call(unsigned int Msg, const WPARAM wParam, const LPARAM lParam )
	{ (*pt2Object.*fpt)(Msg,wParam, lParam);};
};
 

//Mousepointer types
enum TMouseState {normal,EW,NS,NESW,NWSE,mouseMove, noChange};

/** Class for creating a complete MS window. */
class CWindow {
public:
	CWindow();
	~CWindow();
	void RegisterKeypressHandler(TFunctor* Func);
	void RegisterKeyreleaseHandler(TFunctor* Func);
	void RegisterCharEntryHandler(TFunctor* Func);
	void RegisterMouseWheelMsgHandler(TFunctor* Func);
	void RegisterMouseMsgHandler(TFunctor* Func);
	void RegisterAppTasksHandler(TFunctor* Func);
	DelegatePP<void,int,int> resizeHandler; 
	void WinLoop();
	void trapLMouseDown();
	void SetWindow(int x, int y, int width, int height, const std::string& Title, int Style);
	void FullScreen();
	void UnFullScreen();
	void EnumerateDisplaySettings();
	bool ChangeDisplay(int x, int y, int depth);
	void mouseCaptured(bool capture);
	string getFilenameDlg(const std::string&  title, const char* path);
	string saveFilenameDlg(const std::string&  title,const char* path);
	void setMousePointer(TMouseState pointer);
	void quitWindow();
	string getExePath();
	void getDrawingArea(int& width, int& height);
	void showMouse(bool on);
	void setMousePos(int x, int y);
	void getMousePos(int& x, int& y);
	bool keyNow(int vKey);

	HWND hWnd; ///<The handle for the window.

	RECT  WinDimensions; ///<Handy store for the window dimensions when not fullscreen.
	bool bFullscreen;
	int ScreenWidth; ///<Guaranteed to be current screen width in pixels;
	int ScreenHeight;  ///<Guaranteed to be current screen height in pixels;
	TMouseState mouseState;// <Whether mouse is E-W,etc or normal;


private:
	static LRESULT CALLBACK StaticWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT WndProc(UINT message, WPARAM wParam, LPARAM lParam);
	RECT OptimalWindowSize();
	void GetScreenSize();


	TFunctor* KeypressHandler; ///<Pointer to an external method.
	TFunctor* KeyreleaseHandler; ///<Pointer to an external method.
	TFunctor* CharEntryHandler;  ///<Pointer to an external method.
	TFunctor* MouseMsgHandler; ///<Pointer to an external method.
	TFunctor* MouseWheelMsgHandler; ///<Pointer to an external method.
	TFunctor* SizeHandler; ///<Pointer to an external method.
	TFunctor* DoAppTasks; ///<Pointer to an external method.

	int WinStyle; ///<Stores the windows style the user selected, if any.
};

//Windows styles
#define WIN_NORMAL		1
#define WIN_FULL		2
#define WIN_BORDERLESS	4


//My made up Windows message for left-mouse-held-down
#define LM_HELD_DOWN 100

