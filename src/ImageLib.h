// TextureLib.h: interface for the CImageLib class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTURELIB_H__0D53D514_145B_4175_A84E_ADB0B3B7A576__INCLUDED_)
#define AFX_TEXTURELIB_H__0D53D514_145B_4175_A84E_ADB0B3B7A576__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "Types.h"
#include "Sprite.h"
#include "Texture.h"




typedef std::vector<TSpriteSheet*> TSpriteSheetList; ///<Defines a vector type for holding pointers to SpriteSheets.
typedef std::vector<CSprite> CSpriteList; ///<Defines a vector type for holding sprites.
//typedef std::vector<CTexFont*> CFontList; ///<Defines a vector type for holding fonts.
///<Defines a vector type for holding textures

// Helper function to read a piece of data from a stream.
template<class T, class S>
void Read_Object(T& to_read, S& in) {
  in.read(reinterpret_cast<char*>(&to_read), sizeof(T));
}

class CImageLib  
{
public:
	CImageLib();
	virtual ~CImageLib();
	int loadSpriteSheet(const char* Filename, int TileWidth, int TileHeight);
	TSpriteSheet* GetTileSet(int TileSetID);
	int CreateTextureFromImage(const char* Filename,int* TexWidth, int* TexHeight, bool UsePOT);
	CSprite* loadSprite(char* Filename);
	TRect* GetTileData(TSpriteSheet* TileSet, int TileNo);
	unsigned int MakeDefaultTexture(int* TexW, int* TexH);

	unsigned char* readHeightMap(std::string& filename, int& width, int& height);

	TSpriteSheetList TileSetList;
	CSpriteList SpriteList;
	std::vector<CTexture*> textureList;
	int TileSetID; ///<Next free SpriteSheet ID number
	int SpriteID; ///<Next free billboard ID number.

private:
	int Channels; ///<Channels of most recently loaded image file.
	int NoTextures; ///<Number of textures held
};




#endif // !defined(AFX_TEXTURELIB_H__0D53D514_145B_4175_A84E_ADB0B3B7A576__INCLUDED_)
