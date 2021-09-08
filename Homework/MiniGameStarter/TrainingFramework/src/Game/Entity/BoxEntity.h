#pragma once

#include "Entity.h"

class BoxEntity : public Entity
{
public:
    BoxEntity(std::unique_ptr<Sprite2D> sprite);
};
