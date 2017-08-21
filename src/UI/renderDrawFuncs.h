#pragma once

#include "GUIbase.h"
#include "GUIbetterBase.h"
#include "uiRectShader.h"
#include "uiTexShader.h"

#include "..\renderer\renderer.h"

/** A subclass of CDrawFuncs, with direct access to the renderer.
TO DO: eventually this should be renamed (render)UIengine, and the old
UIengine removed altogether, as there's too much overlap of 
functionality. */

struct controlRects {
	CBuf* rect;
	CBuf* border;
};

class CRenderDrawFuncs : public CDrawFuncs {
public:
	CRenderDrawFuncs() {};
	~CRenderDrawFuncs();
	void setRenderer(CRenderer* renderer);
	void loadShaders();
	void registerControl(CGUIbetterBase& control);
	void drawCtrlRect(CGUIbetterBase& control);
	void drawCtrlBorder(CGUIbetterBase& control);
	void setScreenSize(int width, int height);
	unsigned int getTextureHandle(const std::string& textureName);
	void drawTexture(CGUIbetterBase & control, CBaseTexture& texture);

private:
	CRenderer* pRenderer;

	std::map<unsigned int, controlRects> quadBufs;

	CGUIrectShader* uiRectShader;
	CGUItexShader* uiTexShader;

	std::vector<CShader*> shaderList; ///<Tracks ui shaders

	glm::mat4 orthoView; ///<A matrix for viewing 2D drawing;
	int screenWidth, screenHeight;
};