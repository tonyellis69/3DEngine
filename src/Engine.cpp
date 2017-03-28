#include "Engine.h"
#include <math.h>

#include <direct.h>
#include <iostream>
#include <gl/gl.h>
#include "vertBufs.h"
#include <glm/gtx/color_space.hpp>
#include "colour.h"



//using namespace glm;
using namespace vBuf;

CEngine::CEngine() {
	CurrentTileSet = 0; //TO DO - should be the built-in default;
	CurrentTexture = 0; //TO DO - should be the built-in default;
	CurrentSprite = 0;
	CurrentFont = 0;
	MakingFit = false;
	DrawCentred = false;
	BackdropOn = false;
	cursorFlash = 0;
	tmpCursorPos = -1;
	userScale.set(1,1);
	p2dR = &Renderer.rend2d;
	ambientLight = glm::vec4(0.2f,0.2f,0.2f,1);
}

/** Low level initialisation. */
void CEngine::init(HWND& hWnd) {
	Renderer.attachWindow(hWnd);
	Renderer.getGLinfo();
	CurrentFont = sysFont = ImageLib.LoadSysFont();
	ImageLib.LoadSmallSysFont();
	//TO DO: get icons.png built in
	UIicons = loadSpritesheet(dataPath+ "icons.png",32,32);

	
	Renderer.init();
	Renderer.setBackColour(engineTurquiose);


	currentCamera = defaultCamera = createCamera(glm::vec3(0,2,4));

	
	loadShader(vertex,dataPath + "default.vert");
	loadShader(frag,dataPath + "default.frag");
	currentProgram = defaultProgram = linkShaders();
	acquireDataLocations(currentProgram);

}


/** Resize the view - usually in response to the window being resized. */
void CEngine::resizeView(int x, int y, int width, int height) {
	Renderer.set2DView(x,y,width,height);
	if (MakingFit) 
		scaleToFit();
	//ensure any scene layers are up to date.
	Scene.Resize((float)Renderer.Width,(float) Renderer.Height);	
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
unsigned int CEngine::loadFont(const std::string& filename) {
	CurrentFont = ImageLib.loadFont((char*)filename.c_str());
	return CurrentFont->ID;
}

/** Draw a string at the given position, in the current font.*/
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

/** Draw a character in the current font at the given point, and return its width in pixels. */
int CEngine::drawCharacter(char letter, float x, float y) {
	Glyph* glyph = CurrentFont->table[letter];
	p2dR->drawToTextBuffer(&glyph->Rect, x, y);
	return (int)(glyph->Rect.width * p2dR->xScale); 
}

/** Find the width in pixels of this string, in the current font. */
int CEngine::findTextWidth(const char* Str) {
	int textWidth = 0;
	for (size_t i = 0; i != strlen(Str); ++i) {
		if (Str[i] == '\n') continue;
		textWidth += (int)(CurrentFont->table[Str[i]]->Rect.width * p2dR->xScale) ;
	}
	return textWidth;
}

/** Draw a text cursor at the given position, sized to the current font. */
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



/** Draw a formatted string at the given position, in the current font, with justification possible. */
void CEngine::drawString(float x, float y, int w, const char* format, ...) {
  char buffer[256];
  va_list args;
  va_start (args, format);
  vsprintf (buffer,format, args);
  perror (buffer);
  va_end (args);
  drawString(buffer,x,y,w);
}

/** Draw a formatted string at the given position. */
void CEngine::drawString(float x, float y, const char* format, ...) {
  char buffer[256];
  va_list args;
  va_start (args, format);
  vsprintf (buffer,format, args);
  perror (buffer);
  va_end (args);
  drawString(buffer,x,y,0);
}

void CEngine::drawString( char* text,float x, float y ) {
	drawString(text,x,y,0);
}

/** Create a texture from an image file, and return its texture index. */
int CEngine::loadTexture(const string& Filename) {
	int TexWidth, TexHeight;
	return ImageLib.CreateTextureFromImage(Filename.c_str(), &TexWidth, &TexHeight, true);
}

/** Set the texture of the backdrop. */
void CEngine::setBackdrop(int Texture) {
	Scene.setBackdropLayer(Texture);
}

void CEngine::setBackdrop(char* Filename) {
	int BackdropTex = loadTexture(Filename);
	Scene.CreateBackdropLayer(BackdropTex);
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
void CEngine::setFont(int FontNo) {
	CTexFont* Font = ImageLib.FontList[FontNo];
	CurrentFont = Font;
}


/** Draw the various backround layers of the scene, such as tile scenery. */
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

/** TO DO: this scrolls the whole scene, not just the backdrop. */
C2DVector CEngine::ScrollBackdrop(float x, float y) {
	return Scene.Scroll(x, y);
}

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


/** Draw the given grid object. */
void CEngine::drawGrid(CGrid& grid) {
	if (!grid.enabled)
		return;
	Renderer.setTextureMode(false);
	p2dR->setDrawColour((rgba&)grid.colour);
	float xExtent = Renderer.Width/userScale.x;
	float yExtent = Renderer.Height/userScale.y;
	for (float y=grid.yOffset;y<yExtent;y+=grid.size)
		p2dR->drawLine(0,y,xExtent,y);
	for (float x=grid.xOffset;x<xExtent;x+=grid.size)
		p2dR->drawLine(x,0,x,yExtent);
	if (grid.axesOn) {
		p2dR->setDrawColour((rgba&)grid.xColour);
		p2dR->drawLine(0,grid.origin.y,xExtent,grid.origin.y);
		p2dR->setDrawColour((rgba&)grid.yColour);
		p2dR->drawLine(grid.origin.x,0,grid.origin.x,yExtent);
	}
	Renderer.setTextureMode(true);	

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
	userScale.set(x,y);
}

/** Adjust the current user scale, without first resetting it to 1.*/
void CEngine::changeUserScale(float x, float y) {
	C2DVector scale(x,y);
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
CModel* CEngine::createCube(glm::vec3& pos,float size) {
	CModel* cube = new CModel(pos);

	float r = size/2.0f;
	glm::vec3 A(-r,r,r);
	glm::vec3 B(r,r,r);
	glm::vec3 C(r,-r,r);
	glm::vec3 D(-r,-r,r);
	glm::vec3 E(-r,r,-r);
	glm::vec3 F(r,r,-r);
	glm::vec3 G(r,-r,-r);
	glm::vec3 H(-r,-r,-r);





	const int indexSize = 36;

	vBuf::T3Dvert v[24];
	v[0].v=A;v[1].v=B;v[2].v=C;v[3].v=D;//front face
	v[0].normal = v[1].normal = v[2].normal = v[3].normal=glm::vec3(0,0,1);
	v[4].v=B;v[5].v=F;v[6].v=G;v[7].v=C;//right face
	v[4].normal = v[5].normal = v[6].normal = v[7].normal=glm::vec3(1,0,0);
	v[8].v=F;v[9].v=E;v[10].v=H;v[11].v=G;//back face
	v[8].normal = v[9].normal = v[10].normal = v[11].normal=glm::vec3(0,0,-1);
	v[12].v=E;v[13].v=A;v[14].v=D;v[15].v=H;//left face
	v[12].normal = v[13].normal = v[14].normal = v[15].normal=glm::vec3(-1,0,0);
	v[16].v=E;v[17].v=F;v[18].v=B;v[19].v=A;//top face
	v[16].normal = v[17].normal = v[18].normal = v[19].normal=glm::vec3(0,1,0);
	v[20].v=D;v[21].v=C;v[22].v=G;v[23].v=H;//bottom face
	v[20].normal = v[21].normal = v[22].normal = v[23].normal=glm::vec3(0,-1,0);
	glm::vec4 colour = glm::vec4(col::randHue(),1);
	for (int x=0;x<24;x++)
		v[x].colour = colour;

	//fill index with indices
	unsigned short index[indexSize] = {1,0,3,1,3,2,
		5,4,7,5,7,6,
		9,8,11,9,11,10,
		13,12,15,13,15,14,
		16,19,18,18,17,16,
		21,20,23,21,23,22}; 




	

	//cube->noVerts = 24;
	//cube->indexSize = indexSize;
	setVertexDetails(*cube, 1, indexSize, 24);
	storeIndexedModel(cube, v, index);
	CVertexObj* vertObj = &Renderer.getVertexObj(cube->hVertexObj);
	//vertObj->indexSize = indexSize;
	//vertObj->noVerts = 24;


	modelList.push_back(cube);

	return cube;
}

/** Draw all the models on the model list. */
void CEngine::drawModels() {
	setStandard3dShader();

	for (size_t m=0;m<modelList.size();m++) {
		drawModelDefaultShader(*modelList[m]);
	}
	Renderer.setShader(0); //for legacy compatibility
}

void CEngine::drawModelDefaultShader(CModel& model) {
	setShaderValue(Renderer.modelToWorldMatrix,model.worldMatrix);
	glm::mat4 mvp = currentCamera->clipMatrix * model.worldMatrix;
	setShaderValue(Renderer.mvpMatrix,mvp);

	glm::mat3 normMatrix(model.worldMatrix); //converting 4m to m3. TO DO: inefficient?
	//TO DO can't we just use worldMatrix inside shader?
	//TO DO: create a MVP matrix and send that instead
	setShaderValue(Renderer.normalModelToCameraMatrix,normMatrix);

	Renderer.drawModel(model);
}

void CEngine::drawModel(CModel& model) {
	Renderer.drawModel(model);
}


/** Set up the standard 3D shader. */
void CEngine::setStandard3dShader() {
	setCurrentShader(defaultProgram);
	Renderer.setShaderValue(Renderer.cameraToClipMatrix,currentCamera->clipMatrix);
	Renderer.setShaderValue(Renderer.lightDirection,light1.direction);
	Renderer.setShaderValue(Renderer.lightIntensity,light1.intensity);
	Renderer.setShaderValue(Renderer.ambientLight,ambientLight);
}

/** Ensure the renderer has the right data locations for this shader program. */
void CEngine::acquireDataLocations(int program) {
	Renderer.acquireDataLocations(program);
	//copy some of these so that the user can use them
	rNormalModelToCameraMatrix = Renderer.normalModelToCameraMatrix;
	rMVPmatrix = Renderer.mvpMatrix;

}

void CEngine::setCurrentShader(int program) {
	currentProgram = program;
	Renderer.setShader(program);
}

/** Get a handle for thie given variable used in the current shader. */
int CEngine::getShaderDataHandle(std::string varName) {
	return Renderer.getShaderDataHandle(currentProgram,varName);
}



CModel* CEngine::createCylinder(glm::vec3& pos,float r, float h, int s){
	float step = 360.0f/s;

	const int noVerts = (s * 4) + 2;
	const int topDisc = 0; const int botDisc = s;const int top = s*2;
	const int bot = s*3; const int topCent = s*4; const int botCent = topCent+1;
	vBuf::T3Dvert* v = new vBuf::T3Dvert[noVerts];
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
	glm::vec4 colour = glm::vec4(col::randHue(),1);
	for (int c=0;c<noVerts;c++)
		v[c].colour = colour;

	const int noTriangles = s*4;
	unsigned short* index = new unsigned short[noTriangles * 3];
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

	CModel* cylinder = new CModel(pos);
//	cylinder->noVerts =noVerts;// noTriangles * 3;
	//cylinder->indexSize = i;
	setVertexDetails(*cylinder, 1, i, noVerts);
	storeIndexedModel(cylinder,v, index );
	CVertexObj* vertObj = &Renderer.getVertexObj(cylinder->hVertexObj);
	//vertObj->indexSize = i;
	//vertObj->noVerts = noVerts;

	delete[] v;
	delete[] index;
	modelList.push_back(cylinder);
	return cylinder;
}

/** Render a 3D block of pixels of the given volume to a buffer, one pixel-layer after another. */
void CEngine::renderTo3DTexture(glm::vec3 vol, float* buf) {
	Renderer.renderTo3DTexture(currentProgram,vol.x,vol.y,vol.z,buf);
}

/** Render a 2D block of pixels of the given size to a buffer. */
void CEngine::renderTo2DTexture(glm::vec2 size, int* buf) {
	Renderer.renderTo2DTexture(currentProgram,size.x,size.y,buf);
	//glViewport(0,0,Renderer.Width,Renderer.Height);
}

/** Send these vertices to the graphics hardware to be buffered, and register them with the given model. */
void CEngine::storeModel(CModel* model, glm::vec3* verts, int noVerts ) {
	if (!model->hVertexObj)
		model->hVertexObj = Renderer.createVertexObj();
	CVertexObj* vertObj = &Renderer.getVertexObj(model->hVertexObj);
	vertObj->noVerts = noVerts;
	Renderer.storeVertexData(vertObj->hBuffer,verts, noVerts * vertObj->nAttribs * sizeof(glm::vec3));
	vertObj->hIndex = 0; //model->hIndex = 0;
	//vertObj->hBuffer = model->hBuffer;
	Renderer.storeVertexLayout(vertObj->hVAO, vertObj->hBuffer,0, vertObj->nAttribs);
	//vertObj->hVAO = model->hVAO;
	//vertObj->nAttribs = model->nAttribs;
	vertObj->indexSize = 0;
}



/** Free the various graphics memory buffers connected to this model.*/
void CEngine::freeModel(CModel* model) {
	CVertexObj* vertObj = &Renderer.getVertexObj(model->hVertexObj);
	Renderer.freeBuffer(vertObj->hBuffer);
	if (vertObj->hIndex > 0)
		Renderer.freeBuffer(vertObj->hIndex);
	Renderer.freeVAO(vertObj->hVAO);
	model->init();
}


/** Draw the model with the given shader, offscreen, and store the returned vertex data
	in the buffer. */
unsigned int CEngine::getGeometryFeedback(CModel& model, int size,int vertsPerPrimitive, unsigned int& hFeedBackBuf) {
	return Renderer.getGeometryFeedback(model,size, vertsPerPrimitive,hFeedBackBuf);
}


unsigned int CEngine::createDataTexture(renderTextureFormat dataType, int w, int h, const void* data) {

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


/** Run geometry feedback on the given input model, and return with the thus-created model in destModel. */
unsigned int CEngine::acquireFeedbackModel(CModel& srcModel, int feedbackBufSize, int vertsPerPrimitive, CModel& destModel) {
	if (!destModel.hVertexObj)
		destModel.hVertexObj = Renderer.createVertexObj();
	CVertexObj* dest = &Renderer.getVertexObj(destModel.hVertexObj);

	unsigned int noPrimitives = Renderer.getGeometryFeedback(srcModel, feedbackBufSize, vertsPerPrimitive, dest->hBuffer);

	

	//destModel.noVerts = noPrimitives*3; //assuming primitives are triangles
	dest->noVerts = noPrimitives * 3; //assuming primitives are triangles
//	dest->hBuffer = destModel.hBuffer;

	Renderer.storeVertexLayout(dest->hVAO,dest->hBuffer,0,dest->nAttribs);

	//destModel.hVAO = dest->hVAO;

	dest->nTris = noPrimitives;
	return noPrimitives;
}


unsigned int CEngine::drawModelCount(CModel& model) {
	Renderer.initQuery();
	Renderer.drawModel(model);
	return Renderer.query();
}

void CEngine::setVertexDetails(CModel & model, int noAttribs, int noIndices, int noVerts) {
	if (!model.hVertexObj)
		model.hVertexObj = Renderer.createVertexObj();
	CVertexObj* vertObj = &Renderer.getVertexObj(model.hVertexObj);
	vertObj->nAttribs = noAttribs;
	vertObj->noVerts = noVerts;
	vertObj->indexSize = noIndices;
}


CEngine::~CEngine(void) {
	for (size_t c=0;c<cameraList.size();c++)
		delete cameraList[c];
	for (size_t m=0;m<modelList.size();m++)
		delete modelList[m];
	Renderer.detachWindow();
}
