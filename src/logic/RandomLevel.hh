#pragma once

#include "../forward.hh"

#include "Level.hh"

class RandomLevel : public Level
{
private:
    SharedGlobalLight globalLight;
    bool won;

    bool oldClick;
    std::vector<SharedMaze2D> mazes;

    SharedMazeMarkList marks;

public:
    RandomLevel();
    virtual ~RandomLevel();

    virtual void init();

    virtual void tick(float elapsed);
    virtual void update(float elapsed);

    virtual void trigger(CollisionObject* actor, CollisionObject* other);
};
