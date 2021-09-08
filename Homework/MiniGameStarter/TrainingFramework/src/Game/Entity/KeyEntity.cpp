#include "KeyEntity.h"

KeyEntity::KeyEntity() : Entity(EntityType::Key)
{
    LoadSprite("objects\\key.tga");
}
