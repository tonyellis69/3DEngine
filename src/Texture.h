#pragma once


/** Class for defining textures. */
class CTexture
{
public:
	CTexture(int iHandle, int iWidth, int iHeight) { handle = iHandle; width = iWidth; height = iHeight;};
	~CTexture(void);

	int handle; ///<Handle identifying this texture.
	int width; ///<Width in pixels.
	int height; ///Height in pixels.
};
