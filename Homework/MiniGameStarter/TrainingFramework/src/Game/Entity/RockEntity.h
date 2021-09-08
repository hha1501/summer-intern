#pragma once

#include "Entity.h"

class RockEntity : public Entity
{
public:
    RockEntity(std::unique_ptr<Sprite2D> sprite);
};
