#include "Sprite.h"

/** Basic sprite class. */

CSprite::CSprite(void) {
	pos.x = pos.y = 0;
	Rotation = 0;
	Velocity.x = 0;
	Velocity.y = 0;
	Counter = 0;
	Status =0;
	Scale = glm::vec2(1.0f,1.0f);
	AnimStart = 0;
	AnimEnd = 0;
	AnimInterval = 0;
	AnimFwd = true;
	AnimdT = 0;
	Colour.r = 1.0f;Colour.g = 1.0f;Colour.b = 1.0f;Colour.a = 1.0f;
	FloatCounter = 0;
	Speed = 0;
	hFlip = false;
	currentAnim = noAnim;
	Live = true;
 }

/** Set the sprite up to use the given spritesheet. */
void CSprite::setToSheet(TSpriteSheet* SpriteSheet) {
	CSprite::SpriteSheet = SpriteSheet;
	textureHandle = SpriteSheet->textureHandle; //TO DO phase out
	textureNo = SpriteSheet->textureNo;
	Rect.originX = (float)SpriteSheet->TileWidth/2;
	Rect.originY = (float)SpriteSheet->TileHeight/2;
	Rect.width = (float)SpriteSheet->TileWidth;
	Rect.height = (float)SpriteSheet->TileHeight;
	SetToFrame(0);
	//Set bounding box to 80% of tile size as a rule of thumb.

}

/** Set the sprite up to use the given texture. */
void CSprite::SetToTexture(int iTextureNo,int Width, int Height) {
	textureNo = iTextureNo;
	Rect.Map.u = 0; 
	Rect.Map.v = 0;
	Rect.Map.s = 1;
	Rect.Map.t = 1;
	Rect.originX = (float)Width/2;
	Rect.originY = (float)Height/2;
	Rect.width = (float) Width;
	Rect.height = (float)Height;
}


/** Set the sprite's animation cycle to show a particular frame. */
void CSprite::SetToFrame(int FrameNo) {
	Frame = FrameNo;
	Rect.Map = SpriteSheet->Tiles[FrameNo]; 
}

/** Advance sprite's animation one frame. */
void CSprite::IncrementFrame() {
	SetToFrame(Frame+1);
}

/** Reverse sprite's animation one frame. */
void CSprite::DecrementFrame() {
	SetToFrame(Frame-1);
}

/** Set the size of this sprite's bounding box. */
void CSprite::SetBoundingBox(float x, float y) {
//	AABB.set(-x,-y,x,y);
}

/** Check for a simple bounding box collision with the given sprite. */
/*
bool CSprite::BBCollision(CSprite& Sprite) {
	int xdiff = (int) abs(pos.x - Sprite.pos.x);
	int ydiff = (int) abs(pos.y - Sprite.pos.y);

	//if ( (xdiff < (BBx + Sprite.BBx)) && (ydiff < (BBy + Sprite.BBy)) ) NOV 3
	if ( (xdiff < (AABB.maxX + Sprite.AABB.maxX)) && (ydiff < (AABB.maxY + Sprite.AABB.maxY)) )
		return true;

	return false;
} */

/** Set up the basic animation cycle, if any.
void CSprite::SetAnimation(int Start, int End, double Interval, bool Fwd) {
	AnimStart = Start;
	AnimEnd = End;
	AnimInterval = Interval;
	AnimFwd = Fwd;
} */

/** Set up an animation cycle between the given frames, identified by AnimNo. */
int CSprite::AddAnimation(int Start,int End,double Interval, bool loops) {
	//create a new animation structure.
	SpriteAnim Anim;
	Anim.Start = Start;
	Anim.End = End;
	Anim.Interval = Interval;
/*	Anim.Dir = ((End - Start) > 0)? 1 : -1 ;
	if (Anim.Dir < 0){
			Anim.Start = End;
				Anim.End = Start;
	} */ //NOV 3
	Anim.loops = loops;
	Anim.active = false;

	//add it to this sprite's list of animations.
	AnimList.push_back(Anim);
	return AnimList.size() -1;
}


/** Advance the sprite's animation according to its animation cycle and the time passed.
void CSprite::UpdateAnimation(const double& dT) {
	//is it time to update the animation?
	AnimdT += dT;
	if (AnimdT > AnimInterval) {
		AnimdT = 0;
		if (AnimFwd) {
			Frame++;
			if (Frame > AnimEnd)
				Frame = AnimStart;
		}
		else {
			Frame--;
			if (Frame < AnimStart)
				Frame = AnimEnd;
		}
		SetToFrame(Frame);
	}
}


void CSprite::UpdateAnimation(int AnimNo, const double& dT, bool Fwd) {
	//is it time to update the animation?
	AnimdT += dT;
	if (AnimdT > AnimList[AnimNo].Interval) {
		AnimdT = 0;
		Frame += Fwd ? 1 : -1;
		if (( Frame > AnimList[AnimNo].End)||(Frame < AnimList[AnimNo].Start)) {
			if (Fwd)
				Frame = AnimList[AnimNo].Start;
			else
				Frame = AnimList[AnimNo].End;
		}
		SetToFrame(Frame);
	}
}
*/
/** Set the sprite's current animation. */
void CSprite::setAnim(int animNo) {
	if (currentAnim == animNo) //if animation already running...
		return;
	//...otherwise, start this animation from scratch.
	currentAnim = animNo;
	AnimdT = 0;
	Frame = AnimList[animNo].Start;
	AnimList[animNo].active = true;
}

/** Advance the current animation according to its animation cycle and the time passed.*/
void CSprite::updateAnimation(const double& dT) {

	if (currentAnim == noAnim) //error situation, shouldn't happen
		return;
	if (!AnimList[currentAnim].active)
		return;
	
	SetToFrame(Frame);

	//is it time to update the animation?
	AnimdT += dT;
	if (AnimdT <= AnimList[currentAnim].Interval) 
		return;

	AnimdT = 0;
	if (AnimList[currentAnim].Start < AnimList[currentAnim].End) {
		Frame++;
		if (Frame > AnimList[currentAnim].End) {
			if (AnimList[currentAnim].loops)
				Frame = AnimList[currentAnim].Start;
			else { Frame--;
				AnimList[currentAnim].active = false;
				currentAnim = noAnim;
				return;
			}
		}
	}
	else {
		Frame--;
		if (Frame < AnimList[currentAnim].End) {
			if (AnimList[currentAnim].loops)
				Frame = AnimList[currentAnim].Start;
			else { Frame++;
				AnimList[currentAnim].active = false;
				return;
			}

		}
	}

}



 void CSprite::Update(const double& dT) {
	//UpdateAnimation(dT);
 }

CSprite::~CSprite(void)
{
}
