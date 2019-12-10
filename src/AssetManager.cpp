#include "AssetManager.h"

AssetManager::AssetManager(EntityManager* manager) : manager(manager){

}

void AssetManager::ClearData(){
    textures.clear();
    fonts.clear();
}

void AssetManager::AddTexture(std::string textId, const char* filePath){
    textures.emplace(textId, TextureManager::LoadTexture(filePath));
}

SDL_Texture* AssetManager::GetTexture(std::string textId){
    return textures[textId];
}

void AssetManager::AddFont(std::string fontId, const char* filePath, int fontSize){
    fonts.emplace(fontId, FontManager::LoadFont(filePath, fontSize));
}

TTF_Font* AssetManager::GetFont(std::string fontId){
    return fonts[fontId];
}
