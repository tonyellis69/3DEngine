#include "dynBuf.h"

template <class NT>
inline CDynBuf<NT>::CDynBuf() {
	pReservedVerts = NULL;
	pReservedIndices = NULL;
	nFreeIndices = nFreeVerts = 0;
	vertsPerIndex = 1;//4.0f/6.0f;
	vertBufSize = 10000;
}

template <class NT>
/** Reserve buffer space for nIndices indices and the corresponding vertices.*/
void CDynBuf<NT>::reserve(int nIndices, NT* &verts, unsigned short* &index, int& offset) {
	//do we have enough space in the current buffer?
	CShortBuf<NT>* shortBuf;

	if (nIndices > nFreeIndices) { //no? create a new buffer, reset the offsets
		shortBuf = new CShortBuf<NT>(vertsPerIndex *65536 );

		verts = shortBuf->verts;
		index =  shortBuf->index;
		nFreeIndices = 65536 - nIndices;
		offset = 0;
		bufList.push_back(shortBuf);
		vertBuf = shortBuf;

		pNextFreeVerts = shortBuf->verts + (int)(nIndices * vertsPerIndex);
		pNextFreeIndices = shortBuf->index + nIndices;
		nextOffset = (int)(nIndices * vertsPerIndex);
		shortBuf->totalIndices = nIndices;
		shortBuf->totalVerts += (int)(nIndices * vertsPerIndex);;
		return;
	}

	shortBuf = (CShortBuf<NT>*) vertBuf;
	verts = pNextFreeVerts;
	index = pNextFreeIndices;
	offset = nextOffset;

	nFreeIndices -= nIndices;
	pNextFreeVerts = verts + (int)(nIndices * vertsPerIndex);
	pNextFreeIndices = index + nIndices;
	nextOffset = offset +(int)(nIndices * vertsPerIndex);
	shortBuf->totalIndices += nIndices;
	shortBuf->totalVerts += (int)(nIndices * vertsPerIndex);;
}

template <class NT>
/** Reserve buffer space for unindexed vertices.*/
void CDynBuf<NT>::reserve(int nVerts, NT* &verts) {
	if (nVerts > nFreeVerts) { //no? create a new buffer, reset the offsets
		vertBuf = new CVertBuf<NT>(vertBufSize);
		verts = vertBuf->verts;
		nFreeVerts = vertBufSize - nVerts;
		bufList.push_back(vertBuf);

		pNextFreeVerts = vertBuf->verts + nVerts;
		vertBuf->totalVerts += nVerts;
		return;
	}
	verts = pNextFreeVerts;
	pNextFreeVerts = verts + nVerts;
	vertBuf->totalVerts += nVerts;
	nFreeVerts -= nVerts;
}


/** Free up all memory used.*/
template <class NT>
void CDynBuf<NT>::freeMem() {


}

template <class NT>
CDynBuf<NT>::~CDynBuf() {
	for (size_t b=0; b < bufList.size();b++) {
		delete bufList[b];
	}
}

template class CDynBuf<vBuf::T3Dvert>;