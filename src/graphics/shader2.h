#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

enum TShader { vertexS, fragS, geometryS };


/** New improved shader wrapper. */
class CShader {
public:
	CShader(const std::string& name);
	void activate();
	unsigned int getUniform(const std::string& uniformName);

	void setUniform(unsigned int matrixHandle, glm::mat4& matrix);
	void setUniform(unsigned int matrixHandle, glm::mat3& matrix);
	void setUniform(unsigned int vecHandle, glm::vec2& vector);
	void setUniform(unsigned int vecHandle, glm::i32vec2& vector);
	void setUniform(unsigned int vecHandle, glm::vec3& vector);
	void setUniform(unsigned int vecHandle, const glm::vec4& vector);
	void setUniform(unsigned int intHandle, int value);
	void setUniform(unsigned int floatHandle, float value);
	void setUniform(unsigned int arrayHandle, std::vector<glm::vec4>& value);
	void setTexture(unsigned int samplerHandle, int textureUnit);


	unsigned int handle;

private:
	std::vector<unsigned int> loadFiles(const std::string& name);
	unsigned int load(TShader shader, const std::string& shaderFile);

	void attachShaders(std::vector<unsigned int>& tmpShaderList);
	void linkShaders();

};

namespace shader {
	CShader* create(const std::string& name);
	CShader* get(const std::string& name);
}