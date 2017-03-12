#pragma once
#include "Sprite.h"
#include "Types.h"

//stupid compiler warning with 
#pragma warning(disable:4996)

const int noTile = 255;

/** CSceneLayer is a base class for recording background scenery details. Layers may be tile maps or lists
	of background images and where they're placed. */

class CSceneLayer {
public:
	CSceneLayer() {Drawn = false; ViewOrigin.x = ViewOrigin.y = 0; Parallax = 1;
    drawColour.r =1;drawColour.g=1;drawColour.b=1;drawColour.a=1; 
	occlusionOn = false; visible=true;}
	virtual void GetDrawData(TRect& Rect,int& TextureNo, float& x, float& y) {};
	virtual void InitialiseDrawing() {};
	virtual void Scroll(float x, float y) { };
	virtual void Resize(float ViewWidth, float ViewHeight) {};
	void SetParallax(float val);
	virtual int collides(float x, float y) {return -1;};
	//virtual CRect getElementDimensions(int item) {  CRect dummy; return CRect;};
	virtual void removeItem(int itemNo) {};
	int Type; ///<The type of the layer may be tile, image, etc.
	bool Drawn; ///<True when this layer has been entirely drawn.
	C2DVector ViewOrigin; ///<Relative position of the bottom-left corner of the viewable area of this layer.
	float Parallax;
	rgba drawColour; ///<Base colour used to draw layer, useful for alpha.
	bool occlusionOn; ///<If true we don't draw out-of-frame elements.
	bool visible; ///<Only drawn if this is true.
};



/** A grid of tiles representing a backdrop. Provides the engine with the correct tiles to draw 
	and the correct position, for any given position within the grid. */

class CTileLayer : public CSceneLayer {
public:
	CTileLayer(void);
	~CTileLayer(void);
	void InitialiseDrawing();
	void Resize(float ViewWidth,float ViewHeight);
	void CalcDrawExtents();
	void Scroll(float x, float y); 
	void SetViewOffset(int x, int y);
	int GetTile(int col, int row);
	int GetVisibleTile(int CurrentDrawCol, int CurrentDrawRow);
	void getRowCol(float screenX,float screenY,int& col, int& row);
	int getTileXY(float x, float y);
	void SetSheet( TSpriteSheet* Sheet);
	void GetDrawData(TRect& Rect, int& TextureNo,float& x, float& y);
	void setTile(int col, int row, char value);
	void resizeRowCol(int newCols, int newRows);

	unsigned char* TileData; ///<Points to memory representing the tile numbers of each cell of the grid.
	TSpriteSheet* TileSheet; ///<The tilesheet this tile layer uses.
	int ViewTotalRows; ///<Number of columns of tiles composing the onscreen view.
	int ViewTotalCols; ///<Number of rows of tiles composing the onscreen view.
	int TotalCols; ///<Total number of tile columns.
	int TotalRows; ///<Total number of tile rows.
	
	int LeftmostViewCol; ///< Leftmost column of tiles to draw, given the current screen offset.
	int BottomVisibleRow; ///< Bottom-most column of tiles to draw, given the current screen offset.
	bool Repeats; ///<True if the pattern of this layer repeats beyond its boundries.

	C2DVector SubTileOffset; ///<The extent to which tiles are offset from a regular grid.

	int CurrentDrawCol; ///<Current column we're drawing of the viewable area.
	int CurrentDrawRow; ///<Current row we're drawing of the viewable area.

	int RightmostDrawCol; ///<The rightmost drawable column of the current view.
	int TopmostDrawRow; ///<The topmost drawable row of the current view.
	float xStart; ///<Position on screen where we start drawing tiles, usually 0.
	float yStart; ///<Position on screen where we start drawing tiles, usually 0.

	int drawableTiles; ///<Number above which tiles aren't drawn.
};


/** Stores the details of an image to be found at a particular position. */
class CSceneImage {
public:
	void CSceneImag() { Visible = true; };
	int tileOrTexture; ///The texture number of the image, or a spritesheet item.
	float PosX;  ///X coordinate of image position.
	float PosY;  ///Y coordinate of image position.
	TRect Rect; ///<Dimensions and mapping data.
	bool Drawn; ///<True if this image has been drawn.
	bool Visible; ///<True if this image is to be drawn at all.
};


/** An arrangement of images representing a backdrop. Provides the parent scene object with the correct image to draw 
	and the correct position, for any given position within its dimensions. */
class CImageLayer : public CSceneLayer {
public:
	CImageLayer() { imageSheet = NULL;}
	~CImageLayer();
	void AddImage(int texture, float x, float y,int width, int height);
	void CImageLayer::addImageTile(int tileNo,  float x, float y,int width, int height);
	void InitialiseDrawing();
	void GetDrawData(TRect& Rect, int& textureNo,float& x, float& y);
	void Scroll(float x, float y); 
    CRect getElementDimensions(int item);
	void removeItem(int itemNo);

	std::vector<CSceneImage> ImageList; ///<List of images and their positions in the scene.
	unsigned int DrawNext; ///<Identifies next image to draw from ImageList.
	TSpriteSheet* imageSheet; ///<If this layer uses a sheet of images rather than multiple files, this is it.
};


class CBackdropLayer : public CImageLayer {
public:
	void Resize(float ViewWidth,float ViewHeight);
	int CurrentImage; ///<In case of multiple images, this indexes the one we're drawing.
};


class CSceneObj {
public:
	~CSceneObj();
	CTileLayer* createEmptyTileLayer(int totalCols, int totalRows);
	CTileLayer* CreateTileLayer(unsigned char* TileData, int TotalCols, int TotalRows);
	CImageLayer* CreateImageLayer();
	void CreateBackdropLayer(int Texture);
	void Resize(float ViewWidth, float ViewHeight);
	C2DVector Scroll(float x, float y);
	void InitialiseDrawing();
	bool GetDrawData(TRect& Rect, int& TextureNo,float& x, float& y, rgba& drawColour);
	void setBackdropLayer(int Texture);
	void setBackdropTiling( float u, float v, float s, float t);
	void deleteLayer(int layerNo);

	std::vector<CSceneLayer*> LayerList; ///List of scene layers.
};


#define TILE_LAYER 1
#define IMAGE_LAYER 2
#define BACKDROP_LAYER 3