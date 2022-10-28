#pragma once

#include <string>
#include <functional>

#define GLFW_INCLUDE_NONE
#include <GLFW\glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW\glfw3native.h>

/** An interface for creating a window and receiving keyboard. mouse, etc, input. */
class CBaseApp;
class CWin {
public:
	CWin();
	~CWin();
	void createWindow(int w, int h, std::string title);
	void createWindowHidden(int w, int h, std::string title);
	void centreWindow();
	HWND getWindowsHandle();
	void swapBuffers();
	void setWindowSize(int w, int h);
	bool windowClosing();
	void pollEvents();
	void setCallbacks();
	void setApp(CBaseApp* app);
	template <typename T>
	void setReceiver(T& receiver ) {
		mouseBtnReceiver = [&](int button, int action, int mods) {
			receiver.onMouseButton(button, action, mods); };
		mouseMoveReceiver = [&](double x, double y) {
			receiver.onMouseMove(x,y); };
		enterWindowReceiver = [&](int enter) {
			receiver.onWinEnter(enter); };
		keyReceiver = [&](int key, int action, int mods) {
			receiver.onKey(key, action, mods); };

	}
	static void getMousePos(int& x, int& y);
	bool leftMouseHeldDown();
	bool rightMouseDown();
	bool middleMouseDown();
	void setMousePos(int x, int y);
	static void fullScreen();
	static void unFullScreen();
	void quitWindow();
	void hideWindow(bool onOff);
	void setTitle(std::string & title);
	static void showMouse(bool onOff);
	static bool keyPressed(int key);
	static bool mouseButtonPressed(int button);


	static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
	static void mouseButtonCallback(GLFWwindow * window, int button, int action, int mods);
	static void windowSizeCallback(GLFWwindow * window, int width, int height);
	static void keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);
	static void characterCallback(GLFWwindow * window, unsigned int codepoint);
	static void scrollCallback(GLFWwindow * window, double xoffset, double yoffset);
	static void cursorEnterCallback(GLFWwindow* window, int entered);


	static GLFWwindow* window;
	static GLFWmonitor* primaryMonitor;
	static const GLFWvidmode* lastMode;
	int lastWindowPosX, lastWindowPosY;
	int lastWindowWidth, lastWindowHeight;
	static CBaseApp* pApp;
	static bool fullScreenOn;

	static std::function <void(int,int,int)> mouseBtnReceiver;
	static std::function <void(double, double)> mouseMoveReceiver;
	static std::function <void(int, int, int)> keyReceiver;
	static std::function <void(int)> enterWindowReceiver;
};



