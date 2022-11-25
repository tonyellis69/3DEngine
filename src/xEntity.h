#pragma once

#include <vector>
#include "Sprite.h"



/** Class for objects that have a physical existence, eg, velocity, potential of collision, etc. */
class CEntity
{
public:
	CEntity(void);
	~CEntity(void);
	int SetSheet(TSpriteSheet* aSheet);

	float Rotation; ///< Rotation of object, in degrees.
	C2DVector Velocity; ///< Velocity of object.
	int PosX; ///<X-coordinate of position on-screen.
	int PosY; ///<Y-coordinate of position on-screen.
	int ID;

	CSprite Sprite; ///<The sprite used to draw this entity.
};

typedef std::vector<CEntity> CEntityList; ///<Defines a vector type for holding entities.


class CEntityManager 
{
public:
	CEntityManager(void);
	~CEntityManager(void);
	int CreateEntity(TSpriteSheet* SpriteSheet);
	int EntID;

	CEntityList EntityList; ///<Holds all entities;

};
