
#include "BaseApp.h"
#include <iostream> //for cerr
#include <Windowsx.h> //for get_mouse_x etc

#include <glm/glm.hpp>

#include "watch.h"

using namespace watch;


CBaseApp::CBaseApp(void)  {
	homeDir = Window.getExePath();
#ifdef _DEBUG
	homeDir += "..\\";
#endif
	string logpath = homeDir + "ErrorLog.txt";
	freopen_s(&ErrStream,logpath.c_str(),"w",stderr);

	cerr << "\nBaseApp constructor.";

	initWatches();

	Engine.dataPath = homeDir + "Data\\";
	
	//Set up our handlers for window events
	RegisterHandlers();
	EatKeys();

	UIeng.pEngine = &Engine;
	
	RegisterUIfunctors();

	Time = LastTime = 0;
	mouseX = mouseY = 0;

	BuiltInFullScrn = true;
	Paused = false;

	//Set up engine for rendering
	Engine.init(Window.hWnd);

	quitOnEsc = true;
}

void CBaseApp::RegisterHandlers() {
	KeyDownFunc.Set(this, &CBaseApp::OnKeyPress); //set the  functor to point to CBaseApp's keypress handler...
	Window.RegisterKeypressHandler(&KeyDownFunc); //...then pass the functor to the window to use when it detects a keypress.

	KeyUpFunc.Set(this, &CBaseApp::OnKeyRelease);
	Window.RegisterKeyreleaseHandler(&KeyUpFunc);

	
	CharEntryFunc.Set(this, &CBaseApp::OnCharEntry);
	Window.RegisterCharEntryHandler(&CharEntryFunc);

	MouseMsgFunc.Set(this, &CBaseApp::OnMouseMsg);
	Window.RegisterMouseMsgHandler(&MouseMsgFunc);

	MouseWheelMsgFunc.Set(this, &CBaseApp::OnMouseWheelMsg);
	Window.RegisterMouseWheelMsgHandler(&MouseWheelMsgFunc);

	//OnWinResizeFunc.Set(this, &CBaseApp::onWinResize);
	//Window.RegisterSizeHandler(&OnWinResizeFunc);
	Window.resizeHandler.Set(this,&CBaseApp::onWinResize); 

	AppTasksFunc.Set(this, &CBaseApp::AppTasks);
	Window.RegisterAppTasksHandler(&AppTasksFunc);
}

/** Starts the application loop. This will not return until the app is shut down. */
void CBaseApp::start() {
	LastTime = Engine.Time.milliseconds();
	onStart();
	Window.WinLoop();
}

/** Sets the application window to the given dimensions. */
void CBaseApp::SetWindow( int width, int height) {
	Window.SetWindow(-1,-1,width,height, "", 0);
	sizeViewToWindow();
}

void CBaseApp::SetWindow( int width, int height, const std::string& Title, int Style) {
	Window.SetWindow(-1,-1,width,height,Title, Style);
	sizeViewToWindow();
}

void CBaseApp::SetWindow(const std::string& Title) {
	Window.SetWindow(0,0,0,0, Title, WIN_NORMAL);
	sizeViewToWindow();
}

/** Directly sets the width and height of the renderer drawing area to our window dimensions.
	Useful because a Windows WinResize message may not arrive in time for the user. */
void CBaseApp::sizeViewToWindow() {
	int w,h; Window.getDrawingArea(w,h);
	onWinResize(w,h);
}

/** Handler for when our window is rezized. */
void CBaseApp::onWinResize( int w, int h) {
	viewWidth = w; viewHeight = h;
	Engine.resizeView(0,0,w,h); //We need to change shape of view.
	GUIroot.SetPos(0,0,w,h); //Resize the base UI control so it's always the size of the view.
	onResize(w,h); //Call the user's resize handler, in case they want to do something;
}

/** Event handler for when the OS messages that a key is pressed down. */
void CBaseApp::OnKeyPress(unsigned int Key, long Mod) {
	KeyDown[Key] = true; //NB some lag with this.
	if ((Key == VK_TAB) && (BuiltInFullScrn)) {
		Window.bFullscreen ? Window.UnFullScreen() : Window.FullScreen();
		return;
	}
	if ((Key == VK_ESCAPE) && (quitOnEsc)){
		Window.quitWindow();
		return;
	}	
	//send the key to the UI
	GUIroot.KeyboardMsg(Key, Mod);
	OnKeyDown(Key,Mod); //Call the user's key handler - useful when we don't want feedback every frame
}

/** Event handler for when a key is released. */
void CBaseApp::OnKeyRelease(unsigned int Key, long Mod) {
	KeyDown[Key] = false;
}

/** Event handler for when a key is pressed and we want to handle it as a character. Eg, textbox entry. */
void CBaseApp::OnCharEntry(unsigned int Key, long Mod) {
	GUIroot.CharEntryMsg(Key, Mod);
}

/** Event handler for mouse messages such as mouse movement. */
void CBaseApp::OnMouseMsg(unsigned int Msg, unsigned int wParam, long lParam) {
	
	GUIroot.MouseMsg(Msg,GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam),wParam);
	//dMouseX =0;dMouseY=0;
	if (Msg == WM_MOUSEMOVE) {
		//int oldMouseX = mouseX; int oldMouseY = mouseY;
	//	mouseX = GET_X_LPARAM(lParam); //TO DO: move outside if statement?
	//	mouseY = GET_Y_LPARAM(lParam);
		//dMouseX = mouseX - oldMouseX;
		//dMouseY = mouseY - oldMouseY;
		mouseKey = wParam;
		//mouseMove(mouseX,mouseY,wParam);
	}

}

/** Event handler for mouse wheel messages. */
void CBaseApp::OnMouseWheelMsg(unsigned int wParam, long lParam) {
	POINT Mouse;
	GetCursorPos(&Mouse);
	ScreenToClient(Window.hWnd,&Mouse);
	int delta = GET_WHEEL_DELTA_WPARAM(wParam) / 120;
	if (GUIroot.IsOnControl(GUIroot,Mouse.x, Mouse.y))
		GUIroot.MouseWheelMsg(Mouse.x,Mouse.y,delta,GET_KEYSTATE_WPARAM(wParam));
}


void CBaseApp::AppTasks() {
	//Pre-draw
//Engine.Renderer.setTextureMode(true);

	if (Engine.MakingFit) //activate the global scaling matrix, if we're using one.
		//Engine.resetViewMatrix(); //reset to identity matrix, no scaling
		Engine.applyGlobalScale(); //TO DO: messes GUI - look into or scrap - prob due to line-drawing methods

	Engine.clearFrame();

	

	Time = Engine.Time.milliseconds();
	dT = Time - LastTime;
	Engine.dT = dT;
	LastTime = Time;
	getMousePos(mouseX, mouseY);
	keyCheck();

	//TO DO: mouseCheck, which calls user with last reported mouse position.
	if (!Paused) {
		Engine.updateRegisteredSprites(dT);
		Update();
	}


	if (Engine.skyDome) {
		drawSkyDome();
	}


	Engine.applyUserScale(); //Scale matrix in case scene or other user drawing is zoomed

	Engine.drawModels();
	
	Engine.drawSceneLayers();
	Engine.drawRegisteredSprites();
	draw(); //Do user drawing

	

	Engine.removeUserScale();

	updateWatches();
	DrawUI(); 



	Engine.showFrame();


}

void CBaseApp::FullScreen() {
	Window.FullScreen();
}

void CBaseApp::UnFullScreen() {
	Window.UnFullScreen();
}

void CBaseApp::EatKeys() {
	for (int x=0;x<0x100;x++) 
		KeyDown[x] = false;
}

/** Draw the UI system. */
void CBaseApp::DrawUI() {
	Engine.setCurrentShader(0);
	Engine.setDrawColour(engineWhite);
	GUIroot.Draw();
	Engine.setClip(0,0,GUIroot.width,GUIroot.height);
}

/** Assign engine functions to the UI so it can draw itself, etc. */
void CBaseApp::RegisterUIfunctors() {	
	drawFuncs = new CDrawFuncs;
	drawFuncs->setFont.Set(&UIeng,&CGUIengine::setFont);
	drawFuncs->setDrawColours.Set(&UIeng, &CGUIengine::setDrawColours);
	drawFuncs->getTextWidth.Set(&UIeng, &CGUIengine::getTextWidth);
	drawFuncs->drawIcon.Set(&UIeng, &CGUIengine::drawIcon);
	drawFuncs->handleUImsg.Set(this, &CBaseApp::HandleUImsg);
	drawFuncs->drawRect.Set(&UIeng, &CGUIengine::drawRect);
	drawFuncs->drawBorder.Set(&UIeng, &CGUIengine::DrawBorder);
	drawFuncs->setClip.Set(&UIeng, &CGUIengine::setClip);
	drawFuncs->drawText.Set(&UIeng, &CGUIengine::drawText);
	drawFuncs->drawTextMultiLine.Set(&UIeng, &CGUIengine::drawTextMultiLine);
	drawFuncs->drawLine.Set(&UIeng, &CGUIengine::drawLine);
	drawFuncs->setIconset.Set(&UIeng, &CGUIengine::setIconset);
	drawFuncs->drawTile.Set(&UIeng, &CGUIengine::drawTile);
	drawFuncs->drawImage.Set(&UIeng, &CGUIengine::drawImage);
	drawFuncs->mouseCaptured.Set(this, &CBaseApp::mouseCaptured);
	drawFuncs->setCursor.Set(&UIeng, &CGUIengine::setCursor);
	drawFuncs->setDrawColoursConditional.Set(&UIeng, &CGUIengine::setDrawColoursConditional);
	drawFuncs->drawDottedRect.Set(&UIeng,&CGUIengine::drawDottedRect);
	drawFuncs->setScale.Set(&UIeng,&CGUIengine::setScale);

	GUIroot.setDrawFuncs(drawFuncs);
}

void CBaseApp::drawSkyDome() {
	
	Engine.Renderer.setDepthTest(false);

	Engine.skyDome->dome->setPos(Engine.currentCamera->getPos() + glm::vec3(0, -0.5f, 0));
	glm::mat4 mvp = Engine.currentCamera->clipMatrix * Engine.skyDome->dome->worldMatrix;
	Engine.setCurrentShader(Engine.skyDome->hSkyDomeProg);
	Engine.setShaderValue(Engine.skyDome->hMVPmatrix, mvp);
	Engine.setShaderValue(Engine.skyDome->hSkyDomeHeightColours, 4, Engine.skyDome->heightColours[0]);
	Engine.skyDome->dome->drawNew();

	Engine.Renderer.setDepthTest(true);
}









void CBaseApp::mouseCaptured(bool capture) {
	Window.mouseCaptured(capture);
}

string CBaseApp::getFilenameDlg(const std::string& title, const char* path) {
	return Window.getFilenameDlg(title,path);
}

string CBaseApp::saveFilenameDlg(const std::string& title, const char* path) {
		return Window.saveFilenameDlg(title,path);
}

/** Turns mouse pointer on or off.*/
void CBaseApp::showMouse(bool on) {
	Window.showMouse(on);
}

void CBaseApp::setMousePos(int x, int y) {
	Window.setMousePos(x,y);
}

void CBaseApp::getMousePos(int& x, int& y) {
	Window.getMousePos(x,y);
}

bool CBaseApp::keyNow(int vKey) {
	return Window.keyNow(vKey);
}


/** Shut down the entire app and exit. */
void CBaseApp::exit() {
	Window.quitWindow();
}


void CBaseApp::initWatches() {
	wLabel1 = new CGUIlabel(800,100,400,50);
	wLabel1->txtColour = UIwhite;
	wLabel1->TextFont = 0;
	GUIroot.Add(wLabel1);

	wLabel2 = new CGUIlabel(800,150,400,50);
	wLabel2->txtColour = UIwhite;
	wLabel2->TextFont = 0;
	GUIroot.Add(wLabel2);

	wLabel3 = new CGUIlabel(800,200,400,50);
	wLabel3->txtColour = UIwhite;
	wLabel3->TextFont = 0;
	GUIroot.Add(wLabel3);

	consoleLbl = new CGUIlabel(10,10,100,100);
	consoleLbl->txtColour = UIwhite;
	consoleLbl->anchorBottom = 10;
	consoleLbl->anchorRight = 10;
	consoleLbl->multiLine = true;
	consoleLbl->TextFont = 0;
	GUIroot.Add(consoleLbl);

	pTimer = &Engine.Time;
}

void CBaseApp::updateWatches() {
	wLabel1->Text = watch::watch1.str();
	watch::watch1.str("");
	wLabel2->Text = watch::watch2.str();
	watch::watch2.str("");
	wLabel3->Text = watch::watch3.str();
	watch::watch3.str("");
	consoleLbl->Text = watch::con.str();
//	watch::con.str("");
	//TO DO: how to handle? messages to the console made every frame will quickly flood it
	//unless we clear it every frame. But occasional messages will vanish every frame if
	//we clear it every frame.
	//For now, treat console as permanent. Look into ways of getting multiline label to only
	//show the last h lines of text. Maybe the control keeps track of how many lines it 
	//can fit at its current font and height, and also how many lines are in its buffer.
	//May well need to create a more sophisticated control for that, with a more sophisticated buffer.
}



CBaseApp::~CBaseApp() {
	fflush(ErrStream);
	fclose(ErrStream);
	delete drawFuncs;
	watch::con.str(""); //Simple fix to ensure console memory freed.
}



