#pragma once

#include <vector>

#include "../renderer/renderTexture.h"



class CBufRec;
/** Provides 2D blocks of graphics memory for storage. Attempts to recyclee
	and use as few textures as possible. */
class C2DimageBuf {
public:
	C2DimageBuf();
	void setSize(int width, int height);
	unsigned int getBuf(glm::i32vec2& size);


private:
	glm::i32vec2 maxSize;

	std::vector<CBufRec> buffers;
	unsigned int nextId;
};




class CBufRec {
public:

	struct TBlock {
		unsigned int id;
		glm::i32vec2 start;
		glm::i32vec2 size;

	};

	CBufRec() {};
	void setSize(glm::i32vec2& size);
	bool reserve(glm::i32vec2& size, unsigned int id);
	int bufNo;
	int topSpace;
	int bottomSpace;

	CRenderTexture buffer;
	std::vector<TBlock> blocks;


};

