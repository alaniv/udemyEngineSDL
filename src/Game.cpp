#include "Game.h"
#include "AssetManager.h"
#include "./Components/TransformComponent.h"
#include "./Components/SpriteComponent.h"
#include "./Components/KeyboardControlComponent.h"
#include "./Components/ColliderComponent.h"
#include "./Components/TextLabelComponent.h"
#include "./Components/ProjectileEmitterComponent.h"
#include "Map.h"

EntityManager manager;
SDL_Renderer* Game::renderer;
AssetManager* Game::assetManager = new AssetManager(&manager);
SDL_Event Game::event;
SDL_Rect Game::camera = {0,0,Game::WINDOW_WIDTH, Game::WINDOW_HEIGHT};
Map *map;

Game::Game() : isRunning{false} {}
Game::~Game() {}

void Game::run() {
    initalize();
    Uint32 last_time = SDL_GetTicks();
    while(isRunning){
        processInput();
        Uint32 current_time = SDL_GetTicks();
        Uint32 delta_time = current_time - last_time;
        last_time = current_time;
        update(delta_time);
        render();
        SDL_Delay(FRAME_TIME);
    }
    finish();
}

Entity* mainPlayer = NULL;

/*
void Game::loadLevel(int levelNumber __attribute__((unused))){
    //Start including new assets to the assetsmanager list
    assetManager->AddTexture("tank-image", "./assets/images/tank-big-right.png");
    assetManager->AddTexture("chopper-image", "./assets/images/chopper-spritesheet.png");
    assetManager->AddTexture("radar-image", "./assets/images/radar.png");
    assetManager->AddTexture("jungle-tiletexture", "./assets/tilemaps/jungle.png");
    assetManager->AddTexture("heliport-image", "./assets/images/heliport.png");
    assetManager->AddTexture("projectile-image", "./assets/images/bullet-enemy.png");
    assetManager->AddFont("charriot-font", "./assets/fonts/charriot.ttf", 14);

    map = new Map("jungle-tiletexture", 2, 32);
    map->loadMap("./assets/tilemaps/jungle.map", 25, 20);

    //Start including entities and also components to them
    Entity& tankEntity(manager.addEntity("tank", ENEMY_LAYER));
    tankEntity.addComponent<TransformComponent>(150,495,5,0,32,32,1);
    tankEntity.addComponent<SpriteComponent>("tank-image");
    tankEntity.addComponent<ColliderComponent>("ENEMY", 240, 106, 32, 32);

    Entity& projectile(manager.addEntity("projectile", PROJECTILE_LAYER));
    projectile.addComponent<TransformComponent>(150+16, 495+16, 0, 0, 4, 4, 1);
    projectile.addComponent<SpriteComponent>("projectile-image");
    projectile.addComponent<ColliderComponent>("PROJECTILE", 150+16, 495+16, 4, 4);
    projectile.addComponent<ProjectileEmitterComponent>(50,270,200,true);

    player.addComponent<TransformComponent>(240,106,0,0,32,32,1);
    player.addComponent<SpriteComponent>("chopper-image", 2, 90, true, false);
    player.addComponent<KeyboardControlComponent>("up", "right", "down", "left", "space");
    player.addComponent<ColliderComponent>("PLAYER", 240, 106, 32, 32);
    
    Entity& radarEntity(manager.addEntity("radar", UI_LAYER));
    radarEntity.addComponent<TransformComponent>(720,15,0,0,64,64,1);
    radarEntity.addComponent<SpriteComponent>("radar-image", 8, 150, false, true);

    Entity& heliport(manager.addEntity("Heliport", OBSTACLE_LAYER));
    heliport.addComponent<TransformComponent>(470, 420, 0, 0,32,32,1);
    heliport.addComponent<SpriteComponent>("heliport-image");
    heliport.addComponent<ColliderComponent>("LEVEL_COMPLETE", 470, 420, 32, 32);

    Entity& labelLevelName(manager.addEntity("LabelLevelName", UI_LAYER));
    labelLevelName.addComponent<TextLabelComponent>(10,10,"First Level...", "charriot-font", WHITE_COLOR);
}
*/

void Game::loadLevel(int levelNumber) {
    sol::state lua;
    lua.open_libraries(sol::lib::base, sol::lib::os, sol::lib::math);

    std::string levelName = "Level" + std::to_string(levelNumber);
    lua.script_file("./assets/scripts/" + levelName + ".lua");

    ///////////////////////////////////////////////////////////////////////////
    // LOAD LIST OF ASSETS FROM LUA CONFIG FILE
    ///////////////////////////////////////////////////////////////////////////
    sol::table levelData = lua[levelName];
    sol::table levelAssets = levelData["assets"];

    unsigned int assetIndex = 0;
    while (true) {
        sol::optional<sol::table> existsAssetIndexNode = levelAssets[assetIndex];
        if (existsAssetIndexNode == sol::nullopt) {
            break;
        } else {
            sol::table asset = levelAssets[assetIndex];
            std::string assetType = asset["type"];
            if (assetType.compare("texture") == 0) {
                std::string assetId = asset["id"];
                std::string assetFile = asset["file"];
                assetManager->AddTexture(assetId, assetFile.c_str());
            }
            if (assetType.compare("font") == 0) {
                std::string assetId = asset["id"];
                std::string assetFile = asset["file"];
                int fontSize = asset["fontSize"];
                assetManager->AddFont(assetId, assetFile.c_str(), fontSize);
            }
        }
        assetIndex++;
    }

    ///////////////////////////////////////////////////////////////////////////
    // LOAD MAP AND TILE INFORMATION FROM LUA CONFIG FILE
    ///////////////////////////////////////////////////////////////////////////
    sol::table levelMap = levelData["map"];
    std::string mapTextureId = levelMap["textureAssetId"];
    std::string mapFile = levelMap["file"];

    map = new Map(
        mapTextureId,
        static_cast<int>(levelMap["scale"]),
        static_cast<int>(levelMap["tileSize"])
    );

    map->loadMap(
        mapFile,
        static_cast<int>(levelMap["mapSizeX"]),
        static_cast<int>(levelMap["mapSizeY"])
    );

    ///////////////////////////////////////////////////////////////////////////
    // LOAD ENTITIES AND COMPONENTS FROM LUA CONFIG FILE
    ///////////////////////////////////////////////////////////////////////////
    sol::table levelEntities = levelData["entities"];
    unsigned int entityIndex = 0;
    while (true) {
        sol::optional<sol::table> existsEntityIndexNode = levelEntities[entityIndex];
        if (existsEntityIndexNode == sol::nullopt) {
            break;
        }
        else {
            sol::table entity = levelEntities[entityIndex];
            std::string entityName = entity["name"];
            LayerType entityLayerType = static_cast<LayerType>(static_cast<int>(entity["layer"]));

            // Add new entity
            auto& newEntity(manager.addEntity(entityName, entityLayerType));

            // Add Transform Component
            sol::optional<sol::table> existsTransformComponent = entity["components"]["transform"];
            if (existsTransformComponent != sol::nullopt) {
                newEntity.addComponent<TransformComponent>(
                    static_cast<int>(entity["components"]["transform"]["position"]["x"]),
                    static_cast<int>(entity["components"]["transform"]["position"]["y"]),
                    static_cast<int>(entity["components"]["transform"]["velocity"]["x"]),
                    static_cast<int>(entity["components"]["transform"]["velocity"]["y"]),
                    static_cast<int>(entity["components"]["transform"]["width"]),
                    static_cast<int>(entity["components"]["transform"]["height"]),
                    static_cast<int>(entity["components"]["transform"]["scale"])
                );
            }

            // Add sprite component
            sol::optional<sol::table> existsSpriteComponent = entity["components"]["sprite"];
            if (existsSpriteComponent != sol::nullopt) {
                std::string textureId = entity["components"]["sprite"]["textureAssetId"];
                bool isAnimated = entity["components"]["sprite"]["animated"];
                if (isAnimated) {
                    newEntity.addComponent<SpriteComponent>(
                        textureId,
                        static_cast<int>(entity["components"]["sprite"]["frameCount"]),
                        static_cast<int>(entity["components"]["sprite"]["animationSpeed"]),
                        static_cast<bool>(entity["components"]["sprite"]["hasDirections"]),
                        static_cast<bool>(entity["components"]["sprite"]["fixed"])
                    );
                } else {
                    newEntity.addComponent<SpriteComponent>(textureId, false);
                }
            }

            // Add input control component
            sol::optional<sol::table> existsInputComponent = entity["components"]["input"];
            if (existsInputComponent != sol::nullopt) {
                sol::optional<sol::table> existsKeyboardInputComponent = entity["components"]["input"]["keyboard"];
                if (existsKeyboardInputComponent != sol::nullopt) {
                    std::string upKey = entity["components"]["input"]["keyboard"]["up"];
                    std::string rightKey = entity["components"]["input"]["keyboard"]["right"];
                    std::string downKey = entity["components"]["input"]["keyboard"]["down"];
                    std::string leftKey = entity["components"]["input"]["keyboard"]["left"];
                    std::string shootKey = entity["components"]["input"]["keyboard"]["shoot"];
                    newEntity.addComponent<KeyboardControlComponent>(upKey, rightKey, downKey, leftKey, shootKey);
                }
            }

            // Add collider component
            sol::optional<sol::table> existsColliderComponent = entity["components"]["collider"];
            if (existsColliderComponent != sol::nullopt) {
                std::string colliderTag = entity["components"]["collider"]["tag"];
                newEntity.addComponent<ColliderComponent>(
                    colliderTag,
                    static_cast<int>(entity["components"]["transform"]["position"]["x"]),
                    static_cast<int>(entity["components"]["transform"]["position"]["y"]),
                    static_cast<int>(entity["components"]["transform"]["width"]),
                    static_cast<int>(entity["components"]["transform"]["height"])
                );
            }

            // Add projectile emitter component
            sol::optional<sol::table> existsProjectileEmitterComponent = entity["components"]["projectileEmitter"];
            if (existsProjectileEmitterComponent != sol::nullopt) {
                int parentEntityXPos = entity["components"]["transform"]["position"]["x"];
                int parentEntityYPos = entity["components"]["transform"]["position"]["y"];
                int parentEntityWidth = entity["components"]["transform"]["width"];
                int parentEntityHeight = entity["components"]["transform"]["height"];
                int projectileWidth = entity["components"]["projectileEmitter"]["width"];
                int projectileHeight = entity["components"]["projectileEmitter"]["height"];
                int projectileSpeed = entity["components"]["projectileEmitter"]["speed"];
                int projectileRange = entity["components"]["projectileEmitter"]["range"];
                int projectileAngle = entity["components"]["projectileEmitter"]["angle"];
                bool projectileShouldLoop = entity["components"]["projectileEmitter"]["shouldLoop"];
                std::string textureAssetId = entity["components"]["projectileEmitter"]["textureAssetId"];
                Entity& projectile(manager.addEntity("projectile", PROJECTILE_LAYER));
                projectile.addComponent<TransformComponent>(
                    parentEntityXPos + (parentEntityWidth / 2),
                    parentEntityYPos + (parentEntityHeight / 2),
                    0,
                    0,
                    projectileWidth,
                    projectileHeight,
                    1
                );
                projectile.addComponent<SpriteComponent>(textureAssetId);
                projectile.addComponent<ProjectileEmitterComponent>(
                    projectileSpeed,
                    projectileAngle,
                    projectileRange,
                    projectileShouldLoop
                );
                projectile.addComponent<ColliderComponent>(
                    "PROJECTILE",
                    parentEntityXPos,
                    parentEntityYPos,
                    projectileWidth,
                    projectileHeight
                );
            }
        }
        entityIndex++;
    }

    mainPlayer = manager.getEntityByName("player");
}

void Game::initalize(){
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
        throw std::runtime_error(std::string("SDL_INIT error: ") + SDL_GetError());
    if(TTF_Init() != 0)
        throw std::runtime_error(std::string("TTF_INIT error: ") + TTF_GetError());
    window = SDL_CreateWindow( "Game", 
       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
       WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_BORDERLESS
    );    
    if(window == nullptr)
        throw std::runtime_error(std::string("SDL_CreateWindow error: ") + SDL_GetError());
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == nullptr)
        throw std::runtime_error(std::string("SDL_CreateRenderer error: ") + SDL_GetError());
    isRunning = true;
    loadLevel(1);
}

void Game::processInput(){
    SDL_PollEvent(&event);
    switch(event.type){
        case SDL_QUIT:{
            isRunning = false;
            break;
        }
        case SDL_KEYDOWN:{
            if(event.key.keysym.sym == SDLK_ESCAPE){
                isRunning = false;
            }
            break;
        }
        default:{
            break;
        }
    }
}

void Game::update(Uint32 deltaTime __attribute__((unused))){
    manager.update(deltaTime/1000.0f);
    handleCameraMovement();
    checkCollisions();
}

void Game::render(){
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);
    if(manager.hasNoEntities())
        return;
    manager.render();
    SDL_RenderPresent(renderer);
}

void Game::finish(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::handleCameraMovement(){
    if (mainPlayer) {

    TransformComponent * playerTransf = mainPlayer->GetComponent<TransformComponent>();
    camera.x = playerTransf->position.x - (Game::WINDOW_WIDTH/2);
    camera.y = playerTransf->position.y - (Game::WINDOW_HEIGHT/2);

    camera.x = camera.x < 0 ? 0 : camera.x;
    camera.y = camera.y < 0 ? 0 : camera.y;
    camera.w = camera.x > camera.w ? camera.w : camera.x;
    camera.h = camera.y > camera.h ? camera.h : camera.y;
    }
}

// void Game::checkCollisions(){
//     std::string collisionTagType = manager.checkEntityCollisions(player);
//     if(collisionTagType.compare("enemy") == 0){
//         isRunning = false;
//     }
// }
void Game::checkCollisions(){
    CollisionType collisionType = manager.CheckCollisions();
    if(collisionType == PLAYER_ENEMY_COLLISION){
        ProcessGameOver();
    }
    if(collisionType == PLAYER_PROJECTILE_COLLISION){
        std::cout << "projectile death" << std::endl;
        ProcessGameOver();
    }
    if(collisionType == PLAYER_LEVEL_COMPLETE_COLLISION){
        ProcessNextLevel();
    }
}

void Game::ProcessGameOver(){
    std::cout << "Game Over" << std::endl;
    isRunning = false;
}

void Game::ProcessNextLevel(){
    std::cout << "Next Level" << std::endl;
    isRunning = false;
}