#pragma once

#include <stdint.h>
#include <vector>
#include <string_view>
using namespace std::literals;

//
// GameMap format:
// 1. Entity section
// 2. Tile map section
//
// Entity section
//   Section header         (size of section data)          (4 bytes)
//   Section data
//     Entity entry[]       (x, y, entity type)             (3 bytes)
//
// Tile map section
//   Section header         (size of section data)          (4 bytes)
//   Section data
//     Map size             (width, height)                 (2 bytes)
//     Tile data                                            (width * height bytes)
//       Tile entry[]       (tile details | tile type)      (1 bytes)
//                          ----6 bits----|--2 bits---
class GameMap
{
public:
#pragma pack(push, 1)
    struct EntityEntry
    {
        uint8_t x;
        uint8_t y;
        uint8_t entityType;
    };
#pragma pack(pop)

public:
    static constexpr uint8_t c_tileTypeMask = 0b00000011;
    static constexpr uint8_t c_tileDetailsMaskShift = 2;

public:
    GameMap() : m_width(0), m_height(0)
    {
    }

    GameMap(uint8_t width, uint8_t height)
        : m_width(width), m_height(height), m_tiles(width* height)
    {
    }

    GameMap& operator=(const GameMap&) = delete;

    GameMap(GameMap&&) = default;
    GameMap& operator=(GameMap&&) = default;

private:
    GameMap(const GameMap&) = default;

public:
    // TODO: refer to ResourceManager for Data path -or-
    // move map loading/saving to ResourceManager
    static constexpr std::string_view c_mapFileNamePrefix = "..\\Data\\Map\\map"sv;

    static GameMap LoadFromFile(int id);
    static void SaveToFile(const GameMap& gameMap, int id);

    uint8_t Width() const
    {
        return m_width;
    }
    uint8_t Height() const
    {
        return m_height;
    }

    std::vector<EntityEntry>& Entities()
    {
        return m_entities;
    }
    const std::vector<EntityEntry>& Entities() const
    {
        return m_entities;
    }

    std::vector<uint8_t>& Tiles()
    {
        return m_tiles;
    }
    const std::vector<uint8_t>& Tiles() const
    {
        return m_tiles;
    }

    void Clear();

    GameMap Clone() const;

private:
    uint8_t m_width;
    uint8_t m_height;

    std::vector<EntityEntry> m_entities;
    std::vector<uint8_t> m_tiles;
};
