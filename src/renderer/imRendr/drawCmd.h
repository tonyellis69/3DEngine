#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "renderer/drawText.h"

enum TDrawCmd {drwCmdDrawLine, drwCmdText};

/** Class for encapsulating an OpenGL drawing command.*/
class CDrawCmd {
public:
	unsigned int shader;
	TDrawCmd cmdType;

	int vertStart;
	int vertCount;

	glm::mat4 matrix;

	//uniforms
	glm::vec4 vec4_0;
	glm::vec4 vec4_1;


	glm::vec3 vec3_0;
	glm::vec3 vec3_1;

	glm::vec2 vec2_0;
	glm::vec2 vec2_1;

	std::shared_ptr<CDrawText> drawText;
	unsigned int hTexture;
};