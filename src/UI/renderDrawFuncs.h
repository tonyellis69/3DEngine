#pragma once

#include <chrono>

#include "GUIbase.h"
#include "GUIbetterBase.h"


#include "..\renderer\renderer.h"

/** A subclass of CDrawFuncs, with direct access to the renderer.
TO DO: eventually this should be renamed (render)UIengine, and the old
UIengine removed altogether, as there's too much overlap of 
functionality. */

struct controlRects {
	CBuf* rect;
	CBuf* border;
	glm::mat4 posM; 
};

class CRenderDrawFuncs : public CDrawFuncs {
public:
	CRenderDrawFuncs();
	~CRenderDrawFuncs();
	void setRenderer(CRenderer* renderer);
	void loadShaders();
	void registerControl(CGUIbetterBase& control);
	void drawCtrlRect(CGUIbetterBase& control);
	void drawCtrlBorder(CGUIbetterBase& control);
	void setScreenSize(int width, int height);
	unsigned int getTextureHandle(const std::string& textureName);
	void drawTexture(CGUIbetterBase & control, CBaseTexture& texture);
	void setScreenDimensions(CGUIbetterBase & control);
	void updateScreenDimensions(CGUIbetterBase& control);
	void drawCursor(CGUIbetterBase& control,CBuf& cursorPos);
	float getTime();

//private:
	CRenderer* pRenderer;

	std::map<unsigned int, controlRects> quadBufs;

	CShader* uiRectShader;
	GLuint hOrtho; ///<Handle to  orthographic view matrix
	GLuint hColour1;
	GLuint hColour2;

	CShader* uiTexShader;
	GLuint hTexOrtho; ///<Handle to  orthographic view matrix
	GLuint hTextureUnit; ///<
	GLuint hTile;
	GLuint hOffset;

	std::vector<CShader*> shaderList; ///<Tracks ui shaders

	glm::mat4 orthoView; ///<A matrix for viewing 2D drawing;
	int screenWidth, screenHeight;

	bool cursorOn;
	chrono::system_clock::time_point lastCursorFlash;
};

const float cursorFlashDelay = 0.5f;