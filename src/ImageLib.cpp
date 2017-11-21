// TextureLib.cpp: implementation of the CTextureLib class.
//
//////////////////////////////////////////////////////////////////////

#include "ImageLib.h"
//#include "Sprite.h"
#include "TexLoader.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include "SysFnt.h"
#include "smallsysf.h"
#include <sstream>     
#include <windows.h> //TO DO: gl.h needs this, annnoyingly
#include <gl/gl.h> //temp!


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/** Precalculate and store the texture mapping coordinates needed to draw the tiles of this SpriteSheet. */
void TSpriteSheet::CalcTileMapping()
{
	NoXTiles =  TileImageWidth/ TileWidth; 
	NoYTiles =  TileImageHeight/TileHeight;

	Tiles = new TMap[NoXTiles * NoYTiles];

	double w = (1.0f / NoXTiles);
	double h = (1.0f /NoYTiles);
	
	double xAdjust =  0.5f / (TileImageWidth );
	double yAdjust = 0.5f / (TileImageHeight ) ;

	int TileNo = 0;
	for (int y=NoYTiles-1;y>=0;y--)	{
		for (int x=0;x<NoXTiles;x++) {
			Tiles[TileNo].u = (float)w * x; 
			Tiles[TileNo].v = ( (float)h   * y);
			Tiles[TileNo].s = (float)((w * (x+1)) - xAdjust);
			Tiles[TileNo].t =  (float)((h * (y+1)) - yAdjust);
			Tiles[TileNo].u += (float)xAdjust;
			Tiles[TileNo].v += (float)yAdjust;
			TileNo++;
		}
	}
}

TSpriteSheet::~TSpriteSheet(){
}

CImageLib::CImageLib()
{
	TileSetID = 0;
	SpriteID = 0;
	FontID = 0;
	NoTextures = 0;
	TileSetList.reserve(5);
}

/** Load the given image file as a new tile set, having tiles of the given dimensions.*/
int CImageLib::loadSpriteSheet(const char* Filename, int TileWidth, int TileHeight)
{
	int TexWidth, TexHeight;
	int TextureNo = CreateTextureFromImage(Filename,&TexWidth,&TexHeight,false);  //Turn the tile imagefile into a texture.
	//TO DO: don't load spritesheet as a CTexture, it's messy
	//create a new SpriteSheet record
	TSpriteSheet* Tileset = new TSpriteSheet();

	Tileset->TileImageWidth = TexWidth;
	Tileset->TileImageHeight = TexHeight;
	Tileset->textureHandle = textureList[TextureNo]->handle; //TO DO: phase out this member!
	Tileset->textureNo = TextureNo;
	Tileset->TileWidth = TileWidth;
	Tileset->TileHeight = TileHeight;
	Tileset->TileHalfWidth = TileWidth/2;
	Tileset->TileHalfHeight = TileHeight/2;
	Tileset->ID = TileSetID++;

	//Pre-calculate texture coordinates
	Tileset->CalcTileMapping();

	//add it to the list of SpriteSheets
	TileSetList.push_back(Tileset);

	return TileSetID-1;
}


/** Load the given image file as a texture, and returns its texture index number. */
 int CImageLib::CreateTextureFromImage(const char* Filename, int* TexWidth, int* TexHeight, bool UsePOT )
{
	/**
	\param force_channels 0-image format, 1-luminous, 2-luminous/alpha, 3-RGB, 4-RGBA
	\param reuse_texture_ID 0-generate a new texture ID, otherwise reuse the texture ID (overwriting the old texture)
	\param flags can be any of SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_MIPMAPS | SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT | SOIL_FLAG_DDS_LOAD_DIRECT
	\return 0-failed, otherwise returns the OpenGL texture handle
**/
	unsigned char* Data = SOIL_load_image(Filename,TexWidth, TexHeight, &Channels, SOIL_LOAD_AUTO); //load it to get the width, height, etc

	int Flags = SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_INVERT_Y;
	if (UsePOT)
		Flags |= SOIL_FLAG_POWER_OF_TWO;
	//NB SOIL messes up mipmaps where image isn't POT - softens them, presumably through a dodgy conversion to POT
	int handle = 0;
	if (Data) {
		handle = SOIL_create_OGL_texture(Data,*TexWidth,*TexHeight, Channels, SOIL_CREATE_NEW_ID,Flags); //then turn it into an OpenGL texture
		SOIL_free_image_data(Data);	
	}

	//NB 2: Soil uses GL_LINEAR filtering, which samples surrounding pixels. But for tiles this
	//messes up the edge pixels, creating minute seams. We're currently using my version of
	//soil.lib, which uses GL_NEAREST. May cause problems elsewhere.
	

	


	if (handle == 0) {
		fprintf(stderr,"\nFailed to create a texture from %s.",Filename);
		//exit(EXIT_FAILURE );
		return -1; 
	}

	CTexture* texture = new CTexture(handle,*TexWidth,*TexHeight);
	textureList.push_back(texture);

	return NoTextures++;
}

//TO DO: no longer use this, can probably scrap.

/** Return the tile data - texture coordinates etc - for the given tile. */
TRect* CImageLib::GetTileData(TSpriteSheet* TileSet, int TileNo) {
	return (TRect*)  &TileSet->Tiles[TileNo];
}

/** Return a pointer to the identified SpriteSheet. */
TSpriteSheet* CImageLib::GetTileSet(int TileSetID){
	return TileSetList[TileSetID];
}


/** Load a single image as a sprite. */
CSprite* CImageLib::loadSprite(char* Filename) {
	int imgWidth, imgHeight;
	int TexNo = CreateTextureFromImage(Filename, &imgWidth, &imgHeight,true);
	//TO DO: do the above without creating a CTexture - it's messy
	//create a new sprite
	CSprite Sprite; 
	//TO DO: make all this a CSprite function, SetToTexture
	//TO DO: phase out textureHandle, textureNo is agnostic.
	//TO DO: rename textureNo textureID.
	Sprite.textureHandle = textureList[TexNo]->handle; 
	Sprite.textureNo = TexNo;
	Sprite.Rect.width = (float)imgWidth;
	Sprite.Rect.height = (float)imgHeight;
	Sprite.Rect.originX  = (float)imgWidth/2;
	Sprite.Rect.originY = (float)imgHeight/2;
	Sprite.Rect.Map.u = 0;
	Sprite.Rect.Map.v = 0;
	Sprite.Rect.Map.s = 1;
	Sprite.Rect.Map.t = 1;
	Sprite.ID =SpriteID++;

	//add it to the list
	SpriteList.push_back(Sprite);

	return &SpriteList[SpriteID-1];
}


unsigned int CImageLib::MakeDefaultTexture(int* TexW, int* TexH) {
	unsigned char data[3*64*64*3]; int  p = 0;
	 for(int x = 0; x < 64; x++) 
    for(int y = 0; y < 64; y++) 
    {    
         unsigned int c = x ^ y; 
         data[p+ x + (y*64)] = c;
		 data[p+1+ x + (y*64)] = c;
		 data[p+2+ x + (y*64)] = c;
			p += 3;
    } 
	*TexW =  64; *TexH = 64;
	int TexNo = SOIL_create_OGL_texture(data,64,64,3,1,0);

	return TexNo;
}

unsigned char* CImageLib::readHeightMap(std::string& filename, int& width, int& height) {
	int channels;
	return SOIL_load_image (filename.c_str(),&width,&height,&channels,SOIL_LOAD_L);
}

CImageLib::~CImageLib()
{
	//delete the various objects we've created

	int ListSize = TileSetList.size();
	for (int i=0; i<ListSize; ++i) {
		delete[] TileSetList[i]->Tiles;
		delete TileSetList[i];
	}

	SpriteList.clear();
	//ListSize = FontList.size();
//	for (int i=0; i<ListSize; ++i)
//		delete[] FontList[i].Glyphs; 
	//for (int f=0;f<ListSize;f++)
	//	delete FontList[f];

	ListSize = textureList.size();
	for (int i=0; i<ListSize; ++i)
		delete textureList[i]; 
}
