#pragma once

#include "2d.h"
#include <windows.h> 
#include <string>
#include <vector>

#include "model.h"
#include "buf.h"
#include "..\terrain.h"

enum shaderType {vertex,frag,geometry };

enum renderTextureFormat {floatTex,intTex, uintTex};

extern int totalbufsize;
extern int totalchunks;


class CRenderTerrain;

class CModel;

/** The high-level renderer class. */
class CRenderer {
public:
	CRenderer();
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
	void acquireDataLocations(int program);
	int getShaderDataHandle(int program, std::string varName);
	void setShaderValue(int matrixHandle,glm::mat4& matrix); 
	void setShaderValue(int matrixHandle,glm::mat3& matrix); 

	void setShaderValue(int vecHandle,glm::vec3& vector);
	void setShaderValue(int vecHandle,glm::vec4& vector);
	void setShaderValue(int intHandle,int value);
	void setShaderValue(int floatHandle, float value);
	void setShader(int program);
	void drawModel(CRenderModel& model);
	void initRenderToTextureBufs();
	void initRenderToTexture(int w, int h, renderTextureFormat texType);
	void renderTo3DTexture(int shader, int w, int h, int d, float* buf);
	void renderTo2DTexture(int shader, int w, int h, int* buf);
	unsigned int getGeometryFeedback(CModel& model, int size, int vertsPerPrimitive, unsigned int& hFeedBackBuf, unsigned int multiBufferOffset);
	unsigned int getGeometryFeedback2(CModel& model, unsigned int bufSize, CBaseBuf& destBuf);

	unsigned int createDataTexture(renderTextureFormat dataType, int w, int h, const void* data);
	void uploadDataTexture(int hShader, int hTexture);
	void setDataTexture(unsigned int textureHandle);
	void setFeedbackData(int shader, int nVars, const char** strings);
	void initQuery();
	unsigned int query();
	void drawMultiModel(CModelMulti& model);

	int Width; ///<Width of the view
	int Height; ///<Height of the view

	GLuint modelToWorldMatrix; ///<Handle to standard shader matrix location.
	GLuint cameraToClipMatrix; ///<Handle to standard shader matrix location.
	GLuint normalModelToCameraMatrix;
	GLuint mvpMatrix; ///<Handle to standard shader matrix location.
	GLuint lightDirection; ///<Handle to standard shader light vector.
	GLuint lightIntensity; 
	GLuint ambientLight;

	HDC myhDC; ///<Handle for the window device context we're using.
	HGLRC myhRC; ///<Handle for the window rendering context we're using.

	TMatrix DisplayMatrix; ///<A matrix storing whatever global translations, scaling, etc, we use every frame.

	C2dRenderer rend2d; ///<2D renderer.

	


private:
	std::vector<GLuint> tmpShaderList; ///<Stores recently compiled shaders.

	//render to texture stuff:
	unsigned int r2texQuadBuffer;
	unsigned int r2texFrameBuf;
	unsigned int  r2texTex;

	GLuint primitivesQuery; ///<Used in primitives generated queries.


};