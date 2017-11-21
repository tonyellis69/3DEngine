#include "Engine.h"
#include <math.h>

#include <direct.h>
#include <iostream>
#include <gl/gl.h>
#include "vertBufs.h"
#include <glm/gtx/color_space.hpp>
#include "colour.h"
#include "renderer\renderMaterial.h"
#include "renderer\renderShader.h"
#include "physics\physObj.h"

#include "shapes.h"

//using namespace glm;
using namespace vBuf;

CEngine::CEngine() : Renderer(CRenderer::getInstance()) {

	CurrentTileSet = 0; //TO DO - should be the built-in default;
	CurrentTexture = 0; //TO DO - should be the built-in default;
	CurrentSprite = 0;
	//CurrentFont = 0;
	MakingFit = false;
	DrawCentred = false;
	BackdropOn = false;
	cursorFlash = 0;
	tmpCursorPos = -1;
	userScale = glm::vec2(1.0f,1.0f);
	p2dR = &Renderer.rend2d;
	skyDome = NULL;
}

/** Low level initialisation. */
void CEngine::init(HWND& hWnd) {
	Renderer.attachWindow(hWnd);
	Renderer.getGLinfo();
	//CurrentFont = sysFont = ImageLib.LoadSysFont();
	//ImageLib.LoadSmallSysFont();
	//TO DO: get icons.png built in
	UIicons = loadSpritesheet(dataPath+ "icons.png",32,32);

	Renderer.init();
	Renderer.setBackColour(engineTurquiose);

	defaultCamera = createCamera(glm::vec3(0,2,4));
	setCurrentCamera(defaultCamera);


	
}


/** Resize the view - usually in response to the window being resized. */
void CEngine::resizeView(int x, int y, int width, int height) {
	Renderer.set2DView(x,y,width,height);
	if (MakingFit) 
		scaleToFit();
	//ensure any scene layers are up to date.
	//Scene.Resize((float)Renderer.Width,(float) Renderer.Height);	
	currentCamera->setAspectRatio((float)width,(float)height);
}


/** Set the texture the renderer will use. */
void CEngine::setCurrentTexture(int textureNo) {
	Renderer.setCurrentTexture(getTextureHandle(textureNo));
}

/** Returns the texture handle of the given texture. */
/* TO DO: this is a kludge, as the engine shouldn't know about texture handles,
which are renderer-specific. It just deals in texture indexes.
Suggests that imageLib should be part of the renderer, to keep things internal.*/
int CEngine::getTextureHandle(int textureNo) {
	return ImageLib.textureList[textureNo]->handle;
}

/** Sets the in-use tileset to the given tilesheet.*/
void CEngine::setCurrentTileSheet(TSpriteSheet* Sheet) {
	CurrentTileSet = Sheet;
}

/** Sets the in-use tileset to the given tilesheet index.*/
void CEngine::setCurrentTileSheet(int sheet) {
	CurrentTileSet = ImageLib.GetTileSet(sheet);
}

/** Load a spriteSheet from file, returning a pointer to it. */
TSpriteSheet* CEngine::loadSpriteSheet(const string& Filename, int x, int y) {
	int tilesetNo = ImageLib.loadSpriteSheet(Filename.c_str(),x,y);
	return CurrentTileSet = ImageLib.GetTileSet(tilesetNo);
}

/** Load a spriteSheet from file, returning its index number. Useful for external classes, such as the GUI.*/
int CEngine::loadSpritesheet(const string& Filename, int x, int y) {
	return ImageLib.loadSpriteSheet(Filename.c_str(),x,y);
}

/** Call the renderer to draw the given tile at the given coordinates. */
void CEngine::drawTile(int TileNo, float x, float y) {
	setCurrentTexture(CurrentTileSet->textureNo);
	TRect tile;
	tile.Map = CurrentTileSet->Tiles[TileNo];

	tile.width = (float)CurrentTileSet->TileWidth;
	tile.height = (float)CurrentTileSet->TileHeight;
	tile.originX = (float)CurrentTileSet->TileHalfWidth;
	tile.originY = (float)CurrentTileSet->TileHalfHeight;
	p2dR->drawRect(&tile,x,y);	
}

/** Create a single-image sprite from file. */
CSprite* CEngine::loadSprite(char* Filename) {
	CurrentSprite = ImageLib.loadSprite(Filename);
	return CurrentSprite;
}

/** Draw the given sprite. */
void CEngine::drawSprite(const CSprite& Sprite) {
	CurrentSprite = (CSprite*) &Sprite;
	setCurrentTexture(CurrentSprite->textureNo);
	p2dR->setDrawColour((rgba&)Sprite.Colour);
	p2dR->xScale = CurrentSprite->Scale.x;
	p2dR->yScale = CurrentSprite->Scale.y;
	if (CurrentSprite->hFlip) { //TO DO: wtf is this?
		TRect Flip = CurrentSprite->Rect;
		Flip.Map.s = CurrentSprite->Rect.Map.u;
		Flip.Map.u = CurrentSprite->Rect.Map.s;
		p2dR->drawRect(&Flip,CurrentSprite->pos.x,CurrentSprite->pos.y );
	}
	else
		p2dR->drawRect(&CurrentSprite->Rect,CurrentSprite->pos.x,CurrentSprite->pos.y );
	p2dR->setDrawColour(1,1,1,1);
	p2dR->setDrawScale(1,1);
}


/** Load a font from file. */
/*
unsigned int CEngine::loadFont(const std::string& filename) {
	CurrentFont = ImageLib.loadFont((char*)filename.c_str());
	return CurrentFont->ID;
}*/

/** Draw a string at the given position, in the current font.*/
/*
void CEngine::drawString(char* Str, float x, float y, int w) {


	setCurrentTexture(CurrentFont->textureNo);
	float fontHeight = CurrentFont->Glyphs->Rect.height;

	if (w != 0) { //we're not printing left-justified
		int textWidth = findTextWidth(Str); //so find the width of our text
		int adj;
		if (w > 0 )  //draw centred
			adj = (w - textWidth) / 2;
		else
			adj = (abs(w) -  textWidth);
		if (adj > 0)
			x  += adj;
	}
	float startX = x;
	float topY = Renderer.Height - y;
	p2dR->initTextBuffer();
	int buffPos = 0;
	for (size_t i = 0; i != strlen(Str); ++i){
		if (Str[i] == '\n') { //newline
			x = startX;
			topY -= (int)fontHeight +2;
			continue;
		}
		x += drawCharacter(Str[i],x,topY);
	}
	p2dR->drawTextBuffer();
	//draw cursor if needed
	if (tmpCursorPos != -1)
		drawCursor( startX,  y, Str);
}
*/
/*
/** Draw a character in the current font at the given point, and return its width in pixels. */
/*
int CEngine::drawCharacter(char letter, float x, float y) {
	Glyph* glyph = CurrentFont->table[letter];
	p2dR->drawToTextBuffer(&glyph->Rect, x, y);
	return (int)(glyph->Rect.width * p2dR->xScale); 
}
*/
/** Find the width in pixels of this string, in the current font. */
/*
int CEngine::findTextWidth(const char* Str) {
	int textWidth = 0;
	for (size_t i = 0; i != strlen(Str); ++i) {
		if (Str[i] == '\n') continue;
		textWidth += (int)(CurrentFont->table[Str[i]]->Rect.width * p2dR->xScale) ;
	}
	return textWidth;
}
*/
/** Draw a text cursor at the given position, sized to the current font. */
/*
void CEngine::drawCursor(float x, float y, char* Text) {
	//Is time to draw the cursor?
	cursorFlash += dT;
	if (cursorFlash < 500) {
		//find the distance of the cursor from the start of the text
		char copyText[100];
		strcpy(copyText,Text);
		copyText[tmpCursorPos] = 0;
		int cursDistance = findTextWidth(copyText);
		//Draw it
		x+= cursDistance;
		int cursorHeight = (int) (CurrentFont->Glyphs->Rect.height-1 * p2dR->yScale);
		Renderer.setTextureMode(false);
		y = Renderer.Height - y;
		p2dR->drawLine(x,y,x,y - cursorHeight );
		Renderer.setTextureMode(true);
		return;
	}
	if (cursorFlash > 1000)
		cursorFlash = 0;
}
*/


/** Draw a formatted string at the given position, in the current font, with justification possible. */
/*
void CEngine::drawString(float x, float y, int w, const char* format, ...) {
  char buffer[256];
  va_list args;
  va_start (args, format);
  vsprintf (buffer,format, args);
  perror (buffer);
  va_end (args);
  drawString(buffer,x,y,w);
}
*/
/** Draw a formatted string at the given position. */
/*
void CEngine::drawString(float x, float y, const char* format, ...) {
  char buffer[256];
  va_list args;
  va_start (args, format);
  vsprintf (buffer,format, args);
  perror (buffer);
  va_end (args);
  drawString(buffer,x,y,0);
}
*/
/*
void CEngine::drawString( char* text,float x, float y ) {
	drawString(text,x,y,0);
}
*/
/** Create a texture from an image file, and return its texture index. */
int CEngine::loadTexture(const string& Filename) {
	int TexWidth, TexHeight;
	return ImageLib.CreateTextureFromImage(Filename.c_str(), &TexWidth, &TexHeight, true);
}




/** Tell engine to scale all drawing to fit these dimensions, if they lie outside the current screensize. */
void CEngine::makeFit(int PixW, int PixH, bool Centre) {
	MakingFit = true;
	DesiredWidth = PixW;
	DesiredHeight = PixH;
	DrawCentred = Centre;
}

/** Perform the necessary scaling to constrain drawing to the dimensions set up with makeFit. */
void CEngine::scaleToFit() {
	float dH = (float) max( DesiredHeight - Renderer.Height,0) ; 
	float dW = (float )max(DesiredWidth - Renderer.Width,0) ; 
	float Scale = 1;; float XOffset = 0; float YOffset = 0;

	if ((dH > 0) || (dW > 0)) { //Drawing needs to be scaled down to fit the current view.
		if (dH > dW) 
			Scale = (float)Renderer.Height / (float)DesiredHeight;
		else
			Scale = (float)Renderer.Width / (float) DesiredWidth;
	}

	Renderer.resetMatrix(); //Remove any previous transformations, so they don't stack up.

	if (DrawCentred) { //User wants drawing to be centred too.
		dH =  Renderer.Height - (DesiredHeight* Scale) ; 
		dW =  Renderer.Width - (DesiredWidth * Scale) ; 

		if (dH > 0) 
			YOffset = abs(dH)/2;
		if (dW > 0) 
			XOffset = abs(dW)/2;

		p2dR->setPosition(XOffset, YOffset); //perform transformation
	}

	p2dR->scale2D(Scale,Scale);  //perform scaling.

	Renderer.saveDisplayMatrix(); //save this new matrix for future reference.
	return;
}


/** Register a sprite with the engine. The means the engine will automatically draw the sprite,
	run its update routine, etc, every frame. */
void CEngine::registerSprite(const CSprite& Sprite) {
	RegisteredSpriteList.push_back((CSprite*)&Sprite);
}

/** Draw every sprite on the Registered Sprite list that's currently live. */
void CEngine::drawRegisteredSprites() {
	int NoSprites = RegisteredSpriteList.size();
	for (int i=0;i<NoSprites;++i) {
		if (RegisteredSpriteList[i]->Live) {
			drawSprite(*RegisteredSpriteList[i]);
		}
	}
}

/** Call the update method of every sprite on the Registered Sprite list that's currently live. */
void CEngine::updateRegisteredSprites(const double& dT) {
	int NoSprites = RegisteredSpriteList.size();
	for (int i=0;i<NoSprites;++i) {
		if (RegisteredSpriteList[i]->Live){
			RegisteredSpriteList[i]->Update(dT);
		}
	}
}

/** Empty the registered sprite list. */
void CEngine::clearRegisteredSpriteList() {
		RegisteredSpriteList.clear();
}

/** Make the given font the curretn font used for rendering text. */
/*
void CEngine::setFont(int FontNo) {
	CTexFont* Font = ImageLib.FontList[FontNo];
	CurrentFont = Font;
}
*/

/** Draw the various backround layers of the scene, such as tile scenery. */
/*
void CEngine::drawSceneLayers() {
	int NoLayers = Scene.LayerList.size();

	TRect Rect; float x,y; int textureNo;rgba drawColour;
	Scene.InitialiseDrawing();
	while (Scene.GetDrawData(Rect,textureNo, x,y,drawColour)) {
		p2dR->setDrawColour(drawColour);
		setCurrentTexture(textureNo);
		p2dR->drawRect(&Rect,x,y);
	}
}
*/
/** TO DO: this scrolls the whole scene, not just the backdrop. */
/*
C2DVector CEngine::ScrollBackdrop(float x, float y) {
	return Scene.Scroll(x, y);
} */

void CEngine::setDrawColour(const rgba& colour) {
	p2dR->setDrawColour(colour);
}
	
void CEngine::setSecondaryDrawColour(const rgba& colour) {
	p2dR->setSecondColour(colour.r,colour.g,colour.b,colour.a);
}


void CEngine::setClip(int x, int y, int width, int height) {
	y = Renderer.Height - (y + height);
	p2dR->setClip(x,y,width,height);
}



/** Set all subsequent drawing to be scaled by the previously saved factor, normally 1,1. */
void CEngine::removeUserScale() {
	Renderer.popMatrix();
}
/** Preserve the current matrix, then apply the current user scale. */
void CEngine::applyUserScale() {
		Renderer.pushMatrix();
		p2dR->scale2D(userScale.x,userScale.y);
}
/** Set the current user scale values, but don't apply them. */
void CEngine::setUserScale(float x, float y) {
	userScale = glm::vec2(x,y);
}

/** Adjust the current user scale, without first resetting it to 1.*/
void CEngine::changeUserScale(float x, float y) {
	glm::vec2 scale(x,y);
	userScale = userScale + scale;
}

/** Reset the renderer project matrix to the identity matrix, removing any scaling etc. */
void CEngine::resetViewMatrix() {
	Renderer.resetMatrix();
}

void CEngine::applyGlobalScale() {
	Renderer.useDisplayMatrix();
}

/** Erase the contents of the frame buffer to the current background colour, ready for drawing to.*/
void CEngine::clearFrame() {
	Renderer.clearFrame();
}

/** Display the current contents of the framebuffer. */
void CEngine::showFrame() {
	Renderer.showFrame();
}

void CEngine::enableLineMode(bool enabled) {
	Renderer.setTextureMode(!enabled);
}

/** Convert y from normal coordinate system (origin bottom-left) to UI system (origin top-left) and vice versa.*/
float CEngine::flipY(float y) {
	return Renderer.Height - y;
}

/** Return the width and height of the drawing area. */
void CEngine::getViewSize(int& width, int& height) {
	width = Renderer.Width;
	height = Renderer.Height;
}

/** Draw a rectangle from the bottom left corner, in the current drawing colours.*/
void CEngine::drawFilledRect(float x, float y, float width, float height) {
	enableLineMode(true);
	p2dR->drawBLRect(x,y,width,height);
	enableLineMode(false);
}

/** Draw an outline rectangle from the top left corner, in the current drawing colours. */
void CEngine::drawOutlineRect(float x, float y, float width, float height) {
	enableLineMode(true);
	p2dR->drawTLoutlineRect(x,y,width,height);
	enableLineMode(false);
}

/** Draw a 2d line in the current colours. */
void CEngine::drawLine(float x, float y, float x2, float y2) {
	enableLineMode(true);
	p2dR->drawLine(x,y,x2,y2);
	enableLineMode(false);
}

/** Draw the given image at the given position. */
void CEngine::drawImage(TRect* rect,float x, float y) {
	p2dR->drawRect(rect,x,y);
}

void CEngine::enableDottedLineMode(bool enabled) {
	enableLineMode(enabled);
	p2dR->dottedLineMode(enabled);
}

/** Set the internal scaling factor for drawing functions. */
void CEngine::setDrawScale(float x,float y) {
	p2dR->setDrawScale(x,y);
}

void CEngine::drawColourBufTri(vBuf::T2DVertTri* vertBuf, vBuf::TColTri* colourBuf, int size) {
	p2dR->drawColourBufTri(vertBuf,colourBuf,size);
}

/** Load the given shader file into memory.*/
void CEngine::loadShader(shaderType shader, std::string shaderFile) {
	Renderer.loadShader( shader, shaderFile);
}

/** Link currently loaded shaders into a program. */
unsigned int CEngine::linkShaders() {
	return Renderer.linkShaders();
}

/** Link the shaders attached to the given program. */
unsigned int CEngine::linkShaders(unsigned int program) {
	return Renderer.linkShaders(program);
}

/** Attach the currently loaded shaders as a program, but don't link them. */
unsigned int CEngine::attachShaders() {
	return Renderer.attachShaders();
}

/** Creates a new camera at the given position. */
CCamera* CEngine::createCamera(glm::vec3& pos) {
	CCamera* camera = new CCamera(pos);
	cameraList.push_back(camera); 
	return camera;
}

/** Create a cube model. */
CModel* CEngine::createCube(glm::vec3& pos,glm::vec3& size) {
	//TO DO: one day, this should move to a separate class: createShape. But this means passing
	//the model list to some other class too, yet to be created. Something like a scene manager.
	CModel* cube = createModel();
	cube->setPos(pos);

	cube->getMaterial()->setColour(glm::vec4(col::randHue(), 1));

	std::vector<glm::vec3> verts, normals; std::vector<unsigned int> index;
	shape::cube(verts, normals, index);
	shape::scale(verts, size);

	cube->getBuffer()->storeVertexes(verts, normals);
	cube->getBuffer()->storeIndex(index.data(), index.size());
	cube->getBuffer()->storeLayout(3, 3, 0, 0);

	return cube;
}

/** Draw all the models on the model list. */
void CEngine::drawModels() {
	//setStandard3dShader();

	for (size_t m=0;m<modelDrawList.size();m++) {
		drawModelDefaultShader(*modelDrawList[m]);
	}
	Renderer.setShader(0); //for legacy compatibility
}

void CEngine::drawModelDefaultShader(CModel& model) {
	Renderer.setShader(Renderer.phongShader);
	glm::mat4 mvp = currentCamera->clipMatrix * model.worldMatrix;
	Renderer.phongShader->setShaderValue(Renderer.hMVP,mvp);

	glm::mat3 normMatrix(model.worldMatrix); //converting 4m to m3. TO DO: inefficient?

	Renderer.phongShader->setShaderValue(Renderer.hNormalModelToCameraMatrix,normMatrix);

//	model.assignMaterial();
	model.drawNew();
}

void CEngine::drawModel(CModel& model) {
	Renderer.drawModel((CRenderModel&)model);
}


void CEngine::setCurrentShader(int program) {
	currentProgram = program;
	Renderer.setShader(program);
}

/** Get a handle for thie given variable used in the current shader. */
unsigned int CEngine::getShaderDataHandle(std::string varName) {
	return Renderer.getShaderDataHandle(currentProgram,varName);
}



CModel* CEngine::createCylinder(glm::vec3& pos,float r, float h, int s){
	float step = 360.0f/s;

	const int noVerts = (s * 4) + 2;
	const int topDisc = 0; const int botDisc = s;const int top = s*2;
	const int bot = s*3; const int topCent = s*4; const int botCent = topCent+1;
	vBuf::T3DnormVert* v = new vBuf::T3DnormVert[noVerts];
	//rotate through the segments, and create 4 rings of vertices:
	//top, where all the norms point up, base where they all point down,
	//and 2 for the body where they point out,
	//then create indexes 
	float angle = 0; float rangle = 0;
	for (int seg=0;seg < s;seg++) {
		rangle = glm::radians(angle);
		v[topDisc+seg].v = glm::vec3(sin(rangle)*r,h,cos(rangle)*r);
		v[topDisc+seg].normal = glm::vec3(0,1,0);
		v[botDisc+seg].v = glm::vec3(sin(rangle)*r,0,cos(rangle)*r);
		v[botDisc+seg].normal = glm::vec3(0,-1,0);
		v[top+seg].v = glm::vec3(sin(rangle)*r,h,cos(rangle)*r);
		v[top+seg].normal = glm::vec3(sin(rangle),0,cos(rangle));
		v[bot+seg].v = glm::vec3(sin(rangle)*r,0,cos(rangle)*r);
		v[bot+seg].normal = glm::vec3(sin(rangle),0,cos(rangle));
		angle += step;
	}
	v[topCent].v = glm::vec3(0,h,0); v[topCent].normal = glm::vec3(0,1,0);
	v[botCent].v = glm::vec3(0,0,0); v[botCent].normal = glm::vec3(0,-1,0);


	const int noTriangles = s*4;
	unsigned int* index = new unsigned int[noTriangles * 3];
	int i = 0;
	const int finalSeg = s-1;
	for (int seg=0;seg < finalSeg;seg++) {
		index[i++] = topDisc+seg;index[i++] = topDisc+seg+1;index[i++] = topCent;
		index[i++] = botDisc+seg;index[i++] = botCent;index[i++] = botDisc+seg+1;
		index[i++] = bot+seg; index[i++] =  bot+seg+1; index[i++] = top+seg+1;
		index[i++] = top+seg+1; index[i++] =  top+seg; index[i++] = bot+seg;
	}
	
	index[i++] = topDisc+finalSeg;index[i++] = topDisc;index[i++] = topCent;
	index[i++] = botDisc+finalSeg;index[i++] = botCent;index[i++] = botDisc;
	index[i++] = bot+finalSeg; index[i++] =  bot; index[i++] = top;
	index[i++] = top; index[i++] =  top+finalSeg; index[i++] = bot+finalSeg;

	CModel* cylinder = createModel();
	cylinder->setPos(pos);
	cylinder->getMaterial()->setColour(glm::vec4(col::randHue(), 1));



	cylinder->storeVertexes(v, sizeof(vBuf::T3DnormVert) , noVerts);
	cylinder->storeIndex(index, i);
	cylinder->storeLayout(3, 3,0, 0);

	delete[] v;
	delete[] index;
//	modelList.push_back(cylinder);
	modelDrawList.push_back(cylinder);
	return cylinder;
}

CModel * CEngine::createHemisphere(glm::vec3 & pos, float radius, int steps) {
	float ringStep = 360.0f / steps;
	const int noRings = steps / 4;
	const int noVerts = (steps * noRings) + 1;
	vBuf::T3DnormVert* v = new vBuf::T3DnormVert[noVerts];
	int vNo = 0;

	//for each climb of theta
	//create a ring of points
	float theta = 0; float rTheta;
	float lambda = 0; float rLambda;
	for (int ring = 0; ring < noRings; ring++) {
		rTheta = glm::radians(theta);
		for (int seg = 0; seg < steps; seg++) {
			rLambda = glm::radians(lambda);
			v[vNo].v = glm::vec3(sin(rLambda) * cos(rTheta), sin(rTheta), cos(rLambda) * cos(rTheta)) * radius;
			v[vNo].normal = glm::vec3(sin(rLambda) * cos(rTheta), sin(rTheta), cos(rLambda) * cos(rTheta)) * -1.0f;
	
			vNo++;
			lambda += ringStep;
		}
		theta += ringStep;
	}

	v[vNo].v = glm::vec3(0.0f, radius, 0.0f); //apex
	v[vNo].normal = glm::vec3(0.0f, -1, 0.0f);
	

	const int noTriangles = (steps * 2 * (noRings - 1)) + steps;
	unsigned int* index = new unsigned int[noTriangles * 3];
	//for each ring except the last, use its verts to make two triangles
	int baseRing, topRing; int i = 0; int seg;
	for (int ring = 0; ring < noRings - 1; ring++) {
		baseRing = ring * steps; topRing = baseRing + steps;
		for (seg = 0; seg < steps-1; seg++) {
			index[i++] = topRing + seg; index[i++] = baseRing + seg + 1; index[i++] = baseRing + seg;
			index[i++] = topRing + seg + 1; index[i++] = baseRing + seg + 1; index[i++] = topRing + seg;
		}
		index[i++] = topRing + seg; index[i++] = baseRing; index[i++] = baseRing + seg;
		index[i++] = topRing; index[i++] = baseRing ; index[i++] = topRing + seg;
	}

	//cap
	for (seg = 0; seg < steps - 1; seg++) {
		index[i++] = vNo; index[i++] = topRing + seg +1; index[i++] = topRing + seg;
	}
	index[i++] = vNo; index[i++] = topRing ; index[i++] = topRing + seg;
	
	CModel* dome = createModel();
	dome->setPos(pos);
	dome->getMaterial()->setColour(glm::vec4(col::randHue(), 1));

	dome->storeVertexes(v, sizeof(vBuf::T3DnormVert) , noVerts);
	dome->storeIndex(index,  i);
	dome->storeLayout(3, 3, 0, 0);
	dome->setDrawMode(drawTris);

	delete[] v;
	delete[] index;
	//modelList.push_back(dome);

	return dome;
}

CModel * CEngine::createPlane(glm::vec3 & pos, float width, float depth, int steps) {
	//calculate the number of verts
	const int noVerts = (steps + 1) * (steps + 1);
	const float  xStep = width / steps; 
	const float  zStep = depth / steps;
	const float  texStep = 1.0f / (steps + 1);
	const float xOffset = width * -0.5f; const float zOffset = width * -0.5f;
	const float height = width * 0.05f;

	vBuf::T3DtexVert* v = new vBuf::T3DtexVert[noVerts];

	//create them
	const float maxDist = glm::length(glm::vec2(xOffset , zOffset)) * 0.25f;
	int vNo = 0; float dist, xPos, zPos, yPos;
	for (int x = 0; x <= steps; x++) {
		for (int z = 0; z <= steps; z++) {
			xPos = x * xStep + xOffset;
			zPos = z * zStep + zOffset;
			dist = glm::length(glm::vec2(xPos, zPos));
			//dist = max(glm::abs(xPos), abs(zPos));
			dist = (dist/ maxDist  );
			if (dist > 1)
				dist = 1;
			yPos = height - (height * dist);
			v[vNo].v = glm::vec3(xPos, yPos, zPos);
			v[vNo++].tex = glm::vec2(x * texStep , z * texStep );
		}
	}

	//create indices
	const int noTriangles = (steps * steps) * 2;
	unsigned int* index = new unsigned int[noTriangles * 3];
	int i = 0;
	const int nextRow = steps + 1;

	for (int x = 0; x < steps; x++) {
		for (int z = 0; z < steps; z++) {
			index[i++] = x + (z * (nextRow)); index[i++] = x + ((z + 1) * (nextRow)); index[i++] = x + ((z + 1) * (nextRow)) + 1;
			index[i++] = x + ((z + 1) * (nextRow)) + 1; index[i++] = x + (z * (nextRow)) + 1; index[i++] = x + (z * (nextRow));
		}
	}

	CModel* plane = createModel();
	plane->setPos(pos);
	plane->getMaterial()->setColour(glm::vec4(col::randHue(), 1));

	plane->storeVertexes(v, sizeof(vBuf::T3DtexVert) , noVerts);
	plane->storeIndex(index,  i);
	plane->storeLayout(3, 2, 0, 0);
	plane->setDrawMode(drawTris);

	delete[] v;
	delete[] index;
//	modelList.push_back(plane);

	return plane;
}







/** Free the various graphics memory buffers connected to this model.*/
void CEngine::freeModel(CModel* model) {
	model->freeBuffers();
	model->init();
}


/** Draw the model with the given shader, offscreen, and store the returned vertex data
	in the buffer. */
//unsigned int CEngine::getGeometryFeedback(CModel& model, int size,int vertsPerPrimitive, unsigned int& hFeedBackBuf, unsigned int multiBufferOffset) {
//	return Renderer.getGeometryFeedback(model,size, vertsPerPrimitive,hFeedBackBuf, multiBufferOffset);
//}


CBaseTexture* CEngine::createDataTexture(renderTextureFormat dataType, int w, int h, const void* data) {

	return Renderer.createDataTexture(dataType,w,h,data);
}

void CEngine::uploadDataTexture(int hShader, int hTexture) {
	Renderer.uploadDataTexture(hShader,hTexture);
}

void CEngine::setDataTexture(unsigned int textureHandle) {
	Renderer.setDataTexture(textureHandle);
}

void CEngine::setFeedbackData(int shader, int nVars, const char** strings) {
	Renderer.setFeedbackData(shader, nVars, strings);
}



// pass a CBaseBuf and get it back full of feedback verts
unsigned int CEngine::acquireFeedbackVerts(CBaseBuf& srcBuf, TdrawMode srcDrawMode, CBaseBuf& destBuf, TdrawMode destDrawMode) {
	unsigned int noPrimitives = 0;
	noPrimitives = Renderer.getGeometryFeedback((CBuf&)srcBuf, srcDrawMode, (CBuf&)destBuf, destDrawMode);

	return noPrimitives;
}


unsigned int CEngine::drawModelCount(CModel& model) {
	Renderer.initQuery();
	model.drawNew();
	return Renderer.query();
}



/**	Create a CModel (subclassed to CRenderModel) and return a pointer to it. */
CModel * CEngine::createModel() {
	CRenderModel* model = new CRenderModel();
	//model->pRenderer = &Renderer; //TO DO: do in a setter!
	//model->buf.setRenderer(&Renderer); //TO DO: above setter should handle this!
	CMaterial* material = createMaterial();
	model->setMaterial(*material);
	modelList.push_back(model);
	return model;
}



CBaseBuf * CEngine::createBuffer() {
	return Renderer.createBuffer();
}

CSkyDome * CEngine::createSkyDome() {
	if (skyDome != NULL) {
		cerr << "\nAttemp to create skydome when it already exists.";
		return NULL;
	}

	CModel* dome = createHemisphere(glm::vec3(0, 0, 0), 1, 25);

	skyDome = new CSkyDome();
	skyDome->setModel(dome);

	//load skyDome shader
	skyDome->skyShader = Renderer.createShader(dataPath + "skyDome");
	skyDome->skyShader->setType(userShader);
	skyDome->hSkyDomeHeightColours = skyDome->skyShader->getUniformHandle("heightColour");
	skyDome->hSkyDomeMVP = skyDome->skyShader->getUniformHandle("mvpMatrix");
	dome->getMaterial()->setShader(skyDome->skyShader);

	//cloud plane:
	skyDome->plane = createPlane(glm::vec3(0, 450, -4), 10000, 10000, 20);

	//create cloud material
	skyDome->cloud = createMaterial(dataPath + "cloud001.png");
	skyDome->cloud->setShader(Renderer.multiTexShader);
	skyDome->cloud->setTile(0, glm::vec2(20));
	skyDome->cloud->addImage(dataPath + "cloud002.png");
	skyDome->cloud->setTile(1, glm::vec2(20));
	skyDome->plane->setMaterial(*skyDome->cloud);

	//create sun billboard
	skyDome->sunBoard = createBillboard(glm::vec3(0, 400, -400), glm::vec2(50, 50));
	skyDome->sunMat = createMaterial(dataPath + "sun2.png");
	skyDome->sunMat->setShader(Renderer.billboardShader);
	skyDome->sunBoard->setMaterial(*skyDome->sunMat);
	return skyDome;
}

CMaterial * CEngine::createMaterial(std::string filename) {
	CMaterial* material = createMaterial();
	material->addImage(filename);
	return material;
}

CMaterial * CEngine::createMaterial() {
	CRenderMaterial* material = new CRenderMaterial();
	material->setShader(Renderer.phongShader);
	materialList.push_back(material);
	return material;
}

CShader * CEngine::createShader(std::string name) {
	CShader* shader = createShader();
	shader->create(name);
	return shader;
}

CShader * CEngine::createShader() {
	CRenderShader* shader = new CRenderShader();
	Renderer.shaderList.push_back(shader);
	return shader;
}







CBillboard * CEngine::createBillboard(glm::vec3 & pos, glm::vec2 size) {
	//create the object
	CBillboard* billboard = new CBillboard(pos, size);
	//billboard->pRenderer = &Renderer;
	CMaterial* material = createMaterial();
	material->setShader(Renderer.billboardShader);
	billboard->setMaterial(*material);
	modelList.push_back(billboard);
		 
	//return it
	return billboard;
}


void CEngine::setCurrentCamera(CCamera * camera) {
	currentCamera = camera;
	Renderer.currentCamera = camera;
}

CCamera * CEngine::getCurrentCamera() {
	return currentCamera;
}

CBasePhysObj * CEngine::addPhysics(C3dObject * model) {
	return	physObjManager.addModel(model);
}

void CEngine::recompileShaders() {
	for (size_t s = 0; s < Renderer.shaderList.size(); s++)
		Renderer.shaderList[s]->recompile();
}




CEngine::~CEngine(void) {
	for (size_t c=0;c<cameraList.size();c++)
		delete cameraList[c];
	for (size_t m = 0; m < modelList.size(); m++)
		 delete modelList[m];
	if (skyDome)
		delete skyDome;
	for (size_t m = 0; m<materialList.size(); m++)
		delete materialList[m];
	for (size_t s = 0; s<Renderer.shaderList.size(); s++)
		delete Renderer.shaderList[s];

	Renderer.detachWindow();
}
