#pragma once

#include "chunk.h"


class CHandonLayer {
public:
	CHandonLayer();
	~CHandonLayer();
	void setSize(int s);
	void init();
	void clear();

	Chunk** chunks;
	int totalSlots;
	bool first;
};
