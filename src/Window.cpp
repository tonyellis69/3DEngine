#include "Window.h"
#include <tchar.h>
#include <iostream>
using namespace std;
#include <shlwapi.h>

/** Create and display a Windows window for use as a drawing surface. */
CWindow::CWindow(): WinStyle (WS_CAPTION|WS_SYSMENU){
	return;


	bFullscreen = false; 
    WNDCLASSEX wc;

	//Find screen dimensions
	GetScreenSize();

    // clear out the window class for use
    ZeroMemory(&wc, sizeof(WNDCLASSEX));
	HINSTANCE hInstance = GetModuleHandle(NULL);

    // fill in the struct with the needed information
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)StaticWinProc;
    wc.hInstance =  hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.lpszClassName = _T("WindowClass1"); (LPCSTR)"WindowClass1";

//	static TCHAR szWindowClass[] = _T("win32app");
//static TCHAR szTitle[] = _T("Win32 Guided Tour Application");
   RegisterClassEx(&wc);     // register the window class

   //Get screen size
   WinDimensions = OptimalWindowSize();

    // create the window and use the result as the handle
    hWnd = CreateWindowEx(NULL,
                          _T("WindowClass1"),    // name of the window class
                          _T("2D app window"),   // title of the window
                          WinStyle   ,    // window style
                          WinDimensions.left,    // x-position of the window
                          WinDimensions.bottom,    // y-position of the window
                          WinDimensions.right - WinDimensions.left,    // width of the window
                          WinDimensions.top - WinDimensions.bottom,    // height of the window
                          NULL,    // we have no parent window, NULL
                          NULL,    // we aren't using menus, NULL
                          hInstance,    // application handle
                          this);    // store a pointer to this instance, for our WndProc to use

	EnumerateDisplaySettings();
	mouseState = normal;
}


/** Simple static Windows-friendly WndProc that forwards messages to the real WndProc of our CWindow instance. */
LRESULT CALLBACK CWindow::StaticWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
 CWindow *wnd = 0;

 if(message == WM_NCCREATE) {//This message is called very early, when our CWindow instance may
	 //not be fully initialised, so we trap it here.
    // Its lParam conveniently points to a struct holding all the data in the CreateWindowEx call.
	// So we can retrieve the pointer to this CWindow instance we placed there during construction.
    wnd = (CWindow*)((LPCREATESTRUCT)lParam)->lpCreateParams;
    SetWindowLongPtr(hWnd, GWL_USERDATA, (LONG_PTR)wnd); //store it in the window user data for future access

	wnd->hWnd = hWnd; 	// save window handle
  }
  else
    // retrieve the pointer to our CWindow instance, now stored in the user data
    wnd = (CWindow*)(GetWindowLongPtr(hWnd, GWL_USERDATA));

  if(!wnd) return DefWindowProc(hWnd,message,wParam,lParam); //Can't find our instance? Do default message handling.
  
  // otherwise, call the message handler of our windows instance
  return wnd->WndProc(message, wParam, lParam);	
}


/** Custom WndProc message handler for a CWindow instance. */
LRESULT CWindow::WndProc(UINT message, WPARAM wParam, LPARAM lParam) {
	 // sort through and find what code to run for the message given
    switch(message)
    {
        // this message is read when the window is closed
        case WM_DESTROY:
            {
                PostQuitMessage(0);
                return 0;
            } break;
		case WM_CREATE:	{
		//	App->OnCreate( hWnd, message);
			break;
			}
		case WM_SIZE:	{
			//SizeHandler->Call(LOWORD(lParam),HIWORD(lParam));
			////////////////////resizeHandler(LOWORD(lParam),HIWORD(lParam));
			break;
			}
		case WM_KEYDOWN: {
		/*	if (wParam == VK_ESCAPE) {
				 PostQuitMessage(0);
				 return 0;}	*/

			//KeypressHandler->Call(wParam,lParam);
			break;
			}
		case WM_KEYUP: {
			//KeyreleaseHandler->Call(wParam,lParam);
			break;
			}
		case WM_CHAR: {
			//CharEntryHandler->Call(wParam,lParam);
			break;
			}
		case WM_MOUSEMOVE: 
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN: {
			MouseMsgHandler->Call(message,wParam,lParam);
			break;
			}
		case WM_MOUSEWHEEL: {
			MouseWheelMsgHandler->Call(wParam,lParam);
			break;
			}
		case WM_ERASEBKGND: {
			return false;
			break; }
		case WM_SETCURSOR : {
			//setCursorHandler(wParam,lParam);
			setMousePointer(mouseState);
			return true;
			break;				}
    }
    // Handle any messages the switch statement didn't
    return DefWindowProc (hWnd, message, wParam, lParam);
}


/** Receives an external method to use as a keypress event handler. */
void CWindow::RegisterKeypressHandler(TFunctor* Func) {
	KeypressHandler = Func;
}

/** Receives an external method to use as a keyrelease event handler. */
void CWindow::RegisterKeyreleaseHandler(TFunctor* Func) {
	KeyreleaseHandler = Func;
}

void CWindow::RegisterCharEntryHandler(TFunctor* Func) {
	CharEntryHandler = Func;
}


void CWindow::RegisterMouseMsgHandler(TFunctor* Func) {
	MouseMsgHandler = Func;
}

void CWindow::RegisterMouseWheelMsgHandler(TFunctor* Func) {
	MouseWheelMsgHandler = Func;
}


/** Receives an external method to use as a misc task handler. */
void CWindow::RegisterAppTasksHandler(TFunctor* Func) {
	DoAppTasks = Func;
}


/** Start running the window message pump, don't return until we quit the window. */
void CWindow::WinLoop()	{ 
	
	ShowWindow(hWnd, SW_SHOW); //we don't need to show the window until we're ready.

	//TO DO: get actual window size with a windows function!

	MSG msg;// this struct holds Windows event messages

   while(TRUE) {
		// Check to see if any messages are waiting in the queue
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				break;
			
			// Translate the message and dispatch it to WindowProc()
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		trapLMouseDown();
		
		// In between handling messages, run the app's tasks.
		DoAppTasks->Call();
	}
}

/** Sends a message to the client's mouse message handler if the left mouse button is down. There is no Windows message 
	for this, so we create our own. */ 
void CWindow::trapLMouseDown() {
	if ((GetKeyState (VK_LBUTTON)) < 0) {
		POINT Mouse;
		GetCursorPos(&Mouse);
		ScreenToClient(hWnd,&Mouse);
		LPARAM lParam = MAKELPARAM(Mouse.x , Mouse.y ); 
		MouseMsgHandler->Call(LM_HELD_DOWN,0,lParam);
	}
}

/** Adjust the dimensions of the window. */
void CWindow::SetWindow(int x, int y, int width, int height, const std::string& Title, int Style) {
	std::wstring ws(Title.begin(), Title.end());
	SetWindowTextW(hWnd,ws.c_str());

	if (Style & WIN_NORMAL) 
		WinStyle =  (WS_CAPTION|WS_SYSMENU|WS_THICKFRAME);
	if (Style & WIN_FULL) 
		WinStyle  = WS_OVERLAPPEDWINDOW;
	if (Style & WIN_BORDERLESS)
		//WinStyle = WS_BORDER;
		WinStyle &= ~(WS_BORDER | WS_DLGFRAME);


	if (Style & WIN_FULL) {
		if (width != -1) {//user wants to be fullscreen at requested screen size
			ChangeDisplay(width, height, 32);
		}	
		FullScreen(); //otherwise fullscreen at current screen size
		return;
		
	} 
	else {	
		if ((x == 0)||(y == 0)) { //user isn't interested in positioning, happy with default window.
			//SetWindowLongPtr(hWnd,GWL_STYLE,WinStyle);
		//	return;
		}
	if (x == -1)
		x = (ScreenWidth - width)/2;
	if (y == -1)
		y = (ScreenHeight - height)/2;
	if (x < 0) x = 0;
	if (y < 0) y = 0;
	
	//if (Style) {
		SetWindowLongPtr(hWnd,GWL_STYLE,WinStyle);
		SetWindowPos(hWnd, HWND_TOP, x, y,  width,   height, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	//}
	}
	//MoveWindow(hWnd,x,y,width,height,true);
}



/** Make the window fullscreen. */
void CWindow::FullScreen() {
	ShowWindow(hWnd, SW_SHOW);
	GetScreenSize();
	int x = GetWindowRect(hWnd,&WinDimensions);

	SetWindowLongPtr(hWnd, GWL_STYLE,WS_SYSMENU | WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE);
	//MoveWindow(hWnd, 0, 0, width, height, TRUE); //below gets rid of taskbar:
	 SetWindowPos(hWnd, HWND_TOP, 0, 0,  ScreenWidth,   ScreenHeight, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	bFullscreen = true;
	//ShowCursor(false);
}

/** Revert window to whatever dimensions it had before it went fullscreen. */
void CWindow::UnFullScreen() {
	//TO DO: if we previously changed resolution, now change back
	SetWindowLongPtr(hWnd, GWL_STYLE, WinStyle| WS_VISIBLE);
	MoveWindow(hWnd, WinDimensions.left, WinDimensions.top, WinDimensions.right - WinDimensions.left, WinDimensions.bottom - WinDimensions.top, TRUE);
	bFullscreen = false;
	//ShowCursor(true);
}	

/** Return window dimensions that will fit the current screen size. Default is 800x600. */
RECT CWindow::OptimalWindowSize() {
	RECT WinSize; int WinW, WinH;
	if (ScreenHeight < 600) 
		WinH = ScreenHeight - 20;
	else
		WinH = 600;

	WinW = (int) (WinH / 0.75f); //ie, 800 when height is 600;

	if (ScreenWidth < WinW) //not very likely now, but just in case
		WinW = ScreenWidth - 10;
	
	WinSize.left = (ScreenWidth-WinW) / 2;
	WinSize.bottom = (ScreenHeight-WinH) /2;
	WinSize.right = WinSize.left + WinW;
	WinSize.top = WinSize.bottom + WinH;

	return WinSize;
}


void CWindow::GetScreenSize() {
	ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
	ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
}

void CWindow::EnumerateDisplaySettings() {
	DEVMODE DevStruc;
	int Mode = 0; bool Result;
	do { Result = EnumDisplaySettings(NULL,Mode,&DevStruc) !=0;
		Mode++;
		if (Result) {
			cout << "\n" << DevStruc.dmPelsWidth << " x " <<  DevStruc.dmPelsHeight << "  depth: " <<
         DevStruc.dmBitsPerPel ;

		}
	} while (Result) ;

}

/** Change screen resolution to the given setting. */
bool CWindow::ChangeDisplay(int x, int y, int depth) {
	DEVMODE DisplayMode;
	memset(&DisplayMode, 0, sizeof(DisplayMode));
	if(!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &DisplayMode)){
		return false ;
	}
	DisplayMode.dmPelsWidth	 = x;
	DisplayMode.dmPelsHeight = y;
	//DisplayMode.dmBitsPerPel = depth;
	int ChangeDisplayResult = ChangeDisplaySettings(&DisplayMode, CDS_FULLSCREEN);	
	if(ChangeDisplayResult != DISP_CHANGE_SUCCESSFUL){
		return false;
	} 
	return true;
}

void CWindow::mouseCaptured(bool capture) {
	if (capture)
		SetCapture(hWnd);
	else
		ReleaseCapture();
}

/** Use the Windows Common Dialog to get a filename. */
string CWindow::getFilenameDlg(const std::string&  title,const char* path) {
	const int BUFSIZE = 1024;
    CHAR buffer[BUFSIZE] = {0};
    OPENFILENAME ofns = {0};
    ofns.lStructSize = sizeof( ofns );
    ofns.lpstrFile = buffer;
    ofns.nMaxFile = BUFSIZE;
    ofns.lpstrTitle = (title.c_str()); 
	ofns.lpstrInitialDir = path;
	GetOpenFileName( & ofns );
	return buffer;
}

string CWindow::saveFilenameDlg(const std::string&  title,const char* path) {
	const int BUFSIZE = 1024;
    CHAR buffer[BUFSIZE] = {0};
    OPENFILENAME ofns = {0};
    ofns.lStructSize = sizeof( ofns );
    ofns.lpstrFile = buffer;
    ofns.nMaxFile = BUFSIZE;
    ofns.lpstrTitle = (title.c_str()); 
	ofns.lpstrInitialDir = path;
	GetSaveFileName( & ofns );
	return buffer;
}


void CWindow::setMousePointer(TMouseState pointerType) {
	//get a handle to the cursor type
	HCURSOR cursor;
	switch (pointerType) {
		case noChange : return;
		case normal: cursor = LoadCursor(NULL,IDC_ARROW);
			break;
		case EW: cursor =  LoadCursor(NULL,IDC_SIZEWE); 
			break;
		case NS: cursor =  LoadCursor(NULL,IDC_SIZENS); 
			break;
		case NESW: cursor =  LoadCursor(NULL,IDC_SIZENESW); 
			break;
		case NWSE: cursor =  LoadCursor(NULL,IDC_SIZENWSE); 
			break;
		case mouseMove: cursor =  LoadCursor(NULL,IDC_SIZEALL); 
			break;

		
	} 
	//set the mousepointer to this cursor.
	SetCursor(cursor);	
}

void CWindow::quitWindow() {
	PostQuitMessage(0);
}

/** Returns the file path of the current program. */
string CWindow::getExePath() {
	char path[512];
	GetModuleFileName(NULL,(char*)path,512);
	string tmp(path);
	int n  = tmp.find_last_of(_T('\\'));
	tmp = tmp.substr( 0, n+1 );
	//tmp = tmp.;
	return tmp;
}

/** Return the dimensions of the window area we can draw on. 
	That is, excluding borders, title bar, etc.*/
void CWindow::getDrawingArea(int& width, int& height) {
	RECT rect; 
	GetClientRect(hWnd, &rect); 
	width = rect.right - rect.left; 
	height = rect.bottom - rect.top; 
}

/** Turns mouse pointer on or off.*/
void CWindow::showMouse(bool on) {
	int mouseCounter = ShowCursor(on);
	while ( mouseCounter > 0) {
		mouseCounter = ShowCursor(false);
		mouseCounter--;
	}

	while ( mouseCounter < -1) {
		mouseCounter = ShowCursor(true);
		mouseCounter++;
	}
}

/** Set mouse pointer to the given window position. Minus values are centred.*/
void CWindow::setMousePos(int x, int y) {

	int width, height;
	getDrawingArea(width,height);

	if (x < 0) x = width/2;
	if (y < 0) y = height/2;
	POINT pt;
	pt.x = x; pt.y = y;
	ClientToScreen(hWnd, &pt);
	SetCursorPos(pt.x,pt.y);
//	SetCursorPos(x,y);

}


//get current mouse position
void CWindow::getMousePos(int& x, int& y) {
	return;
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(hWnd, &pt);
	x = pt.x; y = pt.y;
}

/** Return true if given key is down at this instant. */
bool CWindow::keyNow(int vKey) {
	return GetAsyncKeyState(vKey) && 0x4000;
}

CWindow::~CWindow() {
	ShowCursor(true);	
}

