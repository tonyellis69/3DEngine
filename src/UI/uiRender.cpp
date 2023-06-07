#include "uiRender.h"

#include "glm/glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

//#include "UI/GUIbase.h"

#include "renderer/renderer.h"
#include "buf2.h"

CBuf2 templateQuad;
CBuf2 borderTemplateQuad;

CShaderOld* uiRectShader;
CShaderOld* uiTexShader;
CShaderOld* uiTexGradientShader;

glm::mat4 orthoView;

void initialiseTemplateQuads();
void loadShaders();

unsigned int hOrtho, hColour1, hColour2, hTextureUnit, hTexOrtho, hTile, hOffset,
		hTexGradOrtho, hTextureUnitGrad;

/** Initialise the library for drawing to a screen of
	the current size. */
void uiDraw::onSpawn() {
	if (templateQuad.numElements == 0) {//only need to do this once
		initialiseTemplateQuads();
		loadShaders();
	}
	orthoView = glm::ortho<float>(0.0f, (float)renderer.Width, (float)renderer.Height, 0.0f);
}

void uiDraw::drawRect(guiRect& drawBox, const glm::vec4& colour1, const glm::vec4& colour2) {
	glm::mat4 trans = glm::translate(glm::mat4(1), glm::vec3(drawBox.pos, 0));
	glm::mat4 shape = glm::scale(trans, glm::vec3(drawBox.size.x, drawBox.size.y, 1));
	glm::mat4 ctrlMatrix = orthoView * shape;

	renderer.setShader(uiRectShader);

	uiRectShader->setShaderValue(hColour1, colour1);
	uiRectShader->setShaderValue(hColour2, colour2);
	uiRectShader->setShaderValue(hOrtho, ctrlMatrix);

	renderer.drawTriStripBuf(templateQuad);
}

void uiDraw::drawBorder(guiRect& drawBox, const glm::vec4& colour) {

	glm::mat4 borderTrans = glm::translate(glm::mat4(1), glm::vec3(drawBox.pos, 0) + glm::vec3(0.5f, 0.5, 0));
	glm::mat4 borderShape = glm::scale(borderTrans, glm::vec3(drawBox.size.x - 0.5f, (float)drawBox.size.y - 1, 1));
	glm::mat4 borderCtrlMatrix = orthoView * borderShape;

	renderer.setShader(uiRectShader);
	uiRectShader->setShaderValue(hColour1, colour);
	uiRectShader->setShaderValue(hColour2, colour);
	uiRectShader->setShaderValue(hOrtho, borderCtrlMatrix);

	renderer.drawLineLoopBuf(borderTemplateQuad);
}

void uiDraw::drawTexture(guiRect& drawBox, CRenderTexture& texture) {
	glm::mat4 trans = glm::translate(glm::mat4(1), glm::vec3(drawBox.pos, 0) + glm::vec3(-0.0, -0.0, 0));
	glm::mat4 shape = glm::scale(trans, glm::vec3(drawBox.size.x, drawBox.size.y, 1));
	glm::mat4 ctrlMatrix = orthoView * shape;


	renderer.setShader(uiTexShader);
	CRenderTexture* rendTex = (CRenderTexture*)&texture;
	renderer.attachTexture(0, rendTex->handle);
	uiTexShader->setTextureUnit(hTextureUnit, 0);
	uiTexShader->setShaderValue(hTexOrtho, ctrlMatrix);
	//renderer.drawBuf(templateQuad, drawTriStrip);
	renderer.drawTriStripBuf(templateQuad);
}


void uiDraw::drawTextureGradient(guiRect& drawBox, CBaseTexture& texture) {
	renderer.setShader(uiTexGradientShader);
	//pass the texture
	CRenderTexture* rendTex = (CRenderTexture*)&texture;
	renderer.attachTexture1D(0, rendTex->handle);

	glm::mat4 trans = glm::translate(glm::mat4(1), glm::vec3(drawBox.pos, 0));
	glm::mat4 shape = glm::scale(trans, glm::vec3(drawBox.size.x, drawBox.size.y, 1));
	glm::mat4 ctrlMatrix = orthoView * shape;

	uiTexGradientShader->setTextureUnit(hTextureUnitGrad, 0);
	uiTexGradientShader->setShaderValue(hTexGradOrtho, ctrlMatrix);
	renderer.drawTriStripBuf(templateQuad);
}

CFont* uiDraw::getFont(const std::string& name) {
	//return &renderer.fontManager.getFont(name);
	return fnt::get(name);
}

void uiDraw::setClip(UIrect& rect) {
	int flipY = renderer.Height - (rect.y + rect.height);
	//y = Renderer.Height - (y + height);
	renderer.setClip(rect.x, flipY, rect.width,rect.height);
}


 void initialiseTemplateQuads() {
	std::vector<vBuf::T2DtexVert> corners(4);

	corners[0].v = glm::vec2(0, 0); //A
	corners[1].v = glm::vec2(1, 0); //B  1,0
	corners[2].v = glm::vec2(0, 1); //C  0,1
	corners[3].v = glm::vec2(1, 1); //D  1,1
	//NB the top-left origin used for 2D flips Y. By pre-flipping quads we ensure they end up facing the right way

	corners[0].tex = glm::vec2(0);
	corners[1].tex = glm::vec2(1, 0);
	corners[2].tex = glm::vec2(0, 1);
	corners[3].tex = glm::vec2(1, 1);

	std::vector<unsigned int> index = { 2,3,0,1 };
	templateQuad.storeVerts(corners, index, 2, 2);


	//create border template quad
	std::vector<unsigned int> index2 = { 1,0,2,3 };
	borderTemplateQuad.storeVerts(corners, index2, 2, 2);

}


 void loadShaders() {

	 uiRectShader = renderer.createShader(renderer.dataPath + "shaders\\uiRect");
	 uiRectShader->setType(uiRect);
	 hOrtho = uiRectShader->getUniformHandle("orthoMatrix");
	 hColour1 = uiRectShader->getUniformHandle("colour1");
	 hColour2 = uiRectShader->getUniformHandle("colour2");

	 uiTexShader = renderer.createShader(renderer.dataPath + "shaders\\uiTexture");
	 uiTexShader->setType(uiTex);
	 hTexOrtho = uiTexShader->getUniformHandle("orthoMatrix");
	 hTextureUnit = uiTexShader->getUniformHandle("textureUnit");
	 hTile = uiTexShader->getUniformHandle("tile");
	 hOffset = uiTexShader->getUniformHandle("offset");

	 uiTexGradientShader = renderer.createShader("shaders\\uiTextureGradient");
	 uiTexGradientShader->setType(uiTex);
	 hTexGradOrtho = uiTexGradientShader->getUniformHandle("orthoMatrix");
	 hTextureUnitGrad = uiTexGradientShader->getUniformHandle("textureUnit");
 }