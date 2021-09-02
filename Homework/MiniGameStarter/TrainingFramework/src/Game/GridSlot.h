#pragma once

#include "Entity/Entity.h"
#include "Tile/TileType.h"

#include <stdint.h>

class GridSlot
{
public:
	enum class ContentType : uint8_t
	{
		Empty,
		Tile,
		Entity,
		EntityAndKey
	};

public:
	GridSlot() : m_contentType(ContentType::Empty), m_container{}
	{
	}
	explicit GridSlot(TileType tileType) : m_contentType(ContentType::Tile)
	{
		m_container.tileType = tileType;
	}
	explicit GridSlot(Entity* entity, bool containsKey = false)
	{
		m_contentType = containsKey ? ContentType::EntityAndKey : ContentType::Entity;
		m_container.entity = entity;
	}

	ContentType Type() const
	{
		return m_contentType;
	}

	TileType AsTile() const
	{
		return m_container.tileType;
	}
	Entity* AsEntity() const
	{
		return m_container.entity;
	}

private:
	union Container
	{
		TileType tileType;
		Entity* entity;
	};

	ContentType m_contentType;
	Container m_container;
};
