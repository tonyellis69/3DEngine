#pragma once

#include <string>

enum shaderType { vertex, frag, geometry };

enum TStandardShaderType { standardTex, standardPhong, userShader};

/**	Encapsulates a shader, to conveniently gather the various data handles it needs
	and also hide OpenGL specifics from user. */

class CShader {
public:
	CShader() {};
	~CShader() {};
	virtual void load(shaderType shader, std::string shaderFile) {};
	virtual void create(std::string shaderName) {};
	virtual void link() {};
	virtual void getDataHandles() {};
	virtual unsigned int getShaderHandle() { return 0; };
	virtual CShader* getThisShader() { return 0; };
	virtual unsigned int getMVPhandle() { return 0; };
	TStandardShaderType getType();
	void setType(TStandardShaderType ident);

	TStandardShaderType ident;
};