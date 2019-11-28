#include "renderer.h"
//#include <windows.h> 
#include <glew.h>
#include <wglew.h>
#include "..\Macros.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/type_ptr.hpp>

#include "../watch.h"

#include "..\utils\log.h"

#define STB_IMAGE_IMPLEMENTATION
#include "..\external/stb_image.h"

//using namespace std;

//int totalbufsize = 0;
//int totalchunks = 0;


/** Initialise the high-level renderer. */
CRenderer::CRenderer() {
	initialised = false;
}

CRenderer::~CRenderer() {
	for (size_t c = 0; c < cameraList.size(); c++)
			delete cameraList[c];
	for (size_t b = 0; b<bufferList.size(); b++)
		delete bufferList[b];
	_CrtDumpMemoryLeaks();
}

void CRenderer::getGLinfo() {
	cerr << endl;
	GLenum err = glewInit();
	if (GLEW_OK != err)	{
		sysLog << "\nglewInit failed, error: %s\n" << glewGetErrorString(err);
		return;
	}
	sysLog << "\nUsing GLEW " << glewGetString(GLEW_VERSION);
	sysLog << "\nOpenGL version: " << glGetString(GL_VERSION);

	if(	(NULL == strstr( (char const*)glGetString( GL_EXTENSIONS ),
		"GL_ARB_texture_non_power_of_two" ) ) ) {	
		sysLog << "\nNo NPOT";
	} else {
		sysLog << "\nHas NPOT";
	}

	sysLog << "\nglsl version: " << glGetString(GL_SHADING_LANGUAGE_VERSION);
	sysLog << "\n";
}


/** Clear the frame using the current background colour. */
void CRenderer::clearFrame(){
	glClear(GL_COLOR_BUFFER_BIT| GL_DEPTH_BUFFER_BIT);
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
	getGLinfo();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE);;

	glClampColor(GL_CLAMP_VERTEX_COLOR, GL_FALSE);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	setTextureMode(true);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(0.0f, 1.0f);

//	initRenderToTextureBufs();
	glEnable(GL_PROGRAM_POINT_SIZE);

	//From now on, this is something we temporarily switch off, not temporarily switch on
	//index numbers are unlikely to rise high enough for this to be a problem, but be vigilant
	glEnable(GL_PRIMITIVE_RESTART); 
	glPrimitiveRestartIndex(65535);


	createScreenQuad();
	createFrameBuffer();

	currentVAO = -1;
	createStandardPhongShader();
	createStandardTexShader();
	createStandardBillboardShader();
	createStandardMultiTexShader();
	createInstancedPhongShader();
	createTextShader();
	initialised = true;

	clearColour = rendererTurquiose;
	setBackColour(clearColour);

	defaultCamera = createCamera(glm::vec3(0, 2, 4));
	setCurrentCamera(defaultCamera);

	//dummyTexture.resize(1, 1);
	//dummyTexture.setPixelData(&glm::vec4(1));
	static CRenderTexture dummyTexture;
	dummyTexture.resize(1, 1);
	unsigned char one[] = { 0,0,0,0 };
	dummyTexture.setPixelData(&one);
	texture1x1 = &dummyTexture;
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
		liveLog << alertMsg << "\nShader file " << shaderFilename << " not found.";
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
				liveLog << alertMsg << "\nInclude file " << incl << " not found.";
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

		liveLog << alertMsg << "\nCompile failure in " << shaderFilename;
		liveLog << strInfoLog;
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
		liveLog << alertMsg << "\nLinker failure:\n" <<  strInfoLog;
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

/** Store info on how to use given vertex data, eg, positionHint, colour, etc.*/
void CRenderer::storeVertexLayout(unsigned int& hVAO, unsigned int bufferObj, unsigned int hIndex, int nAttributes) {
	//GLuint vaoObject;
	if ( hVAO == 0)
		glGenVertexArrays(1, &hVAO);
	setVAO(hVAO);
		
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
	
	setVAO(0);

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
	unsigned int err = glGetError();
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
void  CRenderer::setShaderValue(unsigned int vecHandle, int elements, const glm::vec4& vector) {
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
	setVAO(model.buf.hVAO);
	if (model.buf.hIndex == 0)
		glDrawArrays(model.drawMode, 0, model.buf.noVerts);
	else
		glDrawElements(model.drawMode, model.buf.noIndices, model.buf.indexType, 0);
	setVAO(0);
}



float g_vertex_buffer_data[] = { 
	-1.0f, -1.0f,
	 1.0f, -1.0f,
	 -1.0f,  1.0f,
	 -1.0f, 1.0f,
	 1.0f, -1.0f,
	1.0f,  1.0f,
};

//TO DO: check if we can scrap this
/*
void CRenderer::initRenderToTextureBufs() {
	
	glGenBuffers(1, &r2texQuadBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, r2texQuadBuffer);
	
	//create a simple quad that will fill clipspace.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

  //get pointer to positionHint attribute
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
*/

/** Create an internal frameBuffer so  we can render to a texture when needed. */
void CRenderer::createFrameBuffer() {
	glGenFramebuffers(1, &hFrameBuffer);
}

/** Create a reusable quad in NDC coordinated for when we want to render to the entire screen, */
void CRenderer::createScreenQuad() {
	screenQuad = (CBuf*)createBuffer();
	vBuf::T2DtexVert vert[4];
	//glm::vec2 vert[4];
	vert[0].v = glm::vec2(-1, 1);
	vert[1].v = glm::vec2(1, 1);
	vert[2].v = glm::vec2(-1, -1);
	vert[3].v = glm::vec2(1, -1);

	vert[0].tex = glm::vec2(0,1);
	vert[1].tex = glm::vec2(1, 1);
	vert[2].tex = glm::vec2(0, 0);
	vert[3].tex = glm::vec2(1, 0);

	unsigned int index[4] = { 2,3,0,1 };
	screenQuad->storeVertexes(vert, sizeof(vert), 4);
	screenQuad->storeIndex(index, 4);
	screenQuad->storeLayout(2, 2, 0, 0);
}

/** Draw a full-screen quad to the given texture using the current shader. */
void CRenderer::renderToTextureQuad(CBaseTexture& texture) {
	beginRenderToTexture(texture);
	drawBuf(*screenQuad, drawTriStrip);
	endRenderToTexture();
}

/** Draw the given buffer of tris to the texture using the current shader. */
void CRenderer::renderToTextureTris(CBuf& buffer, CBaseTexture& texture) {
	beginRenderToTexture(texture);
	drawBuf(buffer, drawTris);
	endRenderToTexture();
}

void CRenderer::renderToTexturePoints(CBuf& buffer, CBaseTexture& texture) {
	beginRenderToTexture(texture);
	drawBuf(buffer, drawPoints);
	endRenderToTexture();
}


/**	Prepare to render to the given texture, leaving the actual drawing to the user. */
//TO DO: see if there's any overhead in deleting/regenerating FBO
//and maybe only calling drawbuffers once
//ANSWER: no overhead, drivers seem to check for this stuff.
void CRenderer::beginRenderToTexture(CBaseTexture& texture) {
	
	
	CRenderTexture* glTex = (CRenderTexture*)&texture;
	glDisable(GL_BLEND); //Otherwise this messes up text texture alpha
	glDisable(GL_SCISSOR_TEST);

	glGenFramebuffers(1, &hFrameBuffer);
	
	glBindFramebuffer(GL_FRAMEBUFFER, hFrameBuffer);
	
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, glTex->handle, 0);

	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		liveLog << alertMsg << "\nError creating framebuffer.";
		return;
	}
	glViewport(0, 0, glTex->width, glTex->height); // Render on the whole framebuffer, complete from the lower left corner to the upper right.    

}

/**	Clean up after rendering to a texture. */
void CRenderer::endRenderToTexture() {
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &hFrameBuffer);
	glViewport(0, 0, Width, Height);
	glEnable(GL_BLEND);
	glEnable(GL_SCISSOR_TEST);
}

void CRenderer::rendertToTextureClear(CBaseTexture& texture,glm::vec4& colour) {
	beginRenderToTexture(texture);
	rgba clr = { colour.r,colour.g, colour.b, colour.a };
	setBackColour((rgba&)colour);
	glClear(GL_COLOR_BUFFER_BIT);
	endRenderToTexture();
	setBackColour(clearColour);
}

/** Draw the model with the current shader, offscreen, and store the returned vertex data
	in a buffer. */
unsigned int CRenderer::getGeometryFeedback(CBuf& srcBuf, TdrawMode srcDrawMode, CBuf& destBuf, TdrawMode destDrawMode) {

	
	glEnable(GL_RASTERIZER_DISCARD);

	//glEnable(GL_PRIMITIVE_RESTART);


	GLint elapsed = 0;
	GLuint query,  primitives;
//	GLuint speedQuery;

	

	glGenQueries(1, &query);
	//glGenQueries(1, &speedQuery);

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, destBuf.getBufHandle());
	//glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, hDestBuf);

	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);
	//glBeginQuery(GL_TIME_ELAPSED, speedQuery);

	glBeginTransformFeedback(getGLdrawMode(destDrawMode));
	drawBuf( srcBuf, srcDrawMode);
	glEndTransformFeedback();

	//	glEndQuery(GL_TIME_ELAPSED);
	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

	glGetQueryObjectuiv(query, GL_QUERY_RESULT, &primitives);
	//	glGetQueryObjectiv(speedQuery,GL_QUERY_RESULT,&elapsed);

	//glDisable(GL_PRIMITIVE_RESTART);
	

		glDisable(GL_RASTERIZER_DISCARD);

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


void CRenderer::initTimerQuery() {
	//glFinish();
	glGenQueries(1, &hTimeQuery);
	glBeginQuery(GL_TIME_ELAPSED, hTimeQuery);
}

unsigned long CRenderer::getTimerQuery() {
	glEndQuery(GL_TIME_ELAPSED);
	GLuint64 timeElapsed;
	int done = 0;
	while (!done) {
	//	cerr << "*";
		glGetQueryObjectiv(hTimeQuery,
			GL_QUERY_RESULT_AVAILABLE,
			&done);
	}
	glGetQueryObjectui64v(hTimeQuery, GL_QUERY_RESULT, &timeElapsed);
	return (unsigned long)timeElapsed; 
}



/** Draw a range of geometry from the given childbuffer of a multibuffer. */
void CRenderer::drawMultiBufChildVerts(TdrawMode drawMode, CMultiBuf & multiBuf, int childBufNo, unsigned int vertStart, unsigned int vertCount) {
	//setVAO(multiBuf.childBufs[childBufNo].hVAO);
	glDrawArrays(getGLdrawMode(drawMode), vertStart, vertCount);

}

/** Draw an instanced model using instancing data from the given childbuffer of a multibuffer. */
void CRenderer::drawMultiBufChildInstanced(TdrawMode drawMode, CMultiBuf & multiBuf, int childBufNo, unsigned int vertStart, unsigned int vertCount) {
	setVAO(multiBuf.childBufs[childBufNo].hVAO);
	int nIndices = multiBuf.instancedBuf->getNoIndices();
	glDrawElementsInstancedBaseInstance(getGLdrawMode(drawMode), nIndices, multiBuf.indexType, 0,
		vertCount, vertStart);
//	cerr << "\nnIndices " << nIndices << " vertCount" << vertCount << " vertStart " << vertStart;
} 



void CRenderer::setDepthTest(bool on) {
	
	if (on) {
		glEnable(GL_DEPTH_TEST);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}
}

/*
void CRenderer::createTextureFromImageFile(std::string filename) {
	int width, height, channels;
	unsigned char* data = SOIL_load_image(filename.c_str(), &width, &height, &channels, SOIL_LOAD_AUTO); //load it to get the width, height, etc
}*/

void CRenderer::attachTexture(unsigned int textureUnit, unsigned int hTexture) {
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, hTexture);
}

void CRenderer::attachTexture1D(unsigned int textureUnit, unsigned int hTexture) {
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_1D, hTexture);
}

void CRenderer::attachTexture(unsigned int textureUnit, CBaseTexture & texture) {
	CRenderTexture& renderTexture = (CRenderTexture&)texture;
	glActiveTexture(GL_TEXTURE0 + textureUnit);
	glBindTexture(GL_TEXTURE_2D, renderTexture.handle);
}




void CRenderer::drawBuf(CBuf & buf, TdrawMode drawMode) {

	setVAO(buf.hVAO);
	if (buf.hIndex == 0)
		glDrawArrays(getGLdrawMode(drawMode), 0, buf.noVerts);
	else
		glDrawElements(getGLdrawMode(drawMode), buf.noIndices, buf.indexType, 0);
	setVAO(0);
}

/////!!!!Prefer this kind of specialised drawing command from now on.
/////The decision of which to use should be taken higher up 
/** Draw a buffer of line loop vertices using the current shader. */
void CRenderer::drawLineLoopBuf(CBuf& buf) {
	setVAO(buf.hVAO);
	glDrawElements(GL_LINE_LOOP, buf.noIndices, buf.indexType, 0);
	setVAO(0);
}

void CRenderer::drawLineStripBuf(CBuf& buf) {
	setVAO(buf.hVAO);
	glDrawElements(GL_LINE_STRIP, buf.noIndices, buf.indexType, 0);
	setVAO(0);
}

void CRenderer::drawLinesBuf(CBuf& buf) {
	setVAO(buf.hVAO);
	glDrawElements(GL_LINES, buf.noIndices, buf.indexType, 0);
	setVAO(0);
}

void CRenderer::drawTriStripBuf(CBuf& buf) {
	setVAO(buf.hVAO);
	glDrawElements(GL_TRIANGLE_STRIP, buf.noIndices, buf.indexType, 0);
	setVAO(0);
}

unsigned int CRenderer::getGLdrawMode(TdrawMode iDrawMode) {
	if (iDrawMode == drawPoints)
		return rDrawPoints;
	if (iDrawMode == drawLines)
		return rDrawLines;
	if (iDrawMode == drawTris)
		return rDrawTris;
	if (iDrawMode == drawLinesAdjacency)
		return rDrawLinesAdjacency;
	if (iDrawMode == drawLinesStrip)
		return rDrawLineStrip;
	if (iDrawMode == drawTriStrip)
		return rDrawTriStrip;
	if (iDrawMode == drawLineLoop)
		return rDrawLineLoop;
	return NULL;
}

CBaseBuf* CRenderer::createBuffer() {
	CBuf* newBuf = new CBuf();
	//newBuf->setRenderer(this);
	bufferList.push_back(newBuf);
	return newBuf;
}

void CRenderer::createStandardPhongShader() {
	phongShader = createShader(dataPath + "default");
	phongShader->setType(standardPhong);

	setShader(phongShader);
	hNormalModelToCameraMatrix = phongShader->getUniformHandle("normalModelToCameraMatrix");
	hLightPosition = phongShader->getUniformHandle("light.position");
	hLightSpecular = phongShader->getUniformHandle("light.specular");
	hLightDiffuse = phongShader->getUniformHandle("light.diffuse");
	hLightAmbient = phongShader->getUniformHandle("light.ambient");
	hMatDiffuse = phongShader->getUniformHandle("material.diffuse");
	hMatAmbient = phongShader->getUniformHandle("material.ambient");
	hMatSpecular = phongShader->getUniformHandle("material.specular");
	hMatShininess = phongShader->getUniformHandle("material.shininess");
	hSample = phongShader->getUniformHandle("sample");
	hMVP = phongShader->getUniformHandle("mvpMatrix");
	hModel = phongShader->getUniformHandle("model");;
	hView = phongShader->getUniformHandle("view");;

	//some default settings
	phongShader->setShaderValue(hLightSpecular, glm::vec4(0.8f, 0.8f, 0.8f, 1));
	phongShader->setShaderValue(hLightDiffuse, glm::vec4(0.8f, 0.8f, 0.8f, 1));
	phongShader->setShaderValue(hLightAmbient, glm::vec4(0.2f, 0.2f, 0.2f, 1));
	phongShader->setShaderValue(hLightPosition, defaultLightPos);
	phongShader->setShaderValue(hMatShininess, 32.0f);
	phongShader->setShaderValue(hMatDiffuse, glm::vec4(1));
	phongShader->setShaderValue(hMatAmbient, glm::vec4(1));
	phongShader->setShaderValue(hMatSpecular, glm::vec4(0.5f, 0.5f, 0.5f, 1));
}

//The default phong shader draw. *//
 void CRenderer::phongDrawCallout(void* callee, CModel2* model) {
	 CRenderer* pThis = &CRenderer::getInstance();
	 pThis->setShader(pThis->phongShader);
	 pThis->phongShader->setShaderValue(pThis->hLightPosition, pThis->defaultLightPos);
	 pThis->phongShader->setShaderValue(pThis->hLightSpecular, glm::vec4(0.8f, 0.8f, 0.8f, 1));
	 pThis->phongShader->setShaderValue(pThis->hLightDiffuse, glm::vec4(0.5f, 0.5f, 0.5f, 1));
	 pThis->phongShader->setShaderValue(pThis->hLightAmbient, glm::vec4(0.2f, 0.2f, 0.2f, 1));
	 pThis->phongShader->setShaderValue(pThis->hMatDiffuse, model->material.diffuse);
	 pThis->phongShader->setShaderValue(pThis->hMatAmbient, model->material.ambient);
	pThis-> phongShader->setShaderValue(pThis->hMatSpecular, model->material.specular);
	 pThis->phongShader->setShaderValue(pThis->hMatShininess, model->material.shininess);


	 pThis->attachTexture(0, model->material.texture1->handle);
	 pThis->phongShader->setShaderValue(pThis->hSample, 0);
	 pThis->phongShader->setShaderValue(pThis->hMVP, pThis->currentCamera->clipMatrix * model->worldMatrix);
	 pThis->phongShader->setShaderValue(pThis->hModel, model->worldMatrix);
	 pThis->phongShader->setShaderValue(pThis->hView, pThis->currentCamera->camMatrix);
	 pThis->drawBuf(model->buffer, drawTris);
	 pThis->attachTexture(0, 0);
}

void CRenderer::createStandardTexShader() {
	texShader = createShader(dataPath + "texture");
	texShader->setType(standardTex);

	setShader(texShader);
	hTexTexture = texShader->getUniformHandle("mySampler");
	hTile = texShader->getUniformHandle("tile");
	hOffset = texShader->getUniformHandle("offset");
	hTexMVP = texShader->getUniformHandle("mvpMatrix");
}

void CRenderer::createStandardBillboardShader() {
	billboardShader = createShader(dataPath + "billboard");
	billboardShader->setType(standardBillboard);

	setShader(billboardShader);
	hBillTexture = billboardShader->getUniformHandle("mySampler");
	hBillTile = billboardShader->getUniformHandle("tile");
	hBillOffset = billboardShader->getUniformHandle("offset");
	hBillMVP = billboardShader->getUniformHandle("mvpMatrix");
	hBillCentre = billboardShader->getUniformHandle("centrePos");
	hBillCamWorldMatrix = billboardShader->getUniformHandle("camWorldMatrix");
	hBillboardSize = billboardShader->getUniformHandle("size");
}

void CRenderer::createStandardMultiTexShader() {
	multiTexShader = createShader(dataPath + "multiTexture");
	multiTexShader->setType(standardMultiTex);
	setShader(multiTexShader);
	
	hMultTextureUnit[0] = multiTexShader->getUniformHandle("mySampler1");
	hMultTextureUnit[1] = multiTexShader->getUniformHandle("mySampler2");
	hMultTile[0] = multiTexShader->getUniformHandle("tile1");
	hMultTile[1] = multiTexShader->getUniformHandle("tile2");
	hMultOffset[0] = multiTexShader->getUniformHandle("offset1");
	hMultOffset[1] = multiTexShader->getUniformHandle("offset2");
	hMultMVP = multiTexShader->getUniformHandle("mvpMatrix");
}

void CRenderer::createInstancedPhongShader() {
	phongShaderInstanced = createShader(dataPath + "defaultInstanced");
	phongShaderInstanced->setType(standardPhong);
	setShader(phongShaderInstanced);
	hPhongInstNormalModelToCameraMatrix = phongShaderInstanced->getUniformHandle("normalModelToCameraMatrix");
	hPhongInstLightDirection = phongShaderInstanced->getUniformHandle("lightDirection");
	hPhongInstLightIntensity = phongShaderInstanced->getUniformHandle("lightIntensity");
	hPhongInstAmbientLight = phongShaderInstanced->getUniformHandle("ambientLight");
	hPhongInstColour = phongShaderInstanced->getUniformHandle("colour");
	hPhongInstMVP = phongShaderInstanced->getUniformHandle("mvpMatrix");

	phongShaderInstanced->setShaderValue(hPhongInstLightDirection, glm::normalize(glm::vec3(0.866f, 0.9f, 0.5f)));
	phongShaderInstanced->setShaderValue(hPhongInstLightIntensity, glm::vec4(0.8f, 0.8f, 0.8f, 1));
	phongShaderInstanced->setShaderValue(hPhongInstAmbientLight,glm::vec4(0.2f, 0.2f, 0.2f, 1));
	phongShaderInstanced->setShaderValue(hPhongInstColour,glm::vec4(1));
}

void CRenderer::createTextShader() {
	textShader = createShader(dataPath + "text");
	textShader->setType(text);
	setShader(textShader);
	hTextTexture = textShader->getUniformHandle("fontTexture");
	hTextColour = textShader->getUniformHandle("textColour");
	hTextOrthoMatrix = textShader->getUniformHandle("orthoMatrix");
	hFadeInX = textShader->getUniformHandle("fadeInX");
}

/** Compile the given shader, create a wrapper instance for it and return a pointer to it. */
CShader* CRenderer::createShader(std::string name) {
	CRenderShader* shader = new CRenderShader();
	if (name.compare(0, dataPath.size(), dataPath))
		name = dataPath + name;
	shader->create( name);
	shaderList.push_back(shader);
	return shader;
}

CShader * CRenderer::createShader(std::string name,  char** strings,int nFeedbacks) {
	CRenderShader* shader = new CRenderShader();
	for (int s=0; s < nFeedbacks; s++)
		shader->feedbackVaryings[s] = strings[s];
	shader->create(name, nFeedbacks);
	shaderList.push_back(shader);
	return shader;
}

void CRenderer::backFaceCulling(bool on) {
	if (on)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}

/** Bind the given Vertex Array Object, if it's not bound already. */
void CRenderer::setVAO(GLuint newVAO) {
	if (newVAO != currentVAO) {
		glBindVertexArray(newVAO);
		currentVAO = newVAO;
	}
}

CCamera * CRenderer::createCamera(glm::vec3 & pos) {
	CCamera* camera = new CCamera(pos);
	cameraList.push_back(camera);
	return camera;
}

void CRenderer::setCurrentCamera(CCamera * camera) {
	currentCamera = camera;
}

/** 
void CRenderer::drawMultiBufInstanced() {



} */
