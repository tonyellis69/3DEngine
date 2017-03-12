#include "handonLayer.h"


CHandonLayer::CHandonLayer() {
	chunks=NULL;
	first = true;
	totalSlots = 0;
}

void CHandonLayer::setSize(int s) {
	totalSlots = s * s;
	chunks = new Chunk* [totalSlots];
}

void CHandonLayer::init() {
	first = true;
	clear();
}

void CHandonLayer::clear() {
	for (int i=0;i<totalSlots;i++)
		chunks[i] = NULL;
}



CHandonLayer::~CHandonLayer() {
	if (chunks != NULL)
		delete chunks;
}

