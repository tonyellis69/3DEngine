#pragma once

#include "Types.h"
#include "2DVector.h"
#include <vector>
#include "BBox.h"

class TSpriteSheet {
public:
	virtual ~TSpriteSheet();
	void CalcTileMapping();
	int TileImageWidth;	///<Width of the SpriteSheet image
	int TileImageHeight; ///<Height of the SpriteSheet image
	int NoXTiles; ///<Number of columns of tiles in this SpriteSheet.
	int NoYTiles; ///<Number of rows of tiles in this SpriteSheet.
	int TileWidth; ///<Width of every tile in this SpriteSheet.
	int TileHeight; ///<Height of every tile in this SpriteSheet.
	int TileHalfWidth; ///<Stored to save constant recalculation.
	int TileHalfHeight;///<Stored to save constant recalculation.
	int textureHandle; ///<The loaded texture storing the tile images. 
	//TO DO: phase out the above!!! textureHandle is renderer-specific, spritesheets should be agnostic
	int textureNo; ///<Texture index number. USE THIS INSTEAD.
	TMap* Tiles; ///<Pointer to an array of data structures defining the tiles of this SpriteSheet.
	unsigned int ID; ///<The unique ID number of this SpriteSheet.
};

struct SpriteAnim {
	int No;
	int Start; 
	int End;
	double Interval;
	bool loops;
	bool active;
};

class CSprite
{
public:
	CSprite(void);
	void setToSheet(TSpriteSheet* SpriteSheet);
	void SetToTexture(int iTextureNo,int Width, int Height);
	void SetToFrame(int FrameNo);
	void IncrementFrame();
	void DecrementFrame();
	void SetBoundingBox(float x, float y);
	bool BBCollision(CSprite& Sprite);
	//void SetAnimation(int Start, int End, double Interval, bool Fwd);
	int AddAnimation(int Start,int End,double Interval,bool loops);
//	void UpdateAnimation(const double& dT);
//	void UpdateAnimation(int AnimNo, const double& dT,bool Fwd);
	virtual void Update(const double& dT);
	void setAnim(int animNo);
	void updateAnimation(const double& dT);
	
	C2DVector Scale;

	TRect Rect;
	//SetVectorint XOffset;
	//int YOffset;
	int textureHandle; ///<The loaded texture storing the image.
	int textureNo; ///<Index of the texture this sprite uses.
	unsigned int ID; 
	TSpriteSheet* SpriteSheet; ///<Identifies the spritesheet used by this sprite.
	int Frame; ///<Current framenumber of the spritesheet;
	int Status; ///<General purpose bitmask.


	float Rotation; ///< Rotation of sprite, in degrees.
	C2DVector Velocity; ///< Velocity of sprite.
	float Speed; ///<The velocity multiplier. 
	C2DVector pos; ///<Sprite's position, stored as a vector.
	bool Live; ///<User flag indicating if the sprite is in play.
	int Counter; ///<General purpose counter.
	float FloatCounter; ///<Floating point counter
	//int BBx, BBy; ///<Dimensions of bounding box.

	double AnimdT;
	int AnimStart; 
	int AnimEnd;
	double AnimInterval;
	bool AnimFwd;
	rgba Colour; ///<Drawing colour used with this sprite.

	std::vector<SpriteAnim> AnimList;

	bool hFlip; ///<True if the image should be horizontally flipped.
	CBBox AABB; ///<Axis-aligned bounding box for this sprite.

	~CSprite(void);

private:
	int currentAnim; ///<Whatever animation is runnng currently.


};

const int noAnim = -1;
