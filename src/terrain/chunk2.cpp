#include "chunk2.h"

Chunk2::Chunk2()
{
	//noTris = 0;
	bufId = 0;
	//creatorSC = NULL;
	//overlapDir = none;
	status = chFree;
	drawDetails.childBufNo = -1;
	drawDetails.vertCount = 0;
	drawDetails.vertStart = 0;
	//treeDrawDetails.childBufNo = -1;
	//treeDrawDetails.vertCount = 0;
	//treeDrawDetails.vertStart = 0;
	//grassDrawDetails.childBufNo = -1;
	//grassDrawDetails.vertCount = 0;
	//grassDrawDetails.vertStart = 0;
	colour = { 0.5,0.5,0.5,1.0 };
}
