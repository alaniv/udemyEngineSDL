#include "Game.h"
#include "Map.h"
#include "EntityManager.h"
#include "Entity.h"
#include <fstream>
#include "Components/TileComponent.h"

extern EntityManager manager;

Map::Map(std::string textureId, int scale, int tileSize){
    this->textureId = textureId;
    this->scale = scale;
    this->tileSize = tileSize;
}

void Map::loadMap(std::string filePath, int mapSizeX, int mapSizeY){
    std::fstream mapFile;
    mapFile.open(filePath);

    for(int y=0; y < mapSizeY; y++){
        for(int x=0; x<mapSizeX; x++){
            char ch;
            mapFile.get(ch);
            int sourceRectY = atoi(&ch) * tileSize; 
            mapFile.get(ch);
            int sourceRectX = atoi(&ch) * tileSize;
            addTile(sourceRectX, sourceRectY, x*(scale * tileSize), y*(scale * tileSize));
            mapFile.ignore();
        }
    }
    mapFile.close();
}

void Map::addTile(int sourceRectX, int sourceRectY, int x, int y){
    Entity& newTile(manager.addEntity("Tile", TILEMAP_LAYER));
    newTile.addComponent<TileComponent>(sourceRectX, sourceRectY, x, y, tileSize, scale, textureId);
}
