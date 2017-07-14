#pragma once

#include <string>

enum shaderType { vertex, frag, geometry };

enum TStandardShaderType { standardTex, standardPhong, standardWire, standardMultiTex, standardBillboard,
	uiRect, uiTex, userShader};

/**	Encapsulates a shader, to conveniently gather the various data handles it needs
	and also hide OpenGL specifics from user. */

class CShader {
public:
	CShader() { ident = userShader; };
	~CShader() {};
	virtual void load(shaderType shader, std::string shaderFile) {};
	virtual void create(std::string shaderName) {};
	virtual void attach() {};
	virtual void setFeedbackData(int nVars, const char** strings) {};
	virtual void link() {};
	virtual void getDataHandles() {};
	virtual unsigned int getShaderHandle() { return 0; };
	virtual CShader* getThisShader() { return 0; };
	virtual unsigned int getMVPhandle() { return 0; };
	virtual void recompile() {};
	TStandardShaderType getType();
	void setType(TStandardShaderType ident);
	virtual void getShaderHandles() {};

	TStandardShaderType ident;

	std::string vertFile;
	std::string geomFile;
	std::string fragFile;


};