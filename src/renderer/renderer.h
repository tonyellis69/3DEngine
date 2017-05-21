#pragma once

#include "2d.h"
#include <windows.h> 
#include <string>
#include <vector>

#include "..\soil.h"

//#include "model.h"
#include "camera.h"
#include "modelMulti.h"
#include "renderModel.h"
#include "buf.h"
#include "texManager.h"
#include "..\terrain.h"

#include "texShader.h"
#include "multiTexShader.h"
#include "phongShader.h"
#include "wireShader.h"
#include "billboardShader.h"

enum renderTextureFormat {floatTex,intTex, uintTex};

extern int totalbufsize;
extern int totalchunks;


class CRenderTerrain;

//class CModel;
//class CRenderModel;

/** The high-level renderer class. */
class CRenderer {
public:
	CRenderer();
	~CRenderer();
	void attachWindow(HWND& hWnd);
	void getGLinfo();
	void detachWindow();
	void clearFrame();
	void showFrame();
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
	void setShaderValue(unsigned int vecHandle, int elements, glm::vec3& vector);
	void setShaderValue(unsigned int vecHandle, int elements, glm::vec4& vector);
	void setShaderValue(unsigned int intHandle, int elements, int value);
	void setShaderValue(unsigned int floatHandle, int elements, float value);
	void setShader(int program);
	void setShader(CShader* shader);
	void drawModel(CRenderModel& model);
	void initRenderToTextureBufs();
	void initRenderToTexture(int w, int h, renderTextureFormat texType);
	void renderTo3DTexture(int shader, int w, int h, int d, float* buf);
	void renderTo2DTexture(int shader, int w, int h, int* buf);
	unsigned int getGeometryFeedback2(CModel& model, CBaseBuf& tmpBuf, CBaseBuf& destBuf);

	CBaseTexture* createDataTexture(renderTextureFormat dataType, int w, int h, const void* data);
	void uploadDataTexture(int hShader, int hTexture);
	void setDataTexture(unsigned int textureHandle);
	void setFeedbackData(int shader, int nVars, const char** strings);
	void initQuery();
	unsigned int query();
	void drawMultiModel(CModelMulti& model);
	void setDepthTest(bool on);
	void createTextureFromImageFile(std::string filename);
	void attachTexture(unsigned int textureUnit, unsigned int hTexture);


	int Width; ///<Width of the view
	int Height; ///<Height of the view

	HDC myhDC; ///<Handle for the window device context we're using.
	HGLRC myhRC; ///<Handle for the window rendering context we're using.

	TMatrix DisplayMatrix; ///<A matrix storing whatever global translations, scaling, etc, we use every frame.

	C2dRenderer rend2d; ///<2D renderer.

	CTextureManagerOGL textureManager;

	CCamera* currentCamera;

	std::string dataPath;
private:
	std::vector<GLuint> tmpShaderList; ///<Stores recently compiled shaders.

	//render to texture stuff:
	unsigned int r2texQuadBuffer;
	unsigned int r2texFrameBuf;
	unsigned int  r2texTex;

	GLuint primitivesQuery; ///<Used in primitives generated queries.


};