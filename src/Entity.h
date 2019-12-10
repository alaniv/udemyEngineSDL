#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include <string>
#include <map>
#include <typeinfo> 
#include "Component.h"

#include "Constants.h"

//FD
class EntityManager;

class Entity {
public:
    std::string name;
    Entity(EntityManager& entManager);
    Entity(EntityManager& entManager, std::string entName, LayerType layer);
    LayerType layer;
    bool isEntityActive() const;
    void update(float deltaTime __attribute__((unused)));
    void render();
    void destroy();
    template <typename T, typename... TArgs>
    T& addComponent(TArgs&&... args){
       T* newComponent(new T(std::forward<TArgs>(args)...));
       newComponent->owner = this;
       components.emplace_back(newComponent);
       componentTypeMap[&typeid(*newComponent)] = newComponent;
       newComponent->initialize();
       return *newComponent;
    }

    template <typename T>
    bool HasComponent() const {
        return componentTypeMap.count(&typeid(T));
    }

    template <typename T>
    T* GetComponent(){
        return static_cast<T*>(componentTypeMap[&typeid(T)]);
    }

private:
    EntityManager& manager;
    bool isActive;
    std::vector<Component*> components;
    std::map<const std::type_info*, Component*> componentTypeMap;
    

};

#endif