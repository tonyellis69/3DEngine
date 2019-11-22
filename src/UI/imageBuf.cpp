#include "imageBuf.h"



C2DimageBuf::C2DimageBuf() {
	nextId = 0;
}

/** Set the size of the main buffer. This will wipe any existing content.*/
void C2DimageBuf::setSize(int width, int height) {
	maxSize = { width, height };

	buffers.resize(1);
	buffers[0].setSize(glm::i32vec2{ width, height });
	nextId = 0;
}

/** Reserve a chunk of image memory, and return the identifying id. */
unsigned int C2DimageBuf::getBuf(glm::i32vec2& size) {
	//do we have space in an existing buffer?
	//yes? Reserve it
	//no? Create a new buffer
	//reserve space in it
	for (auto buffer : buffers) {
		if (buffer.reserve(size, nextId))
			return nextId++;
	}

	buffers.push_back(CBufRec());
	buffers.back().setSize(glm::i32vec2{ maxSize.x,glm::max(maxSize.y / 2, size.y) });
	//TO DO: this may not be the best strategy for size of new buffers
	//investigate?

	buffers.back().reserve(size, nextId);
	return nextId++;
}



void CBufRec::setSize(glm::i32vec2& size) {
	buffer.resize(size.x, size.y);
	topSpace = 0;
	bottomSpace = size.y;
};

//** Attempt to reserve the requested block of memory.*/
bool CBufRec::reserve(glm::i32vec2& size, unsigned int id) {
	if (size.x > buffer.width) //needs a wider buffer
		return false;

	if (topSpace > size.y) {
		TBlock block = { id,{0,topSpace - size.y}, size };
		blocks.push_back(block);
		topSpace -= size.y;
		return true;
	}

	if (buffer.height - bottomSpace > size.y) {
		TBlock block = { id,{0,bottomSpace}, size };
		bottomSpace += size.y;
		blocks.push_back(block);
		return true;
	}

	return false;
}
