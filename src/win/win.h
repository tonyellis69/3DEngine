#pragma once

#include <string>

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
	void getMousePos(int& x, int& y);
	bool leftMouseHeldDown();
	bool rightMouseDown();
	bool middleMouseDown();
	void setMousePos(int x, int y);
	void fullScreen();
	void unFullScreen();
	void quitWindow();
	void hideWindow(bool onOff);
	void setTitle(std::string & title);
	void showMouse(bool onOff);
	bool keyPressed(int key);
	bool mouseButtonPressed(int button);


	static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
	static void mouseButtonCallback(GLFWwindow * window, int button, int action, int mods);
	static void windowSizeCallback(GLFWwindow * window, int width, int height);
	static void keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods);
	static void characterCallback(GLFWwindow * window, unsigned int codepoint);
	static void scrollCallback(GLFWwindow * window, double xoffset, double yoffset);
	static void cursorEnterCallback(GLFWwindow* window, int entered);


	GLFWwindow* window;
	GLFWmonitor* primaryMonitor;
	const GLFWvidmode* lastMode;
	int lastWindowPosX, lastWindowPosY;
	int lastWindowWidth, lastWindowHeight;
	static CBaseApp* pApp;
	bool fullScreenOn;
};

