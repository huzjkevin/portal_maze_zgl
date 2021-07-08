#pragma once

#include "../forward.hh"

#include "Level.hh"

class FinalLevel : public Level
{
private:
    SharedGlobalLight globalLight;
    bool won;

    bool key1Picked;
    bool key2Picked;

    float tileW;
    float tileH;

    bool oldClick;
    std::vector<SharedMaze2D> mazes;

    SharedMazeMarkList marks;

    SharedDynamicPortal portal1;
    SharedDynamicPortal portal2;
    SharedDynamicPortal portal3;
    SharedDynamicPortal portal4;
    SharedDynamicPortal portal5;
    SharedDynamicPortal portal6;
    SharedDynamicPortal portal7;
    SharedDynamicPortal portal8;
    SharedDynamicPortal portal9;
    SharedDynamicPortal portal10;

    SharedDrawable key1Object;
    SharedDrawable key2Object;

    SharedCollisionObject key1Trigger;
    SharedCollisionObject key2Trigger;
    SharedCollisionObject exitTrigger;

public:
    FinalLevel();
    virtual ~FinalLevel();

    virtual void init();

    virtual void tick(float elapsed);
    virtual void update(float elapsed);

    virtual void trigger(CollisionObject* actor, CollisionObject* other);
};
