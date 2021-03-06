#pragma once

#define GLM_ENABLE_EXPERIMENTAL

//#include "Renderer.h"
#include "renderer\renderer.h"
//#include "ImageLib.h"
/////#include "SoundLib.h"
#include "Timer.h"
//#include "Sprite.h"
//#include "2DPolygon.h"
//#include "TileLayer.h"
#include "Config.h"
#include "renderer\camera.h"
#include "renderer\model.h"
#include "renderer\light.h"
#include "isoSurface.h"
#include "terrain.h"
#include "renderer\baseBuf.h"
#include "skyDome.h"
#include "renderer\material.h"
#include "renderer\renderMaterial.h"
#include "renderer\billboard.h"
#include "physics\physObjManager.h"
#include "physics\terrainPhysObj.h"

//#include "UI\uiRectShader.h"

//typedef std::vector<CSprite*> CRegisteredSpriteList; ///<Defines a vector type for holding sprites.


class CEngine
{
public:
	CEngine();
	//void setCurrentTexture( int textureNo);
	//int getTextureHandle(int textureNo);
	//void setCurrentTileSheet(TSpriteSheet* Sheet);
	//void setCurrentTileSheet(int sheet);
	//TSpriteSheet* loadSpriteSheet(const string&  Filename, int x, int y);
	//int loadSpritesheet(const string&  Filename, int x, int y);
	//void drawTile(int TileNo, float x, float y);
	//CSprite* loadSprite(char* Filename);
	//void drawSprite(const CSprite& Sprite);
	//int loadTexture( const string& Filename);
	void makeFit(int PixW, int PixH, bool Centre);
	//void registerSprite(const CSprite& Sprite);
	template<class TYPE>
	void registerSpriteArray(TYPE Sprite[], int NoSprites){
		for (int x=0;x<NoSprites;x++) {
			CSprite* SP = &Sprite[x];
			RegisteredSpriteList.push_back(SP);
		}
	};
//	void drawRegisteredSprites();
	//void updateRegisteredSprites(const double& dT);
	//void clearRegisteredSpriteList();
	//void CreateTileLayer(char* TileData, TSpriteSheet& Sheet, int Width, int Height);
	//void drawSceneLayers();
	//C2DVector ScrollBackdrop(float x, float y);

	void setDrawColour(const rgba& colour);
	void setSecondaryDrawColour(const rgba& colour);
	void setClip(int x, int y, int width, int height);

	void removeUserScale();
	void applyUserScale();
	void setUserScale(float x, float y);
	void changeUserScale(float x, float y);
	void resetViewMatrix();
	void applyGlobalScale();
	void clearFrame();
	//void showFrame();
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

	//CCamera* createCamera(glm::vec3& pos);
	//CModel* createCube(glm::vec3& pos, glm::vec3& size);
	void drawModels();
	void drawModelDefaultShader(CModel& model);
	void drawModel(CModel& model);
	unsigned int getShaderDataHandle(std::string varName);
	template <typename T>
	void setShaderValue(unsigned int handle,T& value) {
		Renderer.setShaderValue(handle,1,value);
	}
	template <typename T>
	void setShaderValue(unsigned int handle, int elements, T& value) {
		Renderer.setShaderValue(handle, elements, value);
	}

	void setCurrentShader(int program);
	CModel* createHemisphere(glm::vec3& pos, float radius, int steps);
	CModel* createPlane(glm::vec3& pos, float width, float height, int steps);
	//void storeModel(CModel* model, glm::vec3* verts, int noVerts);
	/** Send these indexed vertices to the graphics hardware to be buffered, and register them with the given model. */
	template <typename T>
	void storeIndexedModel(CModel* model, T* verts, unsigned int noVerts,  unsigned short* index) {
		model->storeVertexData(verts, noVerts, sizeof(T));
		model->storeIndexedData(index);
		int nAttributes = sizeof(T) / sizeof(glm::vec3);
		model->storeVertexLayout(((CRenderModel*)model)->buf.hIndex);
		((CRenderModel*)model)->buf.nAttribs = nAttributes;
	}
	void freeModel(CModel* model);
	CBaseTexture* createDataTexture(renderTextureFormat dataType, int w, int h, const void* data);
	void uploaddataTexture(int hShader, int hTexture);
	void setDataTexture(unsigned int textureHandle);
	void setFeedbackData(int shader, int nVars, const char** strings);
	unsigned int acquireFeedbackVerts(CBaseBuf& srcBuf, TdrawMode srcDrawMode, CBaseBuf& destBuf, TdrawMode destDrawMode);
	unsigned int drawModelCount(CModel& model);
	CModel* createModel();
	CBaseBuf* createBuffer();
	CSkyDome* createSkyDome();
	CMaterial* createMaterial(std::string filename);
	CMaterial * createMaterial();
	CShader* createShader(std::string name);
	CShader* createShader();
	

	CBillboard* createBillboard(glm::vec3& pos, glm::vec2 size);
	//void setCurrentCamera(CCamera* camera);
	CCamera* getCurrentCamera();
	CBasePhysObj* addPhysics(C3dObject* model);
	void recompileShaders();

	//CImageLib ImageLib;
	//CSoundLib SoundLib;
	CTimer Time;
//	CSceneObj Scene; 

	std::string dataPath; ///<Path to the data folder.
	int currentProgram;///<Handle for the shader program currently in use.
	int defaultProgram;

	//CCamera* defaultCamera; ///<Initially available camera.


	bool BackdropOn; ///<Flags whether to draw backdrop.
	bool MakingFit;///<True if we want to scale drawing to fit desired dimensions.

	int UIicons; ///<Refers to spritesheet used for radio buttons etc

	double dT; ///<Interval in milliseconds since the last frame.
	double cursorFlash; ///<Keeps track of how long since the text cursor last flashed.
	int tmpCursorPos; ///<Position of the cursor during any text-drawing.

	~CEngine(void);

	CRenderer& Renderer;



	C2dRenderer* p2dR; ///<Convenience pointer to the 2d renderer.


	void scaleToFit();

	void* WinHandle;  ///<Handle of the window we render to.
	//CSprite* CurrentSprite; ///<Points to the last-used billboard.
	unsigned int CurrentTexture; ///<Points to the last-used texture.

	int DesiredWidth; ///<Desired width of drawing area, in pixels.
	int DesiredHeight; ///<Desired height of drawing area, in pixels.
	bool DrawCentred; ///<True if the desired drawing area is to be centred within the view.
	
	//CRegisteredSpriteList RegisteredSpriteList;
	//std::vector<CCamera*> cameraList; ///<Tracks engine-created cameras.
	std::vector<CModel2*> modelDrawList; ///<Models to be draw automatically.
	//TO DO: scrap above and use name
	std::vector<CModel*> modelList;///<Engine-created models to delete at closedowm.
	
	public:
	glm::vec2 userScale; ///<Scaling factor applied to user drawing, every frame.
	//TSpriteSheet* CurrentTileSet; ///<Points to the last-used SpriteSheet.
	
	
	CSkyDome* skyDome;

	//TO DO: make a new image manager or resource manager for this kind of stuff:
	std::vector<CRenderMaterial*> materialList; ///<Simple list to track all engine-created materials.


	CPhysObjManager physObjManager;

private:
	//CCamera* currentCamera; ///<In-use camera for clipping.



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
