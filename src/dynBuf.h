#pragma once
#include <vector>
#include "vertBufs.h"

template <class T>
class CVertBuf {
public:
	CVertBuf(int nVerts) {
		verts = new T [nVerts];
		totalVerts = 0; }
	T* verts; //assign memory at initialisation. Ultimately use templates
	int totalVerts;
	~CVertBuf() { delete[] verts;}
};


/** A class for storing the maximum vertices allowed with a short int index.*/
template <class T>
class CShortBuf : public CVertBuf<T> {
public:
	CShortBuf(int nVerts) : CVertBuf(nVerts) {
		//verts = new vBuf::T3Dvert[vertsPerIndex * 65536];
		//verts = new T [nVerts];
		totalVerts = 0; totalIndices = 0;}
	unsigned short index [65536];
	//vBuf::T3Dvert* verts; //assign memory at initialisation. Ultimately use templates
	int totalIndices;
	//~CShortBuf() { delete[] verts;}
};




/** A class for managing maximum-size buffers of vertexes. */
template <class NT>
class CDynBuf {
public:
	CDynBuf();
	~CDynBuf();
	//void reserve(int nIndices, vBuf::T3Dvert* &verts, unsigned short* &index, int& offset);
	void reserve(int nIndices, NT* &verts, unsigned short* &index, int& offset);
	void reserve(int nVerts, NT* &verts);

	void free();

	float vertsPerIndex; ///<Ratio of vertices to indices.
	std::vector<CVertBuf<NT>*> bufList; ///<The buffers this class owns.

	NT* pReservedVerts; ///<Pointer to the currently reserved block of verts.
	unsigned short* pReservedIndices; ///<Pointer to the currently reserved block of indices.


	//private
	int nFreeIndices; ///<Number of free indices in the current buffer.
	int nextOffset; ///<Previous offset to indices.
	unsigned short* pNextFreeIndices;
	NT* pNextFreeVerts;

	
	CVertBuf<NT>* vertBuf;

	int vertBufSize;
	int nFreeVerts;

};