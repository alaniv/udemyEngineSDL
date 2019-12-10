#ifndef COMPONENT_H
#define COMPONENT_H

//FD
class Entity;

class Component{
public:
    Entity* owner;
    virtual ~Component(){};
    virtual void initialize(){};
    virtual void update(float deltaTime __attribute__((unused))){};
    virtual void render(){};
private:
};

#endif