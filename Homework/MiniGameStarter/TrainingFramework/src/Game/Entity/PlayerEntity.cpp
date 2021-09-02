#include "PlayerEntity.h"

PlayerEntity::PlayerEntity() : Entity(EntityType::Player)
{
    LoadSprite("player\\hero7.tga");
}
