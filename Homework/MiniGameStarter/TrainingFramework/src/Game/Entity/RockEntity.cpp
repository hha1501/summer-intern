#include "RockEntity.h"

RockEntity::RockEntity() : Entity(EntityType::Rock)
{
    LoadSprite("objects\\rock3.tga");
}
