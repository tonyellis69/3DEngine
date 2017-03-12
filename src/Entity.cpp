#include "Entity.h"

CEntity::CEntity(void){
	
}

int CEntity::SetSheet(TSpriteSheet* aSheet) {
	Sprite.SetToSheet(aSheet);
	return true;
}

CEntity::~CEntity(void)
{
}




	
CEntityManager::CEntityManager(void) {
	EntID  = 0;
}

int CEntityManager::CreateEntity(TSpriteSheet* SpriteSheet) {
	CEntity Ent;
	//set SpriteSheet: sets up the entity's sprite with the right spritesheet data.
	//
	Ent.Sprite.SetToSheet(SpriteSheet);
	Ent.Sprite.SetToFrame(0);

	//stick it on the entity list:
	Ent.ID = EntID++; //TO DO: get rid of IDs? They seem unnecessary and confusing at this stage.

	//Set to frame 0 by default:

	//add it to the list
	EntityList.push_back(Ent);

	return EntID-1;

}


CEntityManager::~CEntityManager(void) {

}
