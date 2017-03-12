#include "TileLayer.h"
#include <iostream> //cout
#include <fstream> //ifstream
#include "Config.h"
using namespace std;


void CSceneLayer::SetParallax(float val) {
	Parallax = val;
	
}

CTileLayer::CTileLayer(void) {
	ViewOrigin.x = 0; ViewOrigin.y = 0;
	LeftmostViewCol = 1; BottomVisibleRow = 1; 
	Repeats = false;//true;
	drawableTiles = noTile -1; 
	}


CTileLayer::~CTileLayer(void){
	//if (TileData != NULL)
	//;//	delete TileData;
}

/** Prepare this layer for drawing. */
void CTileLayer::InitialiseDrawing() { 
	Drawn = false;
	CurrentDrawCol = 0; //LeftmostViewCol; 
	CurrentDrawRow = 0;//BottomVisibleRow;
}

/** Calculates how many rows and columns of tiles are needed to fill a view of the current size.*/
void CTileLayer::Resize(float ViewWidth,float ViewHeight) {
	ViewTotalCols = (int) (ViewWidth/TileSheet->TileWidth)+2;
	ViewTotalRows = (int)(ViewHeight/TileSheet->TileHeight)+2;
	CalcDrawExtents();
}

void CTileLayer::CalcDrawExtents() {

	if (ViewOrigin.x < 0) {
		LeftmostViewCol = (int) abs(ViewOrigin.x) / TileSheet->TileWidth;
		xStart = 0  - (abs(ViewOrigin.x) -  (LeftmostViewCol * TileSheet->TileWidth ) );
	}
	else {
		xStart = ViewOrigin.x;
		LeftmostViewCol = 0;
	}

	if (ViewOrigin.y < 0) {
		BottomVisibleRow = (int)abs(ViewOrigin.y) / TileSheet->TileHeight;
		yStart = 0  - (abs(ViewOrigin.y) -  (BottomVisibleRow * TileSheet->TileHeight ) );

	}
	else {
		yStart = ViewOrigin.y;
		BottomVisibleRow = 0;
	}

	if (occlusionOn) {
	
		TopmostDrawRow = BottomVisibleRow + ViewTotalRows;
		if (TopmostDrawRow > TotalRows)
			TopmostDrawRow = TotalRows;
		RightmostDrawCol = LeftmostViewCol + ViewTotalCols;
		if (RightmostDrawCol > TotalCols)
			RightmostDrawCol = TotalCols;
	}
	else {
		TopmostDrawRow = TotalRows - BottomVisibleRow;
		RightmostDrawCol = TotalCols -LeftmostViewCol ;
	}

}

/** Sets the position within the tile layer of the bottom-left corner of the screen. */
void CTileLayer::SetViewOffset(int x, int y) {
	ViewOrigin.set((float) x, (float) y);
//	SubTileOffset.set(fmod(ViewOrigin.x, (float)TileSheet->TileWidth), fmod(ViewOrigin.y, (float)TileSheet->TileHeight));
	CalcDrawExtents();
}


/** Moves the relative view position within this tile layer. */
void CTileLayer::Scroll(float x, float y) {
	ViewOrigin.x += (x*Parallax);
	ViewOrigin.y += (y*Parallax);
	
	CalcDrawExtents();
	//SubTileOffset.set(fmod(-ViewOrigin.x, (float)TileSheet->TileWidth), fmod(-ViewOrigin.y,  (float) TileSheet->TileHeight));
}



/** Returns the tile to be found at the given row and column of this layer. */
int CTileLayer::GetTile(int col, int row) {
	//row = TotalRows - row;
/*	if (Repeats) {  //TO DO: probably just get rid of this
		if (col < 0) 
			col = TotalCols + (col % TotalCols)-1;
		else if (col >= TotalCols)
			col = col  % TotalCols;
		if (row < 0 ) 
			row = TotalRows + (row % TotalRows) - 1;
		else if (row >= TotalRows)
			row = row % TotalRows;   
	}
	else */
		if ((col < 0)||(row < 0) || (col >= TotalCols) || (row >= TotalRows))
			return noTile;
	
	int Pos = (row * TotalCols) + col;
	return (unsigned char)TileData[Pos];
}

/** Return the tile at the given row and column of the current view.*/
int CTileLayer::GetVisibleTile(int ViewCol, int ViewRow) {
	int row = TotalRows - (ViewRow+1+BottomVisibleRow);
	int col = ViewCol;// + LeftmostViewCol;
	return GetTile(col,row);
}

/** Return the row and column at the given point on the screen. */
void CTileLayer::getRowCol(float screenX,float screenY,int& col, int& row) {
	float fcol = (screenX - ViewOrigin.x  ) / TileSheet->TileWidth;
	float frow = (screenY - ViewOrigin.y ) / TileSheet->TileWidth;
	col = (int)fcol;
	row = (int) frow;
}

/** Return the tile at the give screen coordinates.*/
int CTileLayer::getTileXY(float x, float y) {
	int row, col;
	getRowCol(x,y,col,row);
	return GetTile(col,row);
}

/**  Set the given tile to the given value. */
void CTileLayer::setTile(int col, int row, char value) {
	TileData[(row * TotalCols) + col] = value;
}


/** Set this tile layer to use the given tilesheet. */
void CTileLayer::SetSheet( TSpriteSheet* Sheet) {
	TileSheet = Sheet;
	CalcDrawExtents();
}

/** Returns data to draw a visible tile in this layer. Call this function repeatedly to draw all the visible tiles in 
	the current view. */
void CTileLayer::GetDrawData(TRect& Rect, int& textureNo,float& x, float& y) {
	unsigned char Tile;
	do { //goes up in cols then rows, eg 1,0 2,0 3,0 ... 0,1  1,1 2,1...
		//TO DO: goes too far, beyond final row, check why
		Tile = GetTile(CurrentDrawCol +LeftmostViewCol ,CurrentDrawRow + BottomVisibleRow);
		if (Tile <= drawableTiles) {
			//textureHandle = TileSheet->textureHandle;
			textureNo = TileSheet->textureNo;
			Rect.Map = TileSheet->Tiles[Tile];
			//Rect.XOff = (float)TileSheet->TileHalfWidth;
			//Rect.YOff = (float)TileSheet->TileHalfHeight;

			Rect.originX = (float)TileSheet->TileHalfWidth;
			Rect.originY = (float)TileSheet->TileHalfHeight;
			Rect.width = (float)TileSheet->TileWidth;
			Rect.height = (float)TileSheet->TileHeight;

			//calculate screen coords at which to draw tile:
		//	x = (TileSheet->TileWidth * (CurrentDrawCol + LeftmostViewCol)) - SubTileOffset.x;
			x = (xStart ) + (TileSheet->TileWidth * CurrentDrawCol);// - SubTileOffset.x ;
			//y =  ((CurrentDrawRow + BottomVisibleRow) * TileSheet->TileHeight) + SubTileOffset.y;
			y = (yStart ) + (TileSheet->TileHeight * CurrentDrawRow);
			
			x += TileSheet->TileHalfWidth;
			y += TileSheet->TileHalfHeight;
		}
		CurrentDrawCol++; //find next tile to draw, if any
		if (CurrentDrawCol > RightmostDrawCol) {
			CurrentDrawCol = 0;// LeftmostViewCol;
			CurrentDrawRow++;
		}
		if (CurrentDrawRow  >= TopmostDrawRow) {
			Drawn = true;
			//break;
		}
	} while ((Tile > drawableTiles) && !Drawn);
}


/** Change the total number of rows and columns. Tile data is lost if it falls outside the 
	new dimensions of the layer. */
void CTileLayer::resizeRowCol(int newCols, int newRows) {
	unsigned char* newTileData = new unsigned char[newCols * newRows];
	std::fill_n(newTileData,newCols * newRows,noTile);
	for (int r=0;r<newRows && r<TotalRows;r++) {
		for (int c=0;c<newCols && c<TotalCols;c++) {
			newTileData[(r*newCols)+c] = TileData[(r*TotalCols)+c];
		}
	}
	delete[] TileData;
	TileData = newTileData;
	TotalRows = newRows; TotalCols = newCols;
	
	CalcDrawExtents();

}


/** Add the given image to this image layer. */
void CImageLayer::AddImage(int texture, float x, float y,int width, int height) {
	CSceneImage Image;
	Image.tileOrTexture = texture;
	Image.PosX = x; Image.PosY = y;
	Image.Rect.width = (float)width; Image.Rect.height = (float)height;
	Image.Rect.originX = (float)width/2; Image.Rect.originY = (float)height/2;
	ImageList.push_back(Image);
}

/** Add the given image tile to this image layer. */
void CImageLayer::addImageTile(int tileNo,  float x, float y,int width, int height) {
	CSceneImage Image;
	Image.tileOrTexture = tileNo;
	Image.PosX = x; Image.PosY = y;
	Image.Rect.width = (float)width;
	Image.Rect.height = (float)height;
	Image.Rect.originX = (float)imageSheet->TileWidth/2;
	Image.Rect.originY = (float)imageSheet->TileHeight/2;
	ImageList.push_back(Image);
}

/** Prepare this layer for drawing. */
void CImageLayer::InitialiseDrawing() {
	DrawNext = 0; 
	Drawn = false;
}

/** Provide drawing data for a rectangular element of this layer. Call this function repeatedly to iterate through
	all the elements of this layer. */
void CImageLayer::GetDrawData(TRect& Rect,int& TextureNo, float& x, float& y) {
	//find the next image in this layer to draw...
	if (DrawNext < ImageList.size()) {
		if (ImageList[DrawNext].Visible) {
			x =  ImageList[DrawNext].PosX + ViewOrigin.x; 
			y = ImageList[DrawNext].PosY + ViewOrigin.y;
			Rect = ImageList[DrawNext].Rect;
			if (imageSheet == NULL) {
				TextureNo =  ImageList[DrawNext].tileOrTexture;
			}
			else {
				Rect.Map = imageSheet->Tiles[ImageList[DrawNext].tileOrTexture];
				TextureNo = imageSheet->textureNo;
			}
		}
		DrawNext++;
		return;
	}

	//nothing left to draw? Declare this layer 'drawn'.
	Drawn = true;
}

/** Scrolls the relative screen position within this image layer.  */
void CImageLayer::Scroll(float x, float y) {
	ViewOrigin.x += (x*Parallax);
	ViewOrigin.y += (y*Parallax);
}

/** Remove the given image from this layer. */
void CImageLayer::removeItem(int itemNo) {
	ImageList.erase(ImageList.begin() + itemNo);
}


CImageLayer::~CImageLayer() {
	//for (int i=0;i<ImageList.size();i++)
	//	;//delete (ImageList[i]);
}

#define _SCL_SECURE_NO_WARNINGS 1

/** Create a scrollable tiled backdrop that is empty of tiles. */
CTileLayer* CSceneObj::createEmptyTileLayer(int totalCols, int totalRows) {
	unsigned char* tileData = new unsigned char[totalCols * totalRows];
	std::fill_n(tileData,totalCols * totalRows,noTile);
	CTileLayer* result = CreateTileLayer(tileData,totalCols,totalRows);
	delete[] tileData;
	return result;
}

/** Create a scrollable tiled backdrop using the given tile data. */
CTileLayer* CSceneObj::CreateTileLayer(unsigned char* TileData,  int TotalCols, int TotalRows) {
	CTileLayer* NewLayer = new CTileLayer;
	NewLayer->TileData = new unsigned char [TotalCols*TotalRows];
	memcpy(NewLayer->TileData,TileData,TotalCols*TotalRows);
	NewLayer->TotalCols = TotalCols;
	NewLayer->TotalRows = TotalRows;
	NewLayer->Type = TILE_LAYER;
	
	LayerList.push_back(NewLayer);
	return (CTileLayer*) LayerList.back();
}





/** Create an image layer and add it to the scene's list of layers. */
CImageLayer* CSceneObj::CreateImageLayer() {
	CImageLayer* ImageLayer = new CImageLayer;
	ImageLayer->Type = IMAGE_LAYER;
	LayerList.push_back(ImageLayer);
	return ImageLayer;
}



/** Return the position and dimensions of the given image. */
CRect CImageLayer::getElementDimensions(int item) {
	CRect image;
	image.x = ImageList[item].PosX - ImageList[item].Rect.originX;
	image.y = ImageList[item].PosY - ImageList[item].Rect.originY;
	image.width = ImageList[item].Rect.width;
	image.height = ImageList[item].Rect.height;
	return image;
}

/** Ensure the backdrop layer's images fill the current view. */
void CBackdropLayer::Resize(float ViewWidth,float ViewHeight) {
	for (size_t i=0;i<ImageList.size();i++) {
		ImageList[i].PosX = ViewWidth/2;
		ImageList[i].PosY = ViewHeight/2;

		ImageList[i].Rect.originX = ViewWidth/2;
		ImageList[i].Rect.originY = ViewHeight/2;
		ImageList[i].Rect.width = ViewWidth;
		ImageList[i].Rect.height = ViewHeight;

	}
}

void CSceneObj::CreateBackdropLayer(int Texture) {
	CBackdropLayer* BackLayer;
	CSceneLayer* FrontLayer = NULL;
	
	if (LayerList.size()){ //if list contains layers
		FrontLayer = LayerList[0];
		if (FrontLayer->Type != BACKDROP_LAYER) { //does a backdrop layer already exist?
			BackLayer = new CBackdropLayer; //no
			BackLayer->Type = BACKDROP_LAYER;
		}
		else //yes
			BackLayer = (CBackdropLayer*)FrontLayer;

	}
	else { //no layers?
		BackLayer = new CBackdropLayer;	
		BackLayer->Type = BACKDROP_LAYER;
	}
	BackLayer->AddImage(Texture,0,0,100,100);
	LayerList.insert(LayerList.begin(),BackLayer); 	//ensure it goes at the front of the list
	BackLayer->CurrentImage = 0;

}

void CSceneObj::Resize(float ViewWidth, float ViewHeight) {
	for (size_t l=0;l<LayerList.size();l++)  {
		LayerList[l]->Resize(ViewWidth, ViewHeight);
		
	}
}

/** Scroll the layers of the scene object by the given amount. */
C2DVector CSceneObj::Scroll(float x, float y) {
	C2DVector result(0,0);
	for (size_t l=0;l<LayerList.size();l++) 
		LayerList[l]->Scroll(x,y);
	return result; //TO DO: put some kind of boundary check here?
}

/** Prepare to draw all the visible layers of this object. Call this every frame before Draw. */
void CSceneObj::InitialiseDrawing() {
	for (size_t l=0;l<LayerList.size();l++) 
		LayerList[l]->InitialiseDrawing();
}

/** Provide data for drawing a rectangular element of this scene. 
Call this function repeatedly to iterate through all the elements of all its layers. */
bool CSceneObj::GetDrawData(TRect& Rect,int& TextureNo, float& x, float& y, rgba& drawColour) {
	int size = LayerList.size();
	for (int l=0;l<size;l++) {
		if (!LayerList[l]->visible)
			continue;
		LayerList[l]->GetDrawData(Rect,TextureNo,x,y);
		drawColour = LayerList[l]->drawColour;
		if (!LayerList[l]->Drawn) {		
			return true;
		}
	}
	return false;
}

void CSceneObj::setBackdropLayer(int Texture) {
	if (!LayerList.size())
		return;
	//get a handle on the backdrop layer
	CSceneLayer* Layer = LayerList.front();
	if (Layer->Type != BACKDROP_LAYER)
		return;
	CBackdropLayer* BackLayer = (CBackdropLayer*)Layer;
	BackLayer->ImageList[BackLayer->CurrentImage].tileOrTexture = Texture;
}

void CSceneObj::setBackdropTiling( float u, float v, float s, float t) {
	//get a handle on the backdrop layer
	CSceneLayer* Layer = LayerList.front();
	if (Layer->Type != BACKDROP_LAYER)
		return;
	CBackdropLayer* BackLayer = (CBackdropLayer*)Layer;
	BackLayer->ImageList[BackLayer->CurrentImage].Rect.Map.u = u;
	BackLayer->ImageList[BackLayer->CurrentImage].Rect.Map.v = v;
	BackLayer->ImageList[BackLayer->CurrentImage].Rect.Map.s = s;
	BackLayer->ImageList[BackLayer->CurrentImage].Rect.Map.t = t;	
}

void CSceneObj::deleteLayer(int layerNo) {
	if (LayerList[layerNo]->Type == TILE_LAYER) {
			delete[] ( ((CTileLayer*)LayerList[layerNo])->TileData );
			delete LayerList[layerNo];
		}
		if (LayerList[layerNo]->Type == IMAGE_LAYER)
			delete( (CImageLayer*)LayerList[layerNo] );
		if (LayerList[layerNo]->Type == BACKDROP_LAYER)
			delete( (CBackdropLayer*)LayerList[layerNo] );

}

CSceneObj::~CSceneObj() {
	int size = LayerList.size();
	for (int l=0;l<size;l++) {
		deleteLayer(l);
	}	
}
