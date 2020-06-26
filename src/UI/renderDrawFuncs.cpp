#include "renderDrawFuncs.h"
#include "..\vertBufs.h"

#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\matrix_access.hpp"

using namespace glm;
using namespace std;

CRenderDrawFuncs::CRenderDrawFuncs() {
	cursorOn = false;
	lastCursorFlash = chrono::system_clock::now();

	//create a template quad:
	vBuf::T2DtexVert corners[4];

	corners[0].v = vec2(0, 0); //A
	corners[1].v = vec2(1, 0); //B  1,0
	corners[2].v = vec2(0, 1); //C  0,1
	corners[3].v = vec2(1, 1); //D  1,1
	//NB the top-left origin used for 2D flips Y. By pre-flipping quads we ensure they end up facing the right way

	corners[0].tex = vec2(0);
	corners[1].tex = vec2(1, 0);
	corners[2].tex = vec2(0, 1);
	corners[3].tex = vec2(1, 1);

	unsigned int index[4] = { 2,3,0,1};
	templateQuad.storeVertexes(corners, sizeof(corners), 4);
	templateQuad.storeIndex(index, 4);
	templateQuad.storeLayout(2, 2, 0, 0);

	//create border template quad
	unsigned int index2[4] = { 1,0,2,3 };
	borderTemplateQuad.storeVertexes(corners, sizeof(corners), 4);
	borderTemplateQuad.storeIndex(index2, 4);
	borderTemplateQuad.storeLayout(2, 2, 0, 0);


}


void CRenderDrawFuncs::setRenderer(CRenderer * renderer) {
	pRenderer = renderer;
}

void CRenderDrawFuncs::loadShaders() {
	uiRectShader = pRenderer->createShader(pRenderer->dataPath + "uiRect");
	uiRectShader->setType(uiRect);
	hOrtho = uiRectShader->getUniformHandle("orthoMatrix");
	hColour1 = uiRectShader->getUniformHandle("colour1");
	hColour2 = uiRectShader->getUniformHandle("colour2");

	uiTexShader = pRenderer->createShader(pRenderer->dataPath + "uiTexture");
	uiTexShader->setType(uiTex);
	hTexOrtho = uiTexShader->getUniformHandle("orthoMatrix");
	hTextureUnit = uiTexShader->getUniformHandle("textureUnit");
	hTile = uiTexShader->getUniformHandle("tile");
	hOffset = uiTexShader->getUniformHandle("offset");

	uiTexGradientShader = pRenderer->createShader( "uiTextureGradient");
	uiTexGradientShader->setType(uiTex);
	hTexGradOrtho = uiTexGradientShader->getUniformHandle("orthoMatrix");
	hTextureUnitGrad = uiTexGradientShader->getUniformHandle("textureUnit");



}

/** Register this control with the uiEngine, storing its details for drawing. */
void CRenderDrawFuncs::registerControl(CGUIbase & control) {	
	controlRects[control.uniqueID].ctrlMatrix = glm::mat4(1);
	controlRects[control.uniqueID].ctrlBorderMatrix = glm::mat4(1);

	setScreenDimensions(control);
}

void CRenderDrawFuncs::deregisterControl(CGUIbase & control) {
	controlRects.erase(control.uniqueID);
}

void CRenderDrawFuncs::setScreenDimensions(CGUIbase & control) {
	//update matrices
	glm::mat4 shape = glm::scale(glm::mat4(1), glm::vec3(control.drawBox.size.x, (float)control.drawBox.size.y, 1));
	
	mat4 trans = glm::translate(mat4(1), vec3(control.drawBox.pos, 0) );

	controlRects[control.uniqueID].orthoTransMatrix = orthoView * trans;

	controlRects[control.uniqueID].ctrlMatrix = controlRects[control.uniqueID].orthoTransMatrix * shape;

	glm::mat4 borderShape = glm::scale(glm::mat4(1), glm::vec3(control.drawBox.size.x - 0.5f, (float)control.drawBox.size.y - 1, 1));

	mat4 borderTrans = glm::translate(mat4(1), vec3(control.drawBox.pos, 0) + vec3(0.5f,0.5,0));
	controlRects[control.uniqueID].ctrlBorderMatrix = orthoView * borderTrans * borderShape;

}

/** Draw the drawBox rectangle of this control. */
void CRenderDrawFuncs::drawCtrlRect(CGUIbase & control) {
	pRenderer->setShader(uiRectShader);
	uiRectShader->setShaderValue(hColour1,(vec4&)control.backColour1);
	uiRectShader->setShaderValue(hColour2,(vec4&)control.backColour2);
	uiRectShader->setShaderValue(hOrtho, controlRects[control.uniqueID].ctrlMatrix);
	pRenderer->drawBuf(templateQuad, drawTriStrip);

	pRenderer->setShader(0); //TO DO: seems unnecessary
}

void CRenderDrawFuncs::drawCtrlBorder(CGUIbase & control) {
	pRenderer->setShader(uiRectShader);
	uiRectShader->setShaderValue(hColour1, (vec4&)control.borderColour);
	uiRectShader->setShaderValue(hColour2, (vec4&)control.borderColour);
	uiRectShader->setShaderValue(hOrtho, controlRects[control.uniqueID].ctrlBorderMatrix);
	pRenderer->drawBuf(borderTemplateQuad, drawLineLoop);

	pRenderer->setShader(0);
}

void CRenderDrawFuncs::setScreenSize(int width, int height) {
	//mat4 flip;
	//flip[1].y = -1;
	//flip[2].y = float(height);
	//orthoView = flip * glm::ortho<float>(0.0f, (float)width, 0.0f, (float)height);
	orthoView =  glm::ortho<float>(0.0f, (float)width,  (float)height, 0.0f);
	screenWidth = width; screenHeight = height;
}

unsigned int CRenderDrawFuncs::getTextureHandle(const std::string & textureName) {
	CRenderTexture* renTex = (CRenderTexture*)pRenderer->textureManager.getTexture(textureName);
	return renTex->handle;
}

void CRenderDrawFuncs::drawTexture(guiRect& drawBox, CBaseTexture& texture) {
	mat4 trans = glm::translate(mat4(1), vec3(drawBox.pos, 0) + vec3(-0.0,-0.0,0) );
	mat4 shape = glm::scale(trans, glm::vec3(drawBox.size.x,drawBox.size.y, 1));
	mat4 ctrlMatrix = orthoView * shape;


	pRenderer->setShader(uiTexShader);
	CRenderTexture* rendTex = (CRenderTexture*)&texture;
	pRenderer->attachTexture(0, rendTex->handle);
	uiTexShader->setTextureUnit(hTextureUnit, 0);
	uiTexShader->setShaderValue(hTexOrtho, ctrlMatrix);
	pRenderer->drawBuf(templateQuad, drawTriStrip);
}

/** Update the vertexes defining the dimensions of this control. */
void CRenderDrawFuncs::updateScreenDimensions(CGUIbase & control) {
	setScreenDimensions(control);
}

/** Assuming this is an interactive text control, draw its cursor. */
void CRenderDrawFuncs::drawCursor(CGUIbase& control,CBuf& cursorPos) {
	chrono::system_clock::time_point currentTime = chrono::system_clock::now();
	chrono::duration<float> time_span = chrono::duration_cast<chrono::duration<float>>(currentTime - lastCursorFlash);
	if (time_span.count() > cursorFlashDelay) {
		cursorOn = !cursorOn;
		lastCursorFlash = currentTime;
	}
	if (!cursorOn)
		return;
	pRenderer->setShader(uiRectShader);
	uiRectShader->setShaderValue(hColour1, (vec4&)UIblack); //TO DO: pass colour choice as parameter
	uiRectShader->setShaderValue(hColour2, (vec4&)UIblack);
	uiRectShader->setShaderValue(hOrtho, controlRects[control.uniqueID].ctrlMatrix);
	pRenderer->drawBuf(cursorPos, drawLineLoop);
	pRenderer->setShader(0);
}

void CRenderDrawFuncs::drawCursor2(guiRect& drawBox) { 
	chrono::system_clock::time_point currentTime = chrono::system_clock::now();
	chrono::duration<float> time_span = chrono::duration_cast<chrono::duration<float>>(currentTime - lastCursorFlash);
	if (time_span.count() > cursorFlashDelay) {
		cursorOn = !cursorOn;
		lastCursorFlash = currentTime;
	}
	if (!cursorOn)
		return;

	drawRect2(drawBox, (vec4&)UIblack, (vec4&)UIblack);
}



float CRenderDrawFuncs::getTime() {
	time_t currentTime = time(NULL);
	return 0.0f;
}

CFont * CRenderDrawFuncs::getFont(std::string name) {
	//get here????????
	return &pRenderer->fontManager.getFont(name);
}

void CRenderDrawFuncs::drawTextureGradient(guiRect & drawBox, CBaseTexture & texture) {
	pRenderer->setShader(uiTexGradientShader);
	//pass the texture
	CRenderTexture* rendTex = (CRenderTexture*)&texture;
	pRenderer->attachTexture1D(0, rendTex->handle);

	mat4 trans = glm::translate(mat4(1), vec3(drawBox.pos, 0));
	mat4 shape = glm::scale(trans, glm::vec3(drawBox.size.x, drawBox.size.y, 1));
	mat4 ctrlMatrix = orthoView * shape;

	uiTexGradientShader->setTextureUnit(hTextureUnitGrad, 0);
	uiTexGradientShader->setShaderValue(hTexGradOrtho, ctrlMatrix);
	pRenderer->drawBuf(templateQuad, drawTriStrip);
}

void CRenderDrawFuncs::drawRect2(guiRect& drawBox, const glm::vec4& colour1,const  glm::vec4&  colour2) {
	mat4 trans = glm::translate(mat4(1), vec3(drawBox.pos, 0));
	mat4 shape = glm::scale(trans, glm::vec3(drawBox.size.x, drawBox.size.y, 1));
	mat4 ctrlMatrix = orthoView *  shape;

	pRenderer->setShader(uiRectShader);
	uiRectShader->setShaderValue(hColour1, colour1);
	uiRectShader->setShaderValue(hColour2, colour2);
	uiRectShader->setShaderValue(hOrtho, ctrlMatrix);
	pRenderer->drawBuf(templateQuad, drawTriStrip);
}

void CRenderDrawFuncs::drawBorder2(guiRect & drawBox, const glm::vec4 & colour) {
	mat4 borderTrans = glm::translate(mat4(1), vec3(drawBox.pos, 0) + vec3(0.5f, 0.5, 0));
	glm::mat4 borderShape = glm::scale(borderTrans, glm::vec3(drawBox.size.x - 0.5f, (float)drawBox.size.y - 1, 1));
	mat4 borderCtrlMatrix = orthoView *  borderShape;

	pRenderer->setShader(uiRectShader);
	uiRectShader->setShaderValue(hColour1, colour);
	uiRectShader->setShaderValue(hColour2, colour);
	uiRectShader->setShaderValue(hOrtho, borderCtrlMatrix);
	pRenderer->drawBuf(borderTemplateQuad, drawLineLoop);
}



CRenderDrawFuncs::~CRenderDrawFuncs() {
	for (size_t s = 0; s < shaderList.size(); s++) {
		delete shaderList[s];
	}
}

