#pragma once

#include "../forward.hh"
#include "DynamicObject.hh"

class PlayerObject : public DynamicObject
{
public:
    PlayerObject(SharedCollisionObject c);
    virtual ~PlayerObject();

    void move(glm::vec3 direction);
    virtual void update(float elapsed);
};
