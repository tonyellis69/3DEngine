#include "win.h"

#include "..\3DEngine\src\BaseApp.h"

#define GLFW_INCLUDE_NONE
#include <GLFW\glfw3.h>

#include <iostream>

CBaseApp* CWin::pApp = NULL;

void error_callback(int error, const char* description) {
	std::cerr << "Error: " << description;
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
}

void CWin::setApp(CBaseApp * app) {
	pApp = app;
}

void CWin::getMousePos(int & x, int & y) {
	double dx, dy;
	glfwGetCursorPos(window, &dx, &dy);
	x = floor(dx); y = floor(dy);
}

/** Poll cached left mouse button state. */
bool CWin::leftMouseDown() {
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
	glfwGetWindowPos(window, &lastWindowPosX, &lastWindowPosY);
	glfwGetWindowSize(window, &lastWindowWidth, &lastWindowHeight);
	lastMode = glfwGetVideoMode(primaryMonitor);
	glfwSetWindowMonitor(window, primaryMonitor, 0, 0, lastMode->width, lastMode->height, lastMode->refreshRate);
	fullScreenOn = true;
}

/** Return the window to its windowed state. */
void CWin::unFullScreen() {
	glfwSetWindowMonitor(window, NULL, lastWindowPosX, lastWindowPosY, lastWindowWidth, lastWindowHeight, lastMode->refreshRate);
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


void CWin::cursorPosCallback(GLFWwindow * window, double xpos, double ypos) {
	pApp->onMouseMove(xpos, ypos);
}

void CWin::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {



	pApp->onMouseButton(button, action, mods);
}

void CWin::windowSizeCallback(GLFWwindow* window, int width, int height) {
	pApp->onWinResize(width, height);
}

void CWin::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS)
		pApp->onKeyPress(key, mods);
	else
		pApp->OnKeyRelease(key, mods);
}

void CWin::characterCallback(GLFWwindow* window, unsigned int codepoint) {
	pApp->OnCharEntry(codepoint,0);
}

void CWin::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	pApp->OnMouseWheelMsg(xoffset,yoffset);
}
