#pragma once



class CBaseBuf {	
public:
	CBaseBuf();

	virtual void setSize() {};
	virtual void copyBuf(CBaseBuf& srcBuf, unsigned int size) {};
	virtual unsigned int getBufHandle() { return 0; };
	virtual unsigned int getBufSize() { return 0; };
};