#pragma once

//#include "Renderer.h"
#include "renderer\renderer.h"
#include "ImageLib.h"
#include "SoundLib.h"
#include "Timer.h"
#include "Sprite.h"
#include "2DPolygon.h"
#include "TileLayer.h"
#include "Config.h"
#include "grid.h"
#include "renderer\camera.h"
#include "renderer\model.h"
#include "renderer\light.h"
#include "isoSurface.h"
#include "terrain.h"
#include "renderer\baseBuf.h"

typedef std::vector<CSprite*> CRegisteredSpriteList; ///<Defines a vector type for holding sprites.


class CEngine
{
public:
	CEngine();
	void init(HWND& hWnd);
	void resizeView(int x, int y, int width, int height);
	void setCurrentTexture( int textureNo);
	int getTextureHandle(int textureNo);
	void setCurrentTileSheet(TSpriteSheet* Sheet);
	void setCurrentTileSheet(int sheet);
	TSpriteSheet* loadSpriteSheet(const string&  Filename, int x, int y);
	int loadSpritesheet(const string&  Filename, int x, int y);
	void drawTile(int TileNo, float x, float y);
	CSprite* loadSprite(char* Filename);
	void drawSprite(const CSprite& Sprite);
	unsigned int loadFont(const std::string& filename);
	void drawString(char* Str, float x, float y, int w);
	void drawString(float x, float y, int w, const char* format, ...);
	void drawString(float x, float y, const char* format, ...);
	void drawString( char* text,float x, float y );
	int drawCharacter(char letter, float x, float y);
	int findTextWidth(const char* Str);
	void drawCursor(float x, float y, char* Text);
	void drawStringWithCursor(int x, int y, int c, char* Text);
	int loadTexture( const string& Filename);
	void setBackdrop(int Texture);
	void setBackdrop(char* Filename);
	void setBackdropTiling(float u, float v, float s, float t);
	void makeFit(int PixW, int PixH, bool Centre);
	void registerSprite(const CSprite& Sprite);
	template<class TYPE>
	void registerSpriteArray(TYPE Sprite[], int NoSprites){
		for (int x=0;x<NoSprites;x++) {
			CSprite* SP = &Sprite[x];
			RegisteredSpriteList.push_back(SP);
		}
	};
	void drawRegisteredSprites();
	void updateRegisteredSprites(const double& dT);
	void clearRegisteredSpriteList();
	void setFont(int FontNo);
	void CreateTileLayer(char* TileData, TSpriteSheet& Sheet, int Width, int Height);
	void drawSceneLayers();
	C2DVector ScrollBackdrop(float x, float y);

	void setDrawColour(const rgba& colour);
	void setSecondaryDrawColour(const rgba& colour);
	void setClip(int x, int y, int width, int height);

	void drawGrid(CGrid& grid);
	void removeUserScale();
	void applyUserScale();
	void setUserScale(float x, float y);
	void changeUserScale(float x, float y);
	void resetViewMatrix();
	void applyGlobalScale();
	void clearFrame();
	void showFrame();
	void enableLineMode(bool enabled);
	float flipY(float y);
	void getViewSize(int& width, int& height);
	void drawFilledRect(float x, float y, float width, float height);
	void drawOutlineRect(float x, float y, float width, float height);
	void drawLine(float x, float y, float x2, float y2);
	void drawImage(TRect* rect,float x, float y);
	void enableDottedLineMode(bool enabled);
	void setDrawScale(float x,float y);
	void drawColourBufTri(vBuf::T2DVertTri* vertBuf, vBuf::TColTri* colourBuf, int size);

	void loadShader(shaderType shader, std::string shaderFile);
	unsigned int linkShaders();
	unsigned int linkShaders(unsigned int program);
	unsigned int attachShaders();

	CCamera* createCamera(glm::vec3& pos);
	CModel* createCube(glm::vec3& pos,float size);
	void drawModels();
	void drawModelDefaultShader(CModel& model);
	void drawModel(CModel& model);
	void setStandard3dShader();
	int getShaderDataHandle(std::string varName);
	template <typename T>
	void setShaderValue(int handle,T& value) {
		Renderer.setShaderValue(handle,value);
	}

	void acquireDataLocations(int progam);
	void setCurrentShader(int program);
	CModel* createCylinder( glm::vec3& pos,float r, float h, int s);
	void renderTo3DTexture(glm::vec3 vol, float* buf);
	void renderTo2DTexture(glm::vec2 size, int* buf);
	void storeModel(CModel* model, glm::vec3* verts, int noVerts);
	/** Send these indexed vertices to the graphics hardware to be buffered, and register them with the given model. */
	template <typename T>
	void storeIndexedModel(CModel* model, T* verts, unsigned int noVerts,  unsigned short* index) {
		//CBuf* vertObj = &Renderer.getVertexObj(model->hVertexObj);
		//Renderer.storeVertexData(vertObj->hBuffer,(glm::vec3*)verts, noVerts * sizeof(T));
		model->storeVertexData(verts, noVerts, sizeof(T));

		//Renderer.storeIndexData(vertObj->hIndex,index, sizeof(unsigned short) * vertObj->indexSize);
		model->storeIndexedData(index);
	
		int nAttributes = sizeof(T) / sizeof(glm::vec3);
		//Renderer.storeVertexLayout(vertObj->hVAO, vertObj->hBuffer, vertObj->hIndex,sizeof(T) / sizeof(glm::vec3));
		model->storeVertexLayout(((CRenderModel*)model)->buf.hIndex);

		//Renderer.getVertexObj(model->hVertexObj).nAttribs = nAttributes;
		((CRenderModel*)model)->buf.nAttribs = nAttributes;
	}
	void freeModel(CModel* model);
	//unsigned int getGeometryFeedback(CModel& model,  int size, int vertsPerPrimitive, unsigned int& hFeedBackBuf, unsigned int multiBufferOffset);
	unsigned int createDataTexture(renderTextureFormat dataType, int w, int h, const void* data);
	void uploadDataTexture(int hShader, int hTexture);
	void setDataTexture(unsigned int textureHandle);
	void setFeedbackData(int shader, int nVars, const char** strings);
	//unsigned int acquireFeedbackModel(CModel& srcModel, int feedbackBufSize, int vertsPerPrimitive, CModel& destModel);
	//unsigned int acquireFeedbackModelMulti(CModel& srcModel, int feedbackBufSize, int vertsPerPrimitive, CModelMulti& destModel);
	unsigned int acquireFeedbackVerts(CModel& srcModel, CBaseBuf&  tmpBuf, CBaseBuf& destBuf);
	unsigned int drawModelCount(CModel& model);
	void setVertexDetails(CModel* model, int noAttribs, int noIndices, int noVerts);
	void setVertexDetailsMulti(CModelMulti& model, int noAttribs, int noIndices,unsigned int bufSize);
	CModel* createModel();
	CTerrain* createTerrain();
	void drawMultiModel(CModelMulti& model);
	CBaseBuf* createBuffer();

	CImageLib ImageLib;
	CSoundLib SoundLib;
	CTimer Time;
	CSceneObj Scene; 

	std::string dataPath; ///<Path to the data folder.
	int currentProgram;///<Handle for the shader program currently in use.
	int defaultProgram;

	CCamera* currentCamera; ///<In-use camera for clipping.
	CCamera* defaultCamera; ///<Initially available camera.

	CTexFont* sysFont;  ///<Points to the default system font.
	CTexFont* CurrentFont; ///<Points to the last-used font.

	bool BackdropOn; ///<Flags whether to draw backdrop.
	bool MakingFit;///<True if we want to scale drawing to fit desired dimensions.

	int UIicons; ///<Refers to spritesheet used for radio buttons etc

	double dT; ///<Interval in milliseconds since the last frame.
	double cursorFlash; ///<Keeps track of how long since the text cursor last flashed.
	int tmpCursorPos; ///<Position of the cursor during any text-drawing.

	~CEngine(void);

	//	C2dRenderer* p2dR; ///<Convenience pointer to the 2d renderer.
//private:
	CRenderer Renderer;
	C2dRenderer* p2dR; ///<Convenience pointer to the 2d renderer.


	void scaleToFit();

	void* WinHandle;  ///<Handle of the window we render to.
	CSprite* CurrentSprite; ///<Points to the last-used billboard.
	unsigned int CurrentTexture; ///<Points to the last-used texture.

	int DesiredWidth; ///<Desired width of drawing area, in pixels.
	int DesiredHeight; ///<Desired height of drawing area, in pixels.
	bool DrawCentred; ///<True if the desired drawing area is to be centred within the view.
	
	CRegisteredSpriteList RegisteredSpriteList;
	std::vector<CCamera*> cameraList; ///<Tracks engine-created cameras.
	std::vector<CModel*> modelList; ///<Tracks engine-created models.
	//TO DO: scrap above and use name
	
	public:
	C2DVector userScale; ///<Scaling factor applied to user drawing, every frame.
	TSpriteSheet* CurrentTileSet; ///<Points to the last-used SpriteSheet.
	CLight light1; ///<Primary lightsource for 3D rendering.
	glm::vec4 ambientLight; ///<Ambient light for standard shader.

	unsigned int rNormalModelToCameraMatrix;
	unsigned int rMVPmatrix; ///<Handle to the standard shader combined MVP matrix.
	

	std::vector<CRenderModel*> renderModelList; ///<Tracks engine-created models.
};

const int NoCursor = -1;

//some colours
const rgba engineWhite = {1,1,1,1};
const rgba engineBlack = {0,0,0,1};
const rgba engineRed = {1,0,0,1};
const rgba engineGreen = {0,1,0,1};
const rgba engineBlue = {0,0,1,1};
const rgba engineTurquiose = {0.0f,0.7f,0.7f,0.5f};

//Keyboard keypress codes
#define VK_A 65
#define VK_D 68
#define VK_S 83
#define VK_Z 90
