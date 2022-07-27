#include "win.h"

#include "..\3DEngine\src\BaseApp.h"

#define GLFW_INCLUDE_NONE
#include <GLFW\glfw3.h>

#include <iostream>

#include "listen/listen.h"

CBaseApp* CWin::pApp = NULL;
GLFWwindow* CWin::window;
std::function <void(int, int, int)> CWin::mouseBtnReceiver;
std::function <void(double, double)> CWin::mouseMoveReceiver;
std::function <void(int, int, int)> CWin::keyReceiver;
std::function <void(int)> CWin::enterWindowReceiver;

void error_callback(int error, const char* description) {
	std::cerr << "Error here: " << description;
}


CWin::CWin() {
	glfwSetErrorCallback(error_callback);
	glfwInit();
	primaryMonitor = glfwGetPrimaryMonitor();
	fullScreenOn = false;
}

CWin::~CWin() {
	glfwTerminate();
}

/** Create a window for rendering to. */
void CWin::createWindow(int w, int h, std::string title) {
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT , true);
	glfwWindowHint(GLFW_SAMPLES, 4);
	window = glfwCreateWindow(w, h, title.c_str(), NULL, NULL);
	glfwMakeContextCurrent(window);


}

void CWin::createWindowHidden(int w, int h, std::string title) {
	glfwWindowHint(GLFW_VISIBLE, false);
	createWindow(w, h, title);
}

void CWin::centreWindow() {
	glfwGetWindowSize(window, &lastWindowWidth, &lastWindowHeight);
	lastMode = glfwGetVideoMode(primaryMonitor);
	glfwSetWindowPos(window, (lastMode->width - lastWindowWidth) / 2, (lastMode->height - lastWindowHeight) / 2);
}

HWND CWin::getWindowsHandle() {
	return glfwGetWin32Window(window);
	
}

void CWin::swapBuffers() {
	glfwSwapBuffers(window);
}

/** Set the client area size. */
void CWin::setWindowSize(int w, int h) {
	glfwSetWindowSize(window, w, h);
}

bool CWin::windowClosing() {
	return glfwWindowShouldClose(window);
}

void CWin::pollEvents() {
	glfwPollEvents();
}

void CWin::setCallbacks() {
	glfwSetCursorPosCallback(window, cursorPosCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetWindowSizeCallback(window, windowSizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCharCallback(window, characterCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetCursorEnterCallback(window, cursorEnterCallback);
}

void CWin::setApp(CBaseApp * app) {
	pApp = app;
}

void CWin::getMousePos(int & x, int & y) {
	double dx, dy;
	glfwGetCursorPos(window, &dx, &dy);
	x = floor(dx); y = floor(dy);
	//pApp->mouseMove(x,y,0);
}



/** Poll cached left mouse button state. */
bool CWin::leftMouseHeldDown() {
	return glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
}

/** Poll cached right mouse button state. */
bool CWin::rightMouseDown() {
	return glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
}

/** Poll cached middle mouse button state. */
bool CWin::middleMouseDown() {
	return glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
}


void CWin::setMousePos(int x, int y) {
	glfwSetCursorPos(window, x, y);
}

/** Make the window fullscreen, borderless. */
void CWin::fullScreen() {
	//glfwGetWindowPos(window, &lastWindowPosX, &lastWindowPosY);
	//glfwGetWindowSize(window, &lastWindowWidth, &lastWindowHeight);
	//lastMode = glfwGetVideoMode(primaryMonitor);
	//glfwSetWindowMonitor(window, primaryMonitor, 0, 0, lastMode->width, lastMode->height, lastMode->refreshRate);
	glfwMaximizeWindow(window);
	fullScreenOn = true;
}

/** Return the window to its windowed state. */
void CWin::unFullScreen() {
	//glfwSetWindowMonitor(window, NULL, lastWindowPosX, lastWindowPosY, lastWindowWidth, lastWindowHeight, lastMode->refreshRate);
	glfwRestoreWindow(window);
	fullScreenOn = false; 
}

void CWin::quitWindow() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

/** Toggle window visibility. */
void CWin::hideWindow(bool onOff) {
	if (onOff)
		glfwHideWindow(window);
	else
		glfwShowWindow(window);
}

void CWin::setTitle(std::string& title) {
	glfwSetWindowTitle(window, title.c_str());
}

void CWin::showMouse(bool onOff) {
	if (onOff)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
}

bool CWin::keyPressed(int key) {
	return glfwGetKey(window, key);
}

bool CWin::mouseButtonPressed(int button) {
	return glfwGetMouseButton(window, button);
}

/** NB: only seems to trigger if mouse moves inside window. */
void CWin::cursorPosCallback(GLFWwindow * window, double xpos, double ypos) {
	pApp->onWinMouseMove(xpos, ypos);
	mouseMoveReceiver(xpos, ypos);
}

void CWin::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
	pApp->onWinMouseButton(button, action, mods);

	mouseBtnReceiver(button, action, mods);
}

void CWin::windowSizeCallback(GLFWwindow* window, int width, int height) {
	pApp->onWinResize(width, height);
}

void CWin::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS)
		pApp->onKeyPress(key, mods);
	if (action == GLFW_RELEASE)
		pApp->OnKeyRelease(key, mods);
	keyReceiver(key, action, mods);
}

void CWin::characterCallback(GLFWwindow* window, unsigned int codepoint) {
	pApp->OnCharEntry(codepoint,0);
}

void CWin::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	pApp->OnMouseWheelMsg(xoffset,yoffset);
}

void CWin::cursorEnterCallback(GLFWwindow* window, int entered) {
	//CEvent e;
	//if (entered)
	//	e.type == eMouseEnterWindow;
	//else
	//	e.type = eMouseExitWindow;
	//lis::event(e);
	enterWindowReceiver(entered);
}


