#pragma once



#include "2d.h"
//#include <windows.h> 
#include <string>
#include <vector>

//#include "..\soil.h"


#include "camera.h"
#include "modelMulti.h"
#include "renderModel.h"
#include "buf.h"
//#include "baseTexture.h"
#include "texManager.h"
#include "..\terrain.h"
#include "shaderDraw.h"

#include "renderShader.h"

#include "fontManager.h"

#include "model2.h"

enum renderTextureFormat {floatTex,intTex, uintTex};
enum  TUidrawMode { uiDrawPoints=0, uiDrawLines=1, uiDrawLineLoop = 2, uiDrawLineStrip=3, uiDrawTris=4, 
	uiDrawTriStrip=5, uiDrawQuads=7};
enum  TRendererDrawMode {
	rDrawPoints = 0, rDrawLines = 1, rDrawLineLoop = 2, rDrawLineStrip = 3, rDrawTris = 4,
	rDrawTriStrip = 5, rDrawQuads = 7, rDrawLinesAdjacency = 10 };



extern int totalbufsize;
extern int totalchunks;


/** The high-level renderer class. */
class CBaseBuf;
class CRenderer  {
public:
	CRenderer();
	~CRenderer();
	/** Return a reference to the renderer singleton, initialising it if necessary. */
	static CRenderer & CRenderer::getInstance() {
		static CRenderer *instance = new CRenderer();
		return *instance;
	}
	void getGLinfo();
	void clearFrame();
	void setBackColour(const rgba& colour);
	void set2DView(int x, int y,int width,int height);
	void init();
	void setTextureMode(bool enabled);
	void setCurrentTexture(int textureHandle);	
	void saveDisplayMatrix();
	void useDisplayMatrix();
	void resetMatrix();
	void pushMatrix();
	void popMatrix();
	void loadShader(shaderType type, std::string shaderFilename);
	unsigned int linkShaders(unsigned int program);
	unsigned int linkShaders();
	unsigned int attachShaders();
	void storeVertexData(unsigned int& handle, glm::vec3* data,int size);
	void storeIndexData(unsigned int& hIndex,unsigned short* data,int size);
	void storeVertexLayout(unsigned int& hVAO,unsigned int bufferObj, unsigned int hIndex,int nAttributes);
	void freeBuffer(unsigned int buffer);
	void freeVAO(unsigned int hVAO);
	unsigned int getShaderDataHandle(int program, std::string varName);
	void setShaderValue(unsigned int matrixHandle,int elements, glm::mat4& matrix);
	void setShaderValue(unsigned int matrixHandle, int elements, glm::mat3& matrix);
	void setShaderValue(unsigned int vecHandle, int elements, glm::vec2& vector);
	void setShaderValue(unsigned int vecHandle, int elements, glm::i32vec2& vector);
	void setShaderValue(unsigned int vecHandle, int elements, glm::vec3& vector);
	void setShaderValue(unsigned int vecHandle, int elements, const glm::vec4& vector);
	void setShaderValue(unsigned int intHandle, int elements, int value);
	void setShaderValue(unsigned int floatHandle, int elements, float value);
	void setShader(int program);
	void setShader(CShader* shader);
	void drawModel(CRenderModel& model);
	//void initRenderToTextureBufs();
	void createFrameBuffer();
	void createScreenQuad();
	void renderToTextureQuad(CBaseTexture & texture);
	void renderToTextureQuad(CBaseTexture& texture, glm::i32vec2& offset, glm::i32vec2& size);
	void renderToTextureTris(CBuf & buffer, CBaseTexture & texture);
	void renderToTextureTriStrip(CBuf& buffer, CBaseTexture& texture);
	void renderToTexturePoints(CBuf & buffer, CBaseTexture & texture);
	void beginRenderToTexture(CBaseTexture & texture);
	void endRenderToTexture();
	void rendertToTextureClear(CBaseTexture& texture, glm::vec4 & colour);
	unsigned int getGeometryFeedback(CBuf& srcBuf, TdrawMode srcDrawMode, CBuf& destBuf, TdrawMode destDrawMode);
	CBaseTexture* createDataTexture(renderTextureFormat dataType, int w, int h, const void* data);
	void uploadDataTexture(int hShader, int hTexture);
	void setDataTexture(unsigned int textureHandle);
	void setFeedbackData(int shader, int nVars, const char** strings);
	void initQuery();
	unsigned int query();
	void initTimerQuery();
	unsigned long getTimerQuery();
	void drawMultiBufChildVerts(TdrawMode drawMode, CMultiBuf& multiBuf, int childBufNo, unsigned int vertStart, unsigned int vertCount);
	void drawMultiBufChildInstanced(TdrawMode drawMode, CMultiBuf & multiBuf, int childBufNo, unsigned int vertStart, unsigned int vertCount);
	void setDepthTest(bool on);
	void createTextureFromImageFile(std::string filename);
	void attachTexture(unsigned int textureUnit, unsigned int hTexture);
	void attachTexture1D(unsigned int textureUnit, unsigned int hTexture);
	void attachTexture(unsigned int textureUnit, CBaseTexture& texture);
	void drawBuf(CBuf& buf, TdrawMode drawMode);
	void drawLineLoopBuf(CBuf& buf);
	void drawLineStripBuf(CBuf& buf);
	void drawLinesBuf(CBuf& buf);
	void drawLinesRange(int start, int count, CBuf& buf);
	void drawTriStripBuf(CBuf& buf);
	unsigned int getGLdrawMode(TdrawMode);
	CBaseBuf* createBuffer();
	void createStandardPhongShader();
	static void phongDrawCallout(void* callee, CModel2* model);
	void createStandardTexShader();
	void createStandardBillboardShader();
	void createStandardMultiTexShader();
	void createInstancedPhongShader();
	void createTextShader();

	CShader * createShader(std::string name);
	CShader * createShader(std::string name,   char** strings, int nFeedbacks);

	void backFaceCulling(bool on);
	void setVAO(GLuint newVAO);

	CCamera* createCamera(glm::vec3& pos);
	void setCurrentCamera(CCamera * camera);

	int Width; ///<Width of the view
	int Height; ///<Height of the view

	TMatrix DisplayMatrix; ///<A matrix storing whatever global translations, scaling, etc, we use every frame.

	C2dRenderer rend2d; ///<2D renderer.

	CTextureManagerOGL textureManager;

	CFontManager fontManager;

	CCamera* currentCamera;
	CCamera* defaultCamera; ///<Initially available camera.

	std::string dataPath;

	CBuf* screenQuad; ///<A re-usable quad for rendering to the entire screen.
	GLuint hFrameBuffer; ///<Handle for our internally used framebuffer.

	unsigned int tmpHandle;

	CShader* phongShader; ///<The standard phong shader
	CShader* texShader; ///<The standard texture shader
	CShader* billboardShader;
	CShader* textShader;
	CShader* textSpriteShader;

	std::vector<CShader*> shaderList; ///<Tracks all shaders

	//TO DO: phongshader handles, hopefully temp
	GLuint hNormalModelToCameraMatrix;
	GLuint hLightPosition;
	GLuint hLightSpecular;
	GLuint hLightDiffuse;
	GLuint hLightAmbient;
	GLuint hMatDiffuse;
	GLuint hMatAmbient;
	GLuint hMatSpecular;
	GLuint hMatShininess;
	GLuint hSample;
	GLuint hMVP;
	GLuint hModel;
	GLuint hView;

	//texture shader, hopefully temp
	GLuint hTexTexture;
	GLuint hTile;
	GLuint hOffset;
	GLuint hTexMVP;

	//text shader
	GLuint hTextTexture;
	GLuint hTextColour;
	GLuint hTextOrthoMatrix;
	GLuint hFadeInX;

	//billboard
	GLuint hBillTexture;
	GLuint hBillTile;
	GLuint hBillOffset;
	GLuint hBillMVP;
	GLuint hBillCentre; ///<Handle for positionHint of billboard centre.
	GLuint hBillCamWorldMatrix;
	GLuint hBillboardSize;


	CShader* multiTexShader; ///<The standard multi-texture shader
	GLuint hMultTextureUnit[16]; ///<Handle to standard texture shader texture units;
	GLuint hMultTile[16]; ///<Handle to standard texture shader texture tiling
	GLuint hMultOffset[16]; ///<Handle to standard texture shader texture tiling
	GLuint hMultMVP; ///<Handle to standard texture shader MVP matrix

	CShader* phongShaderInstanced;
	GLuint hPhongInstNormalModelToCameraMatrix;
	GLuint hPhongInstLightDirection;
	GLuint hPhongInstLightIntensity;
	GLuint hPhongInstAmbientLight;
	GLuint hPhongInstColour;
	GLuint hPhongInstMVP;

	GLuint hTimeQuery;

	rgba clearColour;

	bool initialised;

	glm::vec3 defaultLightPos;
	glm::vec3 defaultLightDir; //TO DO: scrap. Default light is not a spotlight!

	CRenderTexture* texture1x1; ///<1x1 white texture, useful as a default.



private:

	GLenum DrawBuffers[1];


	std::vector<GLuint> tmpShaderList; ///<Stores recently compiled shaders.

	//render to texture stuff:
	//unsigned int r2texQuadBuffer;
	unsigned int r2texFrameBuf;
	unsigned int  r2texTex;

	GLuint primitivesQuery; ///<Used in primitives generated queries.

	GLuint currentVAO; ///<The currently bound VAO. Used to avoid expensive rebinds. 
	
	std::vector<CBuf*> bufferList; ///<Kill list of renderer-created buffers.

	std::vector<CCamera*> cameraList; ///<Tracks renderer-created cameras.


};

//CRenderer* CRenderer::instance = 0;
const rgba rendererTurquiose = { 0.0f,0.7f,0.7f,0.5f };