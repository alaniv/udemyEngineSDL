#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include "Entity.h"
#include "Component.h"
#include <vector>
#include <string>
#include "Constants.h"

class EntityManager {
public:
    void update(float deltaTime);
    void render();
    bool hasNoEntities();
    Entity& addEntity(std::string entityName, LayerType layer);
    std::vector<Entity*> getEntities() const;
    std::vector<Entity*> getEntitiesByLayer(LayerType layer) const;
    unsigned int getEntitiesCount();
    void clearData();
    //std::string checkEntityCollisions(Entity& entity) const;
    CollisionType CheckCollisions() const;
    void destroyInactiveEntities();
    Entity* getEntityByName(std::string entityName) const;
private:
    std::vector<Entity*> entities;
};
#endif