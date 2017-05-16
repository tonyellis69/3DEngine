#include "renderer.h"
#include <windows.h> 
//#include <gl/gl.h>
#include <glew.h>
#include <wglew.h>
#include "..\Macros.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

#include "../watch.h"

using namespace std;

//int totalbufsize = 0;
//int totalchunks = 0;


/** Initialise the high-level renderer. */
CRenderer::CRenderer() {
	myhRC = NULL;
	
	

}

CRenderer::~CRenderer() {
	delete texShader;
	delete phongShader;
}

/** Attach the given window to the renderer, so it can be drawn to. */
void CRenderer::attachWindow(HWND& hWnd){
	//Windows-OpenGL setup stuff:
	//Set the Windows device context pixel format for the given window
	int nPixelFormat;

	static PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),	//size of this structure
		1,								//version
		PFD_DRAW_TO_WINDOW |			//draw to window (not bitmap)
		PFD_SUPPORT_OPENGL |			//support OpenGL calls
		PFD_DOUBLEBUFFER,				//double-buffered mode
		PFD_TYPE_RGBA,					//RGBA colour mode
		32,								//32 bit colour
		0,0,0,0,0,						//not used
		0,0,0,0,0,0,0,					//not used
		32,								//size of depth buffer
		0,0,							//not used
		PFD_MAIN_PLANE,					//draw in main plane
		0,0,0,0							//not used
	};

	myhDC = ::GetDC(hWnd); //get the window's device context 

	//choose pixel format that best matches the one described:
	nPixelFormat = ChoosePixelFormat(myhDC, &pfd);
	SetPixelFormat(myhDC, nPixelFormat, &pfd);

	ASSERT_ALWAYS(nPixelFormat, "Can't Find A Suitable PixelFormat.");
	ASSERT_ALWAYS(SetPixelFormat(myhDC,nPixelFormat,&pfd),"Can't Set The PixelFormat.");
   
	 myhRC = wglCreateContext(myhDC); //create rendering context for OpenGL
	ASSERT_ALWAYS(myhRC,"Can't Create A GL Rendering Context.");
	ASSERT_ALWAYS(wglMakeCurrent(myhDC, myhRC),"Can't activate GL Rendering Context.");
}



void CRenderer::getGLinfo() {
	GLenum err = glewInit();
	if (GLEW_OK != err)	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	fprintf(stderr, "\nStatus: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	const unsigned char *version = (const unsigned char *)glGetString(GL_VERSION);
	fprintf(stderr,"\nOpenGL version: %s.\n",version);

	if(	(NULL == strstr( (char const*)glGetString( GL_EXTENSIONS ),
		"GL_ARB_texture_non_power_of_two" ) ) ) {	
			fprintf(stderr,"No NPOT\n");
	} else {
		fprintf(stderr,"Has NPOT\n");
	}

/*	GLint attribs[] =
	{
    // Here we ask for OpenGL 2.1
    WGL_CONTEXT_MAJOR_VERSION_ARB, 2,
    WGL_CONTEXT_MINOR_VERSION_ARB, 1,
    // Uncomment this for forward compatibility mode
    //WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
    // Uncomment this for Compatibility profile
  //  WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
    // We are using Core profile here
   // WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,0
	};
*/
	int attribs[] =
{
	WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 2,
	//WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
	//WGL_CONTEXT_FLAGS_ARB, 0,
//	0
// WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
//    0
};

//	HGLRC CompHRC = wglCreateContextAttribsARB(myhDC, 0, attribs);
//	if (CompHRC && wglMakeCurrent(myhDC, CompHRC))
//		myhRC = CompHRC;

//	version = (const unsigned char *)glGetString(GL_VERSION);
//	fprintf(stderr,"\nOpenGL version: %s.\n",version);


}


/** Detach the connected window from our renderer, if any. */
void CRenderer::detachWindow(){
	ASSERT_ALWAYS(wglMakeCurrent(myhDC, NULL),"Can't deactivate GL Rendering Context.");
	wglDeleteContext(myhRC);
}

/** Clear the frame using the current background colour. */
void CRenderer::clearFrame(){
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
}

/** Display the current frame. */
void CRenderer::showFrame() {
	SwapBuffers(myhDC);	
}

/** Set the background colour. */
void CRenderer::setBackColour(const rgba& colour){
	glClearColor(colour.r,colour.g,colour.b,colour.a); 
}

/** Establish an orthographical viewing area on our window, with the given dimensions.*/
void CRenderer::set2DView(int x, int y,int width,int height){
	glViewport(x,y,width,height);
	Width = width;
	Height = height;
	glMatrixMode(GL_PROJECTION); //Sets field of view...
	glLoadIdentity();
	glOrtho(0,width,0,height,-1000,1000); //...which we want to be orthographic

	glEnable(GL_SCISSOR_TEST);
}


/** Reset the modelview matrix to the identity matrix, ie, free of all transformations. */ 
void CRenderer::resetMatrix() {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity(); 
}

/** Set up standard rendering paramaters. */
void CRenderer::init() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	setTextureMode(true);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

	initRenderToTextureBufs();

	glPrimitiveRestartIndex(65535);

	createStandardTexShader();
	createStandardPhongShader();
}


/** Set the renderer to draw with textures rather than colours.*/
void CRenderer::setTextureMode(bool enabled){
	if (enabled)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);
}

/** Set the texture we're drawing with to the given texure. */
void CRenderer::setCurrentTexture(int textureHandle) {
	glBindTexture(GL_TEXTURE_2D,textureHandle);
}

/** Save the current state of the modelview matrix. */
void CRenderer::saveDisplayMatrix() {
	glGetFloatv(GL_MODELVIEW_MATRIX,DisplayMatrix.M);
}

/** Load the saved matrix, which will usually contain any transformations used by the engine to fit drawing onscreen. */
void CRenderer::useDisplayMatrix() {
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(DisplayMatrix.M);
}

/** Push a copy of the current matrix onto a stack for quick retrieval. */
void CRenderer::pushMatrix() {
	glPushMatrix();
}

/** Restore the matrix to the version saved to the top of the stack. */
void CRenderer::popMatrix() {
	glPopMatrix();
}

/** Load the given shader onto the temporary shader list.*/
void CRenderer::loadShader(shaderType type, std::string shaderFilename) {
	std::ifstream shaderFile(shaderFilename.c_str());
	if (!shaderFile.is_open()) {
		cerr << "\nShader file " << shaderFilename << " not found.";
		return;
	}
	string path = shaderFilename.substr(0,shaderFilename.find_last_of("\\")+1);
	stringstream shaderData; string line, incl; size_t p;
	while (getline(shaderFile,line)) {
		p = line.find_first_not_of(" \t");
		if (p == string::npos)
			p = 0;
		if (line.compare(p,8,"#include") == 0) {
			incl = line.substr(p+9);
			incl = incl.substr(incl.find_first_not_of(" \t"));
			incl = path + incl;
			std::ifstream inclFile(incl.c_str());
			if (!inclFile.is_open()) {
				cerr << "\nInclude file " << incl << " not found.";
				return;
			}
			shaderData << inclFile.rdbuf();
			inclFile.close();
		}
		else
			shaderData << line << "\n";
	}

	shaderFile.close();
	string tmp; tmp = shaderData.str();
	const char* str = tmp.c_str();
	GLint shaderGL;
	switch(type){
		case vertex: shaderGL = GL_VERTEX_SHADER; break;
		case geometry: shaderGL = GL_GEOMETRY_SHADER; break;
		case frag: shaderGL = GL_FRAGMENT_SHADER; break;
	}
	GLuint shader = glCreateShader(shaderGL);
	glShaderSource(shader, 1, &str, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		cerr << "\nCompile failure in " << shaderFilename;
		cerr << strInfoLog;
		delete[] strInfoLog;
	}
	else
		tmpShaderList.push_back(shader); 
}

/** Link shaders on the temporary list into a program. */
unsigned int CRenderer::linkShaders(unsigned int program) {

	
	glLinkProgram(program);
	

	GLint status;
	glGetProgramiv (program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar *strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
		cerr << "\n Linker failure:\n" <<  strInfoLog;
		delete[] strInfoLog;
		return NULL;
	}

	return program;
}

unsigned int CRenderer::linkShaders() {
	GLuint program = attachShaders();
	return linkShaders(program);
}

unsigned int CRenderer::attachShaders() {
	GLuint program = glCreateProgram();
	for(size_t iLoop = 0; iLoop < tmpShaderList.size(); iLoop++)
		glAttachShader(program, tmpShaderList[iLoop]);
	tmpShaderList.clear();
	return program;
}

/** Store the given vertex data in a buffer and return a handle.*/
void CRenderer::storeVertexData(unsigned int& handle, glm::vec3* data,int size) {
	if (handle ==0) {
		glGenBuffers(1, &handle); //TO DO: generate buffer name *only* if it's still 0
	}
	glBindBuffer(GL_ARRAY_BUFFER, handle);
	glBufferData(GL_ARRAY_BUFFER,  size, (void*)data, GL_DYNAMIC_DRAW); //was static
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/** Store the given index data in a buffer and return a handle.*/
void CRenderer::storeIndexData(unsigned int& hIndex, unsigned short* data,int size) {
	if (hIndex == 0)
		glGenBuffers(1, &hIndex);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIndex);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, (void*)data, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	
}

/** Store info on how to use given vertex data, eg, position, colour, etc.*/
void CRenderer::storeVertexLayout(unsigned int& hVAO, unsigned int bufferObj, unsigned int hIndex, int nAttributes) {
	//GLuint vaoObject;
	if ( hVAO == 0)
		glGenVertexArrays(1, &hVAO);
    glBindVertexArray(hVAO);
        
    glBindBuffer(GL_ARRAY_BUFFER, bufferObj);



	if (nAttributes == 3) {
		int stride = (sizeof(glm::vec3) * 2) + sizeof(glm::vec4);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,(void*) 0);


		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride,(void*) (sizeof(glm::vec3) ));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride,(void*) (sizeof(glm::vec3) + sizeof(glm::vec4) ));
	}
	else {

			//establish first attribute - the vector itself
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),(void*) 0);
	}


	/*for (int attrib = 0;attrib < nAttributes; attrib++) {
		glEnableVertexAttribArray(attrib);
		glVertexAttribPointer(attrib, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) * nAttributes,(void*) (sizeof(glm::vec3) * (attrib)));
	}*/

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, hIndex);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindVertexArray(0);

}

/** Delete the given buffer, freeing up the memory it uses.*/
void CRenderer::freeBuffer(unsigned int buffer) {

	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	glDeleteBuffers(1,&buffer);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CRenderer::freeVAO(unsigned int hVAO){
	glDeleteVertexArrays(1,&hVAO);
}

/** Prepare the given shader for use as a standard drawing shader.*/
void CRenderer::acquireDataLocations(int program) {
	modelToWorldMatrix  = glGetUniformLocation(program, "modelToWorldMatrix");
	cameraToClipMatrix = glGetUniformLocation(program, "cameraToClipMatrix");
	normalModelToCameraMatrix = glGetUniformLocation(program, "normalModelToCameraMatrix");
	mvpMatrix = glGetUniformLocation(program, "mvpMatrix");
	lightDirection = glGetUniformLocation(program, "lightDirection");
	lightIntensity = glGetUniformLocation(program, "lightIntensity");
	ambientLight = glGetUniformLocation(program, "ambientLight");
	hColour = glGetUniformLocation(program, "colour");
}
/** Get a handle for the given variable used in the given shader. */
unsigned int CRenderer::getShaderDataHandle(int program, std::string varName) {
	GLuint error = 0;
	unsigned int x =  glGetUniformLocation(program, varName.c_str());
	error = glGetError();
	return x;
}



/** Use the given matrix as directed with the current shader. */
void  CRenderer::setShaderValue(unsigned int matrixHandle, int elements, glm::mat4& matrix) {
	glUniformMatrix4fv(matrixHandle, elements, GL_FALSE, glm::value_ptr(matrix));
}

void CRenderer::setShaderValue(unsigned int matrixHandle, int elements, glm::mat3& matrix) {
	glUniformMatrix3fv(matrixHandle, elements, GL_FALSE, glm::value_ptr(matrix));
}


void CRenderer::setShaderValue(unsigned int vecHandle, int elements, glm::vec2 & vector){
	glUniform2fv(vecHandle, elements, glm::value_ptr(vector));
}

/** Use the given uniform vector with the current shader. */
void  CRenderer::setShaderValue(unsigned int vecHandle, int elements, glm::vec3& vector) {
	glUniform3fv(vecHandle, elements, glm::value_ptr(vector));
}
void  CRenderer::setShaderValue(unsigned int vecHandle, int elements, glm::vec4& vector) {
	glUniform4fv(vecHandle, elements, glm::value_ptr(vector));
}

void  CRenderer::setShaderValue(unsigned int intHandle, int elements, int value) {
	glUniform1i(intHandle,value);
}

void CRenderer::setShaderValue(unsigned int floatHandle, int elements, float value) {
	glUniform1f(floatHandle,value);
}



void CRenderer::setShader(int program) {
	glUseProgram(program);

}

void CRenderer::setShader(CShader * shader) {
	glUseProgram(shader->getShaderHandle());
}

void CRenderer::drawModel(CRenderModel& model) {
	glBindVertexArray(model.buf.hVAO);
	if (model.buf.hIndex == 0)
		glDrawArrays(model.drawMode, 0, model.buf.noVerts);
	else
		glDrawElements(model.drawMode, model.buf.noIndices, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}

float g_vertex_buffer_data[] = { 
    -1.0f, -1.0f,
     1.0f, -1.0f,
     -1.0f,  1.0f,
	 -1.0f, 1.0f,
	 1.0f, -1.0f,
	1.0f,  1.0f,
};

void CRenderer::initRenderToTextureBufs() {
	
    glGenBuffers(1, &r2texQuadBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, r2texQuadBuffer);
	
	//create a simple quad that will fill clipspace.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

  //get pointer to position attribute
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,2, GL_FLOAT, GL_FALSE, 0,(void*) 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//create framebuffer
	glGenFramebuffers(1, &r2texFrameBuf);
	glBindFramebuffer(GL_FRAMEBUFFER, r2texFrameBuf);

	//create texture
	glGenTextures(1, &r2texTex);

	glBindFramebuffer(GL_FRAMEBUFFER,0); //do this for legacy compatibility
	glDisableVertexAttribArray(0);	//do this for legacy compatibility
}


/** Do the necessary preparation for a 3D render to texture.*/
void CRenderer::initRenderToTexture(int w, int h, renderTextureFormat texType) {

	// "Bind" the render texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, r2texTex);

	glEnableVertexAttribArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, r2texFrameBuf);

 
	// Give an empty image to OpenGL
	if ( texType == floatTex)
		glTexImage2D(GL_TEXTURE_2D, 0,GL_R32F, w, h, 0,GL_RED, GL_FLOAT, 0);
	else if ( texType == uintTex)
		glTexImage2D(GL_TEXTURE_2D, 0,GL_R32UI, w, h, 0,GL_RED_INTEGER, GL_UNSIGNED_INT, 0);
	else if ( texType == intTex)
		glTexImage2D(GL_TEXTURE_2D, 0,GL_R32I, w, h, 0,GL_RED_INTEGER, GL_INT, 0);
 
	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//configure framebuffer with texture
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, r2texTex, 0);
 
	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	// Always check that our framebuffer is ok
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cerr << "\nError creating framebuffer.";
		return ;
	}

	//attach framebuffer for use
	glBindFramebuffer(GL_FRAMEBUFFER, r2texFrameBuf);
	glViewport(0,0,w,h); // Render on the whole framebuffer, complete from the lower left corner to the upper right.    
	glBindBuffer(GL_ARRAY_BUFFER, r2texQuadBuffer);
}

/** Render a block of pixels to a buffer using the given shader, one layer at a time. */
void CRenderer::renderTo3DTexture(int shader, int w, int h, int d,float* buf) {
	initRenderToTexture(w,d,floatTex);
	int sliceUni = glGetUniformLocation(shader, "slice");

	for (int slice=0; slice<h; slice++) {
		glUniform1i(sliceUni,slice);
		glDrawArrays(GL_TRIANGLES,0,6);
  		glReadPixels(0,0,w,d,GL_RED ,GL_FLOAT,buf + (slice * w * d ));
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glDisableVertexAttribArray(0);	//do this for legacy compatibility

	glViewport(0,0,Width,Height);
	
}

/** Render a 2D array of pixels to a buffer using the given shader. */
void CRenderer::renderTo2DTexture(int shader, int w, int h, int* buf) {
	initRenderToTexture(w,h,uintTex);
	glDrawArrays(GL_TRIANGLES,0,6);
  	glReadPixels(0,0,w,h,GL_RED_INTEGER ,GL_UNSIGNED_INT,buf );
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glViewport(0,0,Width,Height);
}


/** Draw the model with the current shader, offscreen, and store the returned vertex data
	in a buffer. */
unsigned int CRenderer::getGeometryFeedback2(CModel& model, CBaseBuf& tmpBuf, CBaseBuf& destBuf) {

	
	glEnable(GL_RASTERIZER_DISCARD);
	glEnable(GL_PRIMITIVE_RESTART);

	GLint elapsed = 0;
	GLuint query, speedQuery, primitives;

	

	glGenQueries(1, &query);
	//glGenQueries(1, &speedQuery);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, tmpBuf.getBufHandle());
	//glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, hDestBuf);

	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);
	//glBeginQuery(GL_TIME_ELAPSED, speedQuery);


	glBeginTransformFeedback(GL_TRIANGLES);

	//drawModel(model);
	//model.drawNew();
	drawModel((CRenderModel&)model);

	glEndTransformFeedback();


	//	glEndQuery(GL_TIME_ELAPSED);
	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);


	glGetQueryObjectuiv(query, GL_QUERY_RESULT, &primitives);
	//	glGetQueryObjectiv(speedQuery,GL_QUERY_RESULT,&elapsed);

	glDisable(GL_PRIMITIVE_RESTART);
	if (primitives == 0) {

		glDisable(GL_RASTERIZER_DISCARD);

		//empty the internal buffer
		//freeBuffer(tbo);
	//	tmpBuf.freeMem();
		return 0; //TO DO fix should not happen
	}


	int outSize = primitives * 3 * sizeof(vBuf::T3DnormVert);
	totalbufsize += outSize;
	totalchunks++;


	//verts are in our temporary feedback buffer. Now we want to copy them to the user-supplied buffer, which may or may not be a multibuffer
	//if it is a multibuffer, we want to say "here's this block of verts, find a space for it." So, some kind of copyBuffer method.
	destBuf.copyBuf(tmpBuf,outSize);

	
	glDisable(GL_RASTERIZER_DISCARD);


	//	cerr << "\nChunk took: " << elapsed << " by query timer";
	//tmpBuf.freeMem();
	return primitives;
}

void CRenderer::uploadDataTexture(int hShader, int hTexture) {
	//TO DO: use hTexture to find texture unit
	GLuint texUnit = 0;
	glUniform1i(hShader,texUnit); 
}


CBaseTexture* CRenderer::createDataTexture(renderTextureFormat dataType, int w, int h, const void* data) {
	CRenderTexture* dataTexture = (CRenderTexture*) textureManager.createTextureObject();

	//NEXT: return an agnostic reference to the data texture for the  user
	//index no? Pointer? String?
	//user uses this to upload data texture to shader


	//glActiveTexture(GL_TEXTURE0);

	/* TO DO: this is a fudge. To use more than one data texture at once, glActiveTexture(GL_TEXTUREn) is needed,
	as above. The data texture handle will need to be used with its texture unit number. 
	This could be done by having an intenal map that associates each unique texture handle with its texture
	unit number.
		Alternatively, if imageLib becomes more sophisticated and can handle multitextures, this book-keeping
	can be handled there to avoid repetition. */


	glBindTexture(GL_TEXTURE_2D, dataTexture->handle);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	if (dataType == intTex)
		glTexImage2D(GL_TEXTURE_2D, 0,GL_R32I, w, h, 0,GL_RED_INTEGER, GL_INT, data);
	else if ( dataType == floatTex)
		glTexImage2D(GL_TEXTURE_2D, 0,GL_R32F, w, h, 0,GL_RED, GL_FLOAT, data);
	else if ( dataType == uintTex)
		glTexImage2D(GL_TEXTURE_2D, 0,GL_R32UI, w, h, 0,GL_RED_INTEGER, GL_UNSIGNED_INT, data);



	return dataTexture;
}


void CRenderer::setDataTexture(unsigned int textureHandle) {
	glBindTexture(GL_TEXTURE_2D,textureHandle);
	/* TO DO: this is a fudge. To use more than one data texture at once, glActiveTexture(GL_TEXTUREn) is needed.
	The data texture handle will need to be used with its texture unit number. And the texture unit reset to
	0 afterwards. See createDataTexture.*/

}

void CRenderer::setFeedbackData(int shader, int nVars, const char** strings) {
	
	glTransformFeedbackVaryings(shader, nVars, strings, GL_INTERLEAVED_ATTRIBS);

}

/** Prepare to run a query such as number of primitives generated. */
void CRenderer::initQuery() {
	glGenQueries(1, &primitivesQuery);
	glBeginQuery(GL_PRIMITIVES_GENERATED, primitivesQuery);
	//TO DO: if needed, open this up to other queries
}

/** End a query, and return the result. */
unsigned int CRenderer::query() {
	glEndQuery(GL_PRIMITIVES_GENERATED);
	GLuint primitives;
	glGetQueryObjectuiv(primitivesQuery, GL_QUERY_RESULT, &primitives);
	glDeleteQueries(1,&primitivesQuery);
	return primitives;
}



void CRenderer::drawMultiModel(CModelMulti & model) {
	glBindVertexArray(model.multiBuf.childBufs[0].hVAO);
	CChildBuf* childBuf; 
	for (int child = 0; child <  model.multiBuf.noChildBufs ; child++) {
		childBuf = &model.multiBuf.childBufs[child];
		glBindVertexArray(model.multiBuf.childBufs[child].hVAO);
		for (int object = 0; object < childBuf->objCount ; object++) {
			//setShaderValue(hColour, 1, childBuf->colour[object]);
			phongShader->setColour(childBuf->colour[object]);
			glDrawArrays(GL_TRIANGLES, childBuf->first[object], childBuf->count[object]);
		}
	}	
}

void CRenderer::setDepthTest(bool on) {
	
	
	if (on) {
	//	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	//	glDepthFunc(GL_LEQUAL);
	//glClear(GL_DEPTH_BUFFER_BIT);

	}
	else {
//glClear(GL_DEPTH_BUFFER_BIT);
	//glDepthMask(GL_FALSE) ;
		glDisable(GL_DEPTH_TEST);
	//	glClear(GL_DEPTH_BUFFER_BIT);
		//glDepthFunc(GL_NEVER);
	}
}

void CRenderer::createTextureFromImageFile(std::string filename) {
	int width, height, channels;
	unsigned char* data = SOIL_load_image(filename.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO); //load it to get the width, height, etc


}

void CRenderer::attachTexture(unsigned int textureUnit, unsigned int hTexture) {
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, hTexture);
}

void CRenderer::createStandardTexShader() {
	texShader = new CTexShader();
	texShader->pRenderer = this;
	texShader->create(dataPath + "texture");
	texShader->getShaderHandles();
	texShader->setType(standardTex);
}

void CRenderer::createStandardPhongShader() {
	phongShader = new CPhongShader();
	phongShader->pRenderer = this;
	phongShader->create(dataPath + "default");
	phongShader->getShaderHandles();
	phongShader->setType(standardPhong);

	setShader(phongShader);
	phongShader->setLightDirection(glm::normalize(glm::vec3(0.866f, 0.9f, 0.5f)));
	phongShader->setLightIntensity(glm::vec4(0.8f, 0.8f, 0.8f,1));
	phongShader->setAmbientLight(glm::vec4(0.2f, 0.2f, 0.2f, 1));
	phongShader->setColour(glm::vec4(1));
}





