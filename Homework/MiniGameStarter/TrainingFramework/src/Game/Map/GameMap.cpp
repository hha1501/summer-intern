#include "GameMap.h"

#include <fstream>
#include <string>
#include <assert.h>

GameMap GameMap::LoadFromFile(int id)
{
    std::string fileName = std::string(c_mapFileNamePrefix) + std::to_string(id);
    std::ifstream mapFile{ fileName, std::ios_base::binary };
    assert(mapFile.is_open());

    // Read section headers and ensure map file is valid
    uint32_t entitySectionSize;
    mapFile.read(reinterpret_cast<char*>(&entitySectionSize), sizeof(entitySectionSize));
    assert(entitySectionSize % sizeof(EntityEntry) == 0);

    mapFile.seekg(sizeof(entitySectionSize) + entitySectionSize);

    uint32_t tileSectionSize;
    mapFile.read(reinterpret_cast<char*>(&tileSectionSize), sizeof(tileSectionSize));

    uint8_t mapWidth, mapHeight;
    mapFile.read(reinterpret_cast<char*>(&mapWidth), sizeof(mapWidth));
    mapFile.read(reinterpret_cast<char*>(&mapHeight), sizeof(mapHeight));
    std::streampos tileDataStart = mapFile.tellg();

    uint32_t tileDataSize = tileSectionSize - (sizeof(mapWidth) + sizeof(mapHeight));
    assert(tileDataSize == mapWidth * mapHeight);

    mapFile.seekg(0, mapFile.end);
    std::streampos fileEnd = mapFile.tellg();
    assert((fileEnd - tileDataStart) == tileDataSize);

    // Preallocate GameMap
    GameMap gameMap{mapWidth, mapHeight};
    std::vector<EntityEntry>& entities = gameMap.Entities();
    entities.assign(entitySectionSize / sizeof(EntityEntry), EntityEntry{});

    // Read entity data
    mapFile.seekg(sizeof(entitySectionSize));
    mapFile.read(reinterpret_cast<char*>(entities.data()), entitySectionSize);

    std::vector<uint8_t>& tiles = gameMap.Tiles();

    // Read tile data
    mapFile.seekg(tileDataStart);
    mapFile.read(reinterpret_cast<char*>(tiles.data()), tileDataSize);

    return gameMap;
}

void GameMap::SaveToFile(const GameMap& gameMap, int id)
{
    std::string fileName = std::string(c_mapFileNamePrefix) + std::to_string(id);
    std::ofstream mapFile{ fileName, std::ios_base::binary };
    assert(mapFile.is_open());

    // Write entity section
    const std::vector<EntityEntry>& entities = gameMap.Entities();
    uint32_t entitySectionSize = entities.size() * sizeof(EntityEntry);

    mapFile.write(reinterpret_cast<const char*>(&entitySectionSize), sizeof(entitySectionSize));
    mapFile.write(reinterpret_cast<const char*>(entities.data()), entitySectionSize);

    // Write tile section
    const std::vector<uint8_t>& tiles = gameMap.Tiles();
    uint32_t tileSectionSize = tiles.size() + sizeof(uint8_t) + sizeof(uint8_t);

    mapFile.write(reinterpret_cast<const char*>(&tileSectionSize), sizeof(tileSectionSize));
    mapFile.write(reinterpret_cast<const char*>(&gameMap.m_width), sizeof(uint8_t));
    mapFile.write(reinterpret_cast<const char*>(&gameMap.m_height), sizeof(uint8_t));
    mapFile.write(reinterpret_cast<const char*>(tiles.data()), tiles.size());
}

void GameMap::Clear()
{
    m_entities.clear();
    m_tiles.clear();
}

GameMap GameMap::Clone() const
{
    GameMap copy = *this;
    return copy;
}
