#include "Entity.h"

Entity::Entity(EntityManager& manager) : 
    manager(manager), isActive(true){}
    
Entity::Entity(EntityManager& manager, std::string name, LayerType layer) :
    name(name), layer(layer), manager(manager), isActive(true){}

void Entity::update(float deltaTime __attribute__((unused))){
    for(auto& component: components)
        component->update(deltaTime);
}

void Entity::render(){
    for(auto& component: components)
        component->render();
}

void Entity::destroy(){
    isActive = false;
}

bool Entity::isEntityActive() const {
    return isActive;
}