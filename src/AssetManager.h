#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include "EntityManager.h"
#include "TextureManager.h"
#include "FontManager.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include <map>
#include <string>

class AssetManager {
    private:
    EntityManager* manager;
    std::map<std::string, SDL_Texture*> textures;
    std::map<std::string, TTF_Font*> fonts;
    public:
    AssetManager(EntityManager* manager);
    ~AssetManager();
    void ClearData();
    void AddTexture(std::string textId, const char* filePath);
    void AddFont(std::string fontId, const char* filePath, int fontSize);
    SDL_Texture* GetTexture(std::string textId);
    TTF_Font* GetFont(std::string fontId);
};


#endif