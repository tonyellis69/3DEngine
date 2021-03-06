#pragma once

#include <chrono>

#include "GUIbase.h"



#include "..\renderer\renderer.h"

/** A subclass of CDrawFuncs, with direct access to the renderer.
TO DO: eventually this should be renamed (render)UIengine, and the old
UIengine removed altogether, as there's too much overlap of 
functionality. */

struct TControlRects {
	glm::mat4 ctrlMatrix = glm::mat4(1);
	glm::mat4 ctrlBorderMatrix = glm::mat4(1);
	glm::mat4 orthoTransMatrix = glm::mat4(1);
};

class CRenderDrawFuncs : public CDrawFuncs {
public:
	CRenderDrawFuncs();
	~CRenderDrawFuncs();
	void setRenderer(CRenderer* renderer);
	void loadShaders();
	void registerControl(CGUIbase& control);
	void deregisterControl(CGUIbase & control);
	void drawCtrlRect(CGUIbase& control);
	void drawCtrlBorder(CGUIbase& control);
	void setScreenSize(int width, int height);
	unsigned int getTextureHandle(const std::string& textureName);
	void drawTexture(guiRect& drawBox, CBaseTexture& texture);
	void setScreenDimensions(CGUIbase & control);
	void updateScreenDimensions(CGUIbase& control);
	void drawCursor(CGUIbase& control,CBuf& cursorPos);
	void drawCursor2(guiRect & drawBox);
	float getTime();
	CFont* getFont(std::string name);
	void drawTextureGradient(guiRect & drawBox, CBaseTexture& texture);
	void drawRect2(guiRect& drawBox, const glm::vec4& colour1, const glm::vec4&  colour2);
	void drawBorder2(guiRect& drawBox, const glm::vec4& colour);


//private:
	CRenderer* pRenderer;

	std::map<unsigned int, TControlRects> controlRects;

	CShader* uiRectShader;
	GLuint hOrtho; ///<Handle to  orthographic view matrix
	GLuint hColour1;
	GLuint hColour2;

	CShader* uiTexShader;
	GLuint hTexOrtho; ///<Handle to  orthographic view matrix
	GLuint hTextureUnit; ///<
	GLuint hTile;
	GLuint hOffset;

	CShader* uiTexGradientShader;
	GLuint hTexGradOrtho;
	GLuint hTextureUnitGrad;

	std::vector<CShader*> shaderList; ///<Tracks ui shaders

	glm::mat4 orthoView; ///<A matrix for viewing 2D drawing;
	int screenWidth, screenHeight;
	glm::mat4 lineOrthoView; ///<A matrix for viewing 2D lines offset by a half-pixel;

	bool cursorOn;
	std::chrono::system_clock::time_point lastCursorFlash;

	CBuf templateQuad;
	CBuf borderTemplateQuad;
	//CBuf cursorTemplateLine;
};

const float cursorFlashDelay = 0.5f;