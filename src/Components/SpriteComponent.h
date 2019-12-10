#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include "SDL2/SDL.h"
#include "Component.h"
#include "Game.h"
#include "Animation.h"
#include "TransformComponent.h"
#include <string>
#include <iostream>


class SpriteComponent : public Component {
    private:
    SDL_Texture* texture;
    SDL_Rect srcRect;
    SDL_Rect destRect;
    bool isAnimated;
    int numFrames;
    int animationSpeed;
    bool isFixed;
    std::map<std::string, Animation> animations;
    std::string currentAnimationName;
    unsigned int animationIndex = 0;
    public:
    TransformComponent* transform;
    SDL_RendererFlip spriteFlip = SDL_FLIP_NONE;

    SpriteComponent(std::string assetTextureId) {
        this->isAnimated = false;
        this->isFixed = false;
        SetTexture(assetTextureId);
    }

    SpriteComponent(std::string assetTextureId, bool isFixed) {
        this->isAnimated = false;
        this->isFixed = isFixed;
        SetTexture(assetTextureId);
    }

    SpriteComponent(std::string id, int numFrames, int animationSpeed, bool hasDirections, bool isFixed){
        isAnimated = true;
        this->numFrames = numFrames;
        this->animationSpeed = animationSpeed;
        this->isFixed = isFixed;
        if(hasDirections){
            Animation downAnimation(0, numFrames, animationSpeed);
            Animation rightAnimation(1, numFrames, animationSpeed);
            Animation leftAnimation(2, numFrames, animationSpeed);
            Animation upAnimation(3, numFrames, animationSpeed);
            this->animations.emplace("downAnimation", downAnimation);
            this->animations.emplace("rightAnimation", rightAnimation);
            this->animations.emplace("leftAnimation", leftAnimation);
            this->animations.emplace("upAnimation", upAnimation);

            this->animationIndex = 0;
            this->currentAnimationName = "downAnimation";
        } else {
            Animation singleAnimation = Animation(0, numFrames, animationSpeed);
            this->animations.emplace("SingleAnimation", singleAnimation);
            this->currentAnimationName = "SingleAnimation";
            SetTexture(id);
        }
        Play(this->currentAnimationName);
        SetTexture(id);
    }

    void Play(std::string animationName){
        numFrames = animations[animationName].numFrames;
        animationIndex = animations[animationName].index;
        animationSpeed = animations[animationName].animationSpeed;
        currentAnimationName = animationName;
    }

    void SetTexture(std::string assetTextureId){
        texture = Game::assetManager->GetTexture(assetTextureId);
    }

    void initialize() override {
        transform = owner->GetComponent<TransformComponent>();
        srcRect.x = 0;
        srcRect.y = 0;
        srcRect.w = transform->width;
        srcRect.h = transform->height;
    }

    void update(float deltaTime __attribute__((unused))) override {
        if(isAnimated){
            srcRect.x = srcRect.w * ( static_cast<int>(SDL_GetTicks()/ animationSpeed)  % numFrames);
        }
        srcRect.y = animationIndex * transform->height;

        destRect.x = static_cast<int>(transform->position.x) - (isFixed ? 0 : Game::camera.x);
        destRect.y = static_cast<int>(transform->position.y) - (isFixed ? 0 : Game::camera.y);
        destRect.w = transform->width * transform->scale;
        destRect.h = transform->height * transform->scale;
    }

    void render() override {
        TextureManager::Draw(texture, srcRect, destRect, spriteFlip);
    }
};


#endif