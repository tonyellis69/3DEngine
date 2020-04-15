
#include "BaseApp.h"
#include <iostream> //for cerr

#include <glm/glm.hpp>

//#include "watch.h"

#include "utils/log.h"

//using namespace watch;

CFont* style::defaultFont;

CBaseApp::CBaseApp(void) : renderer(CRenderer::getInstance()) {
	homeDir = getExePath();
#ifdef _DEBUG
	homeDir += "..\\";
#endif
	string logpath = homeDir + "ErrorLog.txt";
	freopen_s(&ErrStream,logpath.c_str(),"w",stderr);

	logWindow = NULL;
	sysLog.setFile(homeDir + "sysLog.txt");
	fatalLog.setCallback(this);
	liveLog.setCallback(this);
	sysLog << startMsg << "\nBaseApp constructor starting...";
	

	Engine.dataPath = homeDir + "Data\\";
	dataPath = homeDir + "Data\\";
	Engine.Renderer.dataPath = homeDir + "Data\\";
	
	//Set up our handlers for window events
	//RegisterHandlers();

	UIeng.pEngine = &Engine;

	Time = LastTime = 0;
	mouseX = mouseY = 0;

	BuiltInFullScrn = true;
	Paused = false;
	
	//Set up engine for rendering
	win.setApp(this);

	win.createWindowHidden(800, 600, "Base window");
	win.setCallbacks();
	
	//Engine.init();
	renderer.init();
	

	loadSystemFonts();
	GUIroot.setDefaultFont(smallSysFont); //TO DO: stylesheet will replace this
	initialiseSystemStylesheet();
//	GUIroot.setStyleSheet(&sysStyleSheet);
	GUIroot.setMessageObject(this); //Makes this the default for all gui messages

	RegisterUIfunctors();

	
	quitOnEsc = true;
	//initWatches();
	//pTimer = &Engine.Time; //last vestige of watch.h - get rid of!
	initLogWindow();

	//TO DO: temp
	//logWindow->setVisible(true);

	vm.setApp(this);

	sysLog <<  "BaseApp constructor finished. Yay!\n";

}



/** Load the default fonts, currently stored as static data. */
void CBaseApp::loadSystemFonts() {
	std::istringstream ss;
	std::string tmp((char*)SysFnt + 2, 33600);
	ss.str(tmp);
	sysFont = &renderer.fontManager.createFromStream("sysFont",ss);

	ss.clear();
	tmp.clear();
	tmp = std::string((char*)SmallSysFnt + 2, 17728);
	ss.str(tmp);
	smallSysFont = &renderer.fontManager.createFromStream("smallSysFont",ss);

}


/** Starts the application loop. This will not return until the app is shut down. */
void CBaseApp::start() {
	
	LastTime = Engine.Time.seconds();
	onStart();

	win.hideWindow(false);
	//toggleLogWindow();
	while (!win.windowClosing()) {
		AppTasks();
	}
}


void CBaseApp::SetWindow( int width, int height, std::string title) {
	win.setWindowSize(width, height);
	win.setTitle(title);
	win.centreWindow();
	onWinResize(width, height);
}

/** Handler for when our window is rezized. */
void CBaseApp::onWinResize( int w, int h) {
	if (w == 0 || h == 0)
		return; //panic!
	viewWidth = w; viewHeight = h;
	renderer.set2DView(0, 0, w, h); //We need to change shape of view.
	renderer.currentCamera->setAspectRatio((float)w, (float)h);
	drawFuncs->setScreenSize(w, h);
	GUIroot.setLocalDimensions(0,0,w,h); //Resize the base UI control so it's always the size of the view.
	onResize(w,h); //Call the user's resize handler, in case they want to do something;
}

/** Event handler for when the OS messages that a key has been pressed down. */
void CBaseApp::onKeyPress(int key, long mod) {
	//KeyDown[key] = true; //TO DO 'some lag with this' - really? Check.
	if (key == GLFW_KEY_TAB && BuiltInFullScrn) {
		if (!win.fullScreenOn)
			 win.fullScreen();
		else
			 win.unFullScreen();
		return;
	}
	if (key == GLFW_KEY_ESCAPE && quitOnEsc){
		//win.quitWindow();
		PostQuitMessage(0);
		return;
	}	
	if (key == GLFW_KEY_GRAVE_ACCENT) {
		toggleLogWindow();
		return;
	}


	//send the key to the UI
	GUIroot.KeyboardMsg(key, mod);
	onKeyDown(key,mod); //Call the user's key handler - useful when we don't want feedback every frame
}

/** Event handler for when a key is released. */
void CBaseApp::OnKeyRelease(unsigned int Key, long Mod) {
	//KeyDown[Key] = false;
}

/** Event handler for when a key is pressed and we want to handle it as a character. Eg, textbox entry. */
void CBaseApp::OnCharEntry(unsigned int Key, long Mod) {
	GUIroot.CharEntryMsg(Key, Mod);
}


/** Called when the mouse moves within the area of the app - *not* 
	simply anywhere on screen. */
void CBaseApp::onWinMouseMove(int x, int y) {
	unsigned int msg = WM_MOUSEMOVE;
	int key = 0;
	if (win.leftMouseHeldDown())
		key += MK_LBUTTON;
	if (win.rightMouseDown())
		key += MK_RBUTTON;
	if (win.middleMouseDown())
		key += MK_MBUTTON;
	mouseKey = key;
	GUIroot.MouseMsg(msg, x, y, key);
	mouseMove(x, y, key);

}

void CBaseApp::onWinMouseButton(int button, int action, int mods) {
	unsigned int msg = 0; 
	glm::i32vec2 mousePos = getMousePos();

	if (action == GLFW_RELEASE) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			msg = WM_LBUTTONUP;
			static auto before = Time; // std::chrono::system_clock::now();
			auto now = Time;
			double diff_ms = now - before;
			before = now;
			if (diff_ms > 0.01 && diff_ms < 0.4) {
				msg = MY_DOUBLECLICK;
			}

		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT)
			msg = WM_RBUTTONUP;
	}
	else {
		if (button == GLFW_MOUSE_BUTTON_LEFT)
			msg = WM_LBUTTONDOWN;
		if (button == GLFW_MOUSE_BUTTON_RIGHT)
			msg = WM_RBUTTONDOWN;
	}
	//////////GUIroot.MouseMsg(msg, mousePos.x, mousePos.y, mods);
	GUIroot.MouseMsg(msg, mouseX, mouseY, mods);
	onMouseButton(button, action, mods);
}

/** Event handler for mouse wheel messages. */
void CBaseApp::OnMouseWheelMsg(float xoffset, float yoffset) {
//	getMousePos(x, y); //gives negative y for some reason!
	int delta = int(yoffset);
	int keyState = 0; //can get key state if ever needed
	if (GUIroot.IsOnControl(GUIroot,mouseX, mouseY)) //checks that the mouse isn't outside our app altogether
		GUIroot.MouseWheelMsg(mouseX, mouseY,delta,keyState);
}


void CBaseApp::AppTasks() {

	if (Engine.MakingFit) //activate the global scaling matrix, if we're using one.
		Engine.applyGlobalScale(); //TO DO: messes GUI - look into or scrap - prob due to line-drawing methods

	Engine.clearFrame();

	vm.execute();

	Time = Engine.Time.seconds();
	dT = Time - LastTime;
	Engine.dT = dT;
	LastTime = Time;
	//getMousePos(mouseX, mouseY);
	win.getMousePos(mouseX, mouseY);
	keyCheck(); //happens every frame, therefore responsive

	//TO DO: mouseCheck, which calls user with last reported mouse positionHint.

	if (!Paused) {
		//Engine.updateRegisteredSprites(dT);
		Update();
	}
	

	if (Engine.skyDome) {
		drawSkyDome();
	}


	


	Engine.physObjManager.update(float(dT * 10.0f));


	//Engine.applyUserScale(); //Scale matrix in case scene or other user drawing is zoomed

	Engine.drawModels();
	
	

//	Engine.drawSceneLayers();
	//Engine.drawRegisteredSprites();
	draw(); //Do user drawing

	//aftyer here
	

	//Engine.removeUserScale();

	logWindow->update((float)dT);

	DrawUI(); 

	//Engine.showFrame();
	win.swapBuffers();
	win.pollEvents();


}


/** Draw the UI system. */
void CBaseApp::DrawUI() {
	Engine.setCurrentShader(0);
	Engine.setDrawColour(engineWhite);
	GUIroot.Draw();
	Engine.setClip(0,0,GUIroot.drawBox.size.x, GUIroot.drawBox.size.y);
}

/** Assign engine functions to the UI so it can draw itself, etc. */
void CBaseApp::RegisterUIfunctors() {	
	//drawFuncs = new CDrawFuncs;
	//TO DO: rename this when it's been updated
	drawFuncs = new CRenderDrawFuncs();
	drawFuncs->setRenderer(&Engine.Renderer);
	drawFuncs->loadShaders();


//	drawFuncs->setDrawColours.Set(&UIeng, &CGUIengine::setDrawColours);
//	drawFuncs->drawIcon.Set(&UIeng, &CGUIengine::drawIcon);
	drawFuncs->handleUImsg.Set(this, &CBaseApp::HandleUImsg);
	//drawFuncs->drawRect.Set(&UIeng, &CGUIengine::drawRect);
//	drawFuncs->drawBorder.Set(&UIeng, &CGUIengine::DrawBorder);
	drawFuncs->setClip.Set(&UIeng, &CGUIengine::setClip);
//	drawFuncs->drawLine.Set(&UIeng, &CGUIengine::drawLine);
//	drawFuncs->setIconset.Set(&UIeng, &CGUIengine::setIconset);
//	drawFuncs->drawTile.Set(&UIeng, &CGUIengine::drawTile);
//	drawFuncs->drawImage.Set(&UIeng, &CGUIengine::drawImage);
	drawFuncs->mouseCaptured.Set(this, &CBaseApp::mouseCaptured);
//	drawFuncs->setCursor.Set(&UIeng, &CGUIengine::setCursor);
//	drawFuncs->setDrawColoursConditional.Set(&UIeng, &CGUIengine::setDrawColoursConditional);
//	drawFuncs->drawDottedRect.Set(&UIeng,&CGUIengine::drawDottedRect);
//	drawFuncs->setScale.Set(&UIeng,&CGUIengine::setScale);

	GUIroot.setDrawFuncs(drawFuncs);
}

void CBaseApp::drawSkyDome() {
	Engine.Renderer.setDepthTest(false);

	CCamera* currentCamera = Engine.getCurrentCamera();
	Engine.skyDome->dome->setPos(currentCamera->getPos() + glm::vec3(0, -0.5f, 0));
	glm::mat4 mvp = currentCamera->clipMatrix * Engine.skyDome->dome->worldMatrix;
	Engine.Renderer.setShader(Engine.skyDome->skyShader);
	Engine.skyDome->skyShader->setShaderValue(Engine.skyDome->hSkyDomeMVP,mvp);
	Engine.Renderer.setShaderValue(Engine.skyDome->hSkyDomeHeightColours, 4, (glm::vec3&)Engine.skyDome->heightColours);
	Engine.skyDome->dome->drawNew();

	Engine.skyDome->sunBoard->setPos(currentCamera->getPos() + glm::vec3(0,200, -400));
	mvp = currentCamera->clipMatrix * Engine.skyDome->sunBoard->worldMatrix;
	Engine.Renderer.setShader(Engine.Renderer.billboardShader);
	Engine.Renderer.billboardShader->setShaderValue(Engine.Renderer.hBillMVP,mvp);
	Engine.Renderer.billboardShader->setShaderValue(Engine.Renderer.hBillCamWorldMatrix, Engine.Renderer.currentCamera->worldMatrix);

	Engine.skyDome->sunBoard->drawNew();




	//draw cloud plane
	Engine.skyDome->plane->setPos(currentCamera->getPos() + glm::vec3(0,-400.0f, 0));
	Engine.Renderer.setShader(Engine.Renderer.multiTexShader);
	mvp = currentCamera->clipMatrix * Engine.skyDome->plane->worldMatrix;
	Engine.Renderer.multiTexShader->setShaderValue(Engine.Renderer.hMultMVP,mvp);
	Engine.skyDome->plane->drawNew();

	Engine.Renderer.setDepthTest(true);
}








/** Ensures we still get mouse input when the mouse is outside the app window. */
void CBaseApp::mouseCaptured(bool capture) {
	//Window.mouseCaptured(capture);
	if (capture)
		SetCapture(win.getWindowsHandle());
	else
		ReleaseCapture();
}


/** Turns mouse pointer on or off.*/
void CBaseApp::showMouse(bool onOff) {
	//Window.showMouse(on);
	win.showMouse(onOff);
}

void CBaseApp::setMousePos(int x, int y) {
	//Window.setMousePos(x,y);
	win.setMousePos(x, y);
}

/** Return mouse position relative to the app window, */
glm::i32vec2 CBaseApp::getMousePos() {
	int x, y;
	win.getMousePos(x, y);
	return glm::i32vec2(x, y);
}

bool CBaseApp::keyNow(int vKey) {
	return win.keyPressed( vKey);
}

bool CBaseApp::mouseButtonNow(int button) {
	return win.mouseButtonPressed(button);
}

/** Shut down the entire app and exit. */
void CBaseApp::exit() {
	PostQuitMessage(0);
}






void CBaseApp::initLogWindow() {
	logWindow = new CGUIrichText(0, 0, 400, 150);
	logWindow->anchorRight = 10;
	logWindow->anchorBottom = 10;
	logWindow->setVisible(false);
	logWindow->setBorderOn(true);
	logWindow->setFont(smallSysFont);


	logWindow->resizeMode = resizeNone;
	//sysLog.setCallback(this);

	GUIroot.Add(logWindow);


}


/** Sends data from the liveLog stream to a console window. If the window hasn't been
	created yet, send it to the sysLog (which can write it to a file.) */
void CBaseApp::logCallback(std::stringstream & logStream) {
	if (logWindow)
		 logWindow->appendMarkedUpText(logStream.str());
	else
		sysLog << logStream.str();
}

/** Handle a log alert. Make some noise! */
void CBaseApp::logAlertCallback() {
	if (logWindow)
		logWindow->setVisible(true);
	//TO DO: consider making  log window bigger, too.
}

/** Handle a fatal log entry. This typically means exiting. */
void CBaseApp::logFatalCallback(std::stringstream& logStream) {
	if (logWindow) {
		logWindow->setVisible(true);
		logWindow->appendMarkedUpText(logStream.str());
	}
	sysLog << logStream.str();
	sysLog.outFile.flush();
	#ifdef _DEBUG
		//pop up dialog box or something.
	int b = 0;
	#else
		exit();
	#endif
}

/** Returns the file path of the current program. */
string CBaseApp::getExePath() {
	char path[512];
	GetModuleFileName(NULL,(char*)path,512);
	string tmp(path);
	int n  = tmp.find_last_of(('\\'));
	tmp = tmp.substr( 0, n+1 );
	return tmp;
}

/** Stock the built-in stylesheet with default, failsafe fonts and other settings so that
	the GUI will always work out of the box. */
void CBaseApp::initialiseSystemStylesheet() {
	//TO DO: ultimately it might be best to load fonts into font manager here too, 
	//scrap the systemFont* member. More streamlined, less duplication
	//sysStyleSheet.defaultFont = &renderer.fontManager.getFont("smallSysFont");

	style::defaultFont = &renderer.fontManager.getFont("smallSysFont");

	//sysStyleSheet.defaultFontColour = style::uiBlack;

	//kludigly create default text styles for rich text controls
	//GUIroot.themeServer.addToTheme("default", (TtextStyle&)sysStyleSheet.richtextStyle);
	//GUIroot.themeServer.addToTheme("default", (TtextStyle&)sysStyleSheet.richtextHotStyle);
	//GUIroot.themeServer.addToTheme("default", (TtextStyle&)sysStyleSheet.richtextSelectedStyle);

}

/** Use the Windows Common Dialog to get a filename. */
string CBaseApp::getFilenameDlg(const std::string&  title, const char* path) {
	const int BUFSIZE = 1024;
	CHAR buffer[BUFSIZE] = { 0 };
	OPENFILENAME ofns = { 0 };
	ofns.lStructSize = sizeof(ofns);
	ofns.lpstrFile = buffer;
	ofns.nMaxFile = BUFSIZE;
	ofns.lpstrTitle = (title.c_str());
	ofns.lpstrInitialDir = path;
	GetOpenFileName(&ofns);
	return buffer;
}

string CBaseApp::saveFilenameDlg(const std::string&  title, const char* path) {
	const int BUFSIZE = 1024;
	CHAR buffer[BUFSIZE] = { 0 };
	OPENFILENAME ofns = { 0 };
	ofns.lStructSize = sizeof(ofns);
	ofns.lpstrFile = buffer;
	ofns.nMaxFile = BUFSIZE;
	ofns.lpstrTitle = (title.c_str());
	ofns.lpstrInitialDir = path;
	GetSaveFileName(&ofns);
	return buffer;
}

/** Toggle visibility of the logWindow. Take the opportunity to put it to the top of the 
	draw list. TO DO: find a better way of having controls always on top. */
void CBaseApp::toggleLogWindow() {
	if (logWindow->getVisible() == false) {
		auto ctrl = std::find(GUIroot.controls.begin(), GUIroot.controls.end(), logWindow);
		if (*ctrl != GUIroot.controls.back()) {
			std::rotate(ctrl, ctrl + 1, GUIroot.controls.end());
		}
		logWindow->setVisible(true);
	}
	else
		logWindow->setVisible(false);

}



CBaseApp::~CBaseApp() {
	fflush(ErrStream);
	fclose(ErrStream);
	delete drawFuncs;
	GUIroot.setDrawFuncs(NULL);
}



