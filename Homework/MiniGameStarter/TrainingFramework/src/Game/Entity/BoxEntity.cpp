#include "BoxEntity.h"

BoxEntity::BoxEntity() : Entity(EntityType::Box)
{
    LoadSprite("objects\\Box.tga");
}
