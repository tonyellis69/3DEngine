#pragma once

#include "GUIbase.h"
#include "GUIbetterBase.h"
#include "uiRectShader.h"

#include "..\renderer\renderer.h"

/** A subclass of CDrawFuncs, with direct access to the renderer.
TO DO: eventually this should be renamed (render)UIengine, and the old
UIengine removed altogether, as there's too much overlap of 
functionality. */


class CRenderDrawFuncs : public CDrawFuncs {
public:
	CRenderDrawFuncs() {};
	~CRenderDrawFuncs();
	void setRenderer(CRenderer* renderer);
	void loadShaders();
	void registerControl(CGUIbetterBase& control);
	void drawCtrlRect(CGUIbetterBase& control);
	void setScreenSize(int width, int height);

private:
	CRenderer* pRenderer;

	std::map<unsigned int, CBuf> quadBufs;

	CGUIrectShader* uiRectShader;

	std::vector<CShader*> shaderList; ///<Tracks ui shaders

	glm::mat4 orthoView; ///<A matrix for viewing 2D drawing;
	int screenWidth, screenHeight;
};