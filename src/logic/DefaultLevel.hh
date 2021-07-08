#pragma once

#include "../forward.hh"

#include "Level.hh"

class DefaultLevel : public Level
{
private:
    SharedGlobalLight globalLight;
    bool won;

    bool oldClick;
    std::vector<SharedMaze2D> mazes;

    SharedMazeMarkList marks;
    SharedDrawable drawableKey;

public:
    DefaultLevel();
    virtual ~DefaultLevel();
    
    virtual void init();

    virtual void tick(float elapsed);
    virtual void update(float elapsed);

    virtual void trigger(CollisionObject* actor, CollisionObject* other);
};
