#include "shader2.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glew.h>

#include "utils/log.h"

CShader::CShader(const std::string& shaderName) {
	auto tmpShaderList = loadFiles(shaderName);
	attachShaders(tmpShaderList);
	linkShaders();
}

void CShader::activate() {
	//TO DO: track active shader, set only if not already active.
	glUseProgram(handle);
}

unsigned int CShader::getUniform(const std::string& uniformName) {
	return glGetUniformLocation(handle, uniformName.c_str());
}

void CShader::setUniform(unsigned int matrixHandle, glm::mat4& matrix) {
	glUniformMatrix4fv(matrixHandle, 1, GL_FALSE, glm::value_ptr(matrix));
}

void CShader::setUniform(unsigned int matrixHandle, glm::mat3& matrix) {
	glUniformMatrix3fv(matrixHandle, 1, GL_FALSE, glm::value_ptr(matrix));
}

void CShader::setUniform(unsigned int vecHandle, glm::vec2& vector) {
	glUniform2fv(vecHandle, 1, glm::value_ptr(vector));
}

void CShader::setUniform(unsigned int vecHandle, glm::i32vec2& vector) {
	glUniform2iv(vecHandle, 1, glm::value_ptr(vector));
}

void CShader::setUniform(unsigned int vecHandle, glm::vec3& vector) {
	glUniform3fv(vecHandle, 1, glm::value_ptr(vector));
}

void CShader::setUniform(unsigned int vecHandle, const glm::vec4& vector) {
	glUniform4fv(vecHandle, 1, glm::value_ptr(vector));
}

void CShader::setUniform(unsigned int intHandle, int value) {
	glUniform1i(intHandle, value);
}

void CShader::setUniform(unsigned int floatHandle, float value) {
	glUniform1f(floatHandle, value);
}

void CShader::setUniform(unsigned int arrayHandle, std::vector<glm::vec4>& vec) {
	glUniform4fv(arrayHandle, vec.size(), (float*)(vec.data()));

}

void CShader::setTexture0(unsigned int samplerHandle,  int textureHandle) {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	glUniform1i(samplerHandle, 0);
}

void CShader::setTexture1(unsigned int samplerHandle, int textureHandle) {
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	glUniform1i(samplerHandle, 1);
}


std::vector<unsigned int> CShader::loadFiles(const std::string& shaderName) {
	auto dataPath = std::filesystem::current_path().string() + "\\Data\\shaders\\";
	std::string fullPath;

	if (shaderName.compare(0, dataPath.size(), dataPath))
		fullPath = dataPath + shaderName;
	else
		fullPath = shaderName;

	std::vector<unsigned int> tmpShaderList;

	tmpShaderList.push_back( load(vertexS, fullPath + ".vert") );
	std::ifstream geomExists(fullPath + ".geom");
	if (geomExists) {
		tmpShaderList.push_back( load(geometryS, fullPath + ".geom") );
	}
	std::ifstream fragExists(fullPath + ".frag");
	if (fragExists) {
		tmpShaderList.push_back( load(fragS, fullPath + ".frag") );
	}
	return  tmpShaderList;
}

unsigned int CShader::load(TShader shaderType, const std::string& shaderFilename) {
	std::ifstream shaderFile(shaderFilename.c_str());
	if (!shaderFile.is_open()) {
		liveLog << alertMsg << "\nShader file " << shaderFilename << " not found.";
		return 0;
	}
	std::string path = shaderFilename.substr(0, shaderFilename.find_last_of("\\") + 1);
	std::stringstream shaderData; std::string line, incl; size_t p;
	while (getline(shaderFile, line)) {
		p = line.find_first_not_of(" \t");
		if (p == std::string::npos)
			p = 0;
		if (line.compare(p, 8, "#include") == 0) {
			incl = line.substr(p + 9);
			incl = incl.substr(incl.find_first_not_of(" \t"));
			incl = path + incl;
			std::ifstream inclFile(incl.c_str());
			if (!inclFile.is_open()) {
				liveLog << alertMsg << "\nInclude file " << incl << " not found.";
				return 0;
			}
			shaderData << inclFile.rdbuf();
			inclFile.close();
		}
		else
			shaderData << line << "\n";
	}

	shaderFile.close();
	std::string tmp; tmp = shaderData.str();
	const char* str = tmp.c_str();
	GLint shaderGL;
	switch (shaderType) {
	case vertexS: shaderGL = GL_VERTEX_SHADER; break;
	case geometryS: shaderGL = GL_GEOMETRY_SHADER; break;
	case fragS: shaderGL = GL_FRAGMENT_SHADER; break;
	}
	GLuint shader = glCreateShader(shaderGL);
	glShaderSource(shader, 1, &str, NULL);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shader, infoLogLength, NULL, strInfoLog);

		liveLog << alertMsg << "\nCompile failure in " << shaderFilename;
		liveLog << strInfoLog;
		sysLog << alertMsg << "\nCompile failure in " << shaderFilename;
		sysLog << strInfoLog;
		delete[] strInfoLog;
	}
	return shader;
}


void CShader::attachShaders(std::vector<unsigned int>& tmpShaderList) {
	handle = glCreateProgram();
	for (size_t iLoop = 0; iLoop < tmpShaderList.size(); iLoop++)
		glAttachShader(handle, tmpShaderList[iLoop]);
}

void CShader::linkShaders() {
	glLinkProgram(handle);

	GLint status;
	glGetProgramiv(handle, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;
		glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &infoLogLength);

		GLchar* strInfoLog = new GLchar[infoLogLength + 1];
		glGetProgramInfoLog(handle, infoLogLength, NULL, strInfoLog);
		liveLog << alertMsg << "\nLinker failure:\n" << strInfoLog;
		sysLog << alertMsg << "\nLinker failure:\n" << strInfoLog;
		delete[] strInfoLog;
	}

}


namespace {
	std::unordered_map<std::string, std::shared_ptr<CShader>> shaders;
}

CShader* shader::create(const std::string& name) {
	shaders[name] = std::make_shared<CShader>(name);
	return shaders[name].get();
}

CShader* shader::get(const std::string& name) {
	return shaders[name].get();
}
