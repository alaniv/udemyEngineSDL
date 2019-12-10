#include "EntityManager.h"
#include "Collision.h"
#include "Components/ColliderComponent.h"

void EntityManager::clearData(){
    for(auto& entity : entities)
        entity->destroy();
}

bool EntityManager::hasNoEntities(){
    return entities.size() == 0;
}

void EntityManager::update(float deltaTime){
    for(auto& entity : entities)
        entity->update(deltaTime);
    destroyInactiveEntities();
}

void EntityManager::destroyInactiveEntities(){
    for(size_t i = 0; i < entities.size(); i++){
        if(!entities[i]->isEntityActive()){
            entities.erase(entities.begin() + i);
        }
    }
}

void EntityManager::render(){
    for(int layerNumber = 0; layerNumber < NUM_LAYERS; layerNumber++){
        for(auto& entity : getEntitiesByLayer(static_cast<LayerType>(layerNumber)))
            entity->render();
    }

}

Entity& EntityManager::addEntity(std::string entityName, LayerType layer){
    Entity* entity = new Entity(*this, entityName, layer);
    entities.emplace_back(entity);
    return *entity;
}

std::vector<Entity*> EntityManager::getEntities() const {
    return entities;
}

std::vector<Entity*> EntityManager::getEntitiesByLayer(LayerType layer) const {
    std::vector<Entity*> selectedEntities;
    for(auto& entity : entities){
        if(entity->layer == layer){
            selectedEntities.emplace_back(entity);
        }
    }
    return selectedEntities;
}

unsigned int EntityManager::getEntitiesCount(){
    return entities.size();
}

// std::string EntityManager::checkEntityCollisions(Entity& myEntity) const {
//     ColliderComponent* myCollider = myEntity.GetComponent<ColliderComponent>();
//     for(auto& entity : entities){
//         if(entity->name != myEntity.name && entity->name != "Tile" && entity->HasComponent<ColliderComponent>()){
//             ColliderComponent* otherCollider = entity->GetComponent<ColliderComponent>();
//             if(Collision::CheckRectangleCollision(myCollider->collider, otherCollider->collider)){
//                 return otherCollider->colliderTag;
//             }
//         }
//     }
//     return std::string();
// }

CollisionType EntityManager::CheckCollisions() const {
    for(auto& thisEntity : entities){
        if(thisEntity->HasComponent<ColliderComponent>()){
            ColliderComponent *thisCollider = thisEntity->GetComponent<ColliderComponent>();
            for(auto& thatEntity : entities){
                if(thisEntity->name != thatEntity->name && thatEntity->HasComponent<ColliderComponent>()){
                    ColliderComponent* thatCollider = thatEntity->GetComponent<ColliderComponent>();
                    //std::cout << thisCollider->colliderTag << std::endl;
                    //std::cout << thatCollider->colliderTag << std::endl;
                    if(Collision::CheckRectangleCollision(thisCollider->collider, thatCollider->collider)){
                        if(thisCollider->colliderTag == "PLAYER" && thatCollider->colliderTag == "ENEMY"){
                            //std::cout << "test" << std::endl;
                            return PLAYER_ENEMY_COLLISION;
                        } else if(thisCollider->colliderTag == "PLAYER" && thatCollider->colliderTag == "PROJECTILE"){
                            return PLAYER_PROJECTILE_COLLISION;
                        } else if(thisCollider->colliderTag == "ENEMY" && thatCollider->colliderTag == "FRIENDLY_PROJECTILE"){
                            return ENEMY_PROJECTILE_COLLISION;
                        } else if(thisCollider->colliderTag == "PLAYER" && thatCollider->colliderTag == "LEVEL_COMPLETE"){
                            return PLAYER_LEVEL_COMPLETE_COLLISION;
                        }
                    }
                }
            }
        }
    }
    return NO_COLLISION;
}

Entity* EntityManager::getEntityByName(std::string entityName) const {
    for (auto* entity: entities) {
        if (entity->name.compare(entityName) == 0) {
            return entity;
        }
    }
    return NULL;
}