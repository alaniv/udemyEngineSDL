#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <glm/glm.hpp>
#include <LUA/sol.hpp>

#include "Component.h"
#include "EntityManager.h"
#include "Entity.h"

#include <stdexcept>  
#include <iostream>  

#include "Constants.h"

//FD
class AssetManager;

class Game {
public:
    static const unsigned int WINDOW_WIDTH = 800;
    static const unsigned int WINDOW_HEIGHT = 600;
    static const unsigned int FPS = 60;
    static const unsigned int FRAME_TIME = 1000/FPS;
    static SDL_Event event;
    static SDL_Rect camera;
    Game();
    ~Game();
    void run();
    static SDL_Renderer *renderer;    
    static AssetManager *assetManager;    
    void initalize();
    void processInput();
    void update(Uint32 deltaTime);
    void render();
    void finish();
    void loadLevel(int levelNumber);
    void handleCameraMovement();
    void checkCollisions();
private:
    bool isRunning;
    SDL_Window *window;
    void ProcessGameOver();
    void ProcessNextLevel();
};
#endif