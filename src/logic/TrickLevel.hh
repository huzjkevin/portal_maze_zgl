#pragma once

#include "../forward.hh"

#include "Level.hh"

class TrickLevel : public Level
{
private:
    SharedGlobalLight globalLight;

    bool oldClick;
    std::vector<SharedMaze2D> mazes;

    SharedMazeMarkList marks;

    bool touchedTrigger;
    bool swapped;

    SharedCollisionObject triggerIn;
    SharedCollisionObject triggerOut;

    SharedMaze2D mainMaze;
    SharedMaze2D sideMaze;
    SharedMaze2D triggerMaze;

    /*SharedPortal main;
    SharedPortal back;

    SharedCollisionObject mainSourceCollision;
    SharedCollisionObject mainTargetCollision;

    SharedGate mainGate;

    SharedCollisionObject backSourceCollision;
    SharedCollisionObject backTargetCollision;

    SharedGate backGate;*/

    SharedDynamicPortal constPortal;
    SharedDynamicPortal threePortal;
    SharedDynamicPortal fourPortal;

public:
    TrickLevel();
    virtual ~TrickLevel();

    virtual void init();
    virtual void swap();

    virtual void tick(float elapsed);
    virtual void update(float elapsed);

    virtual void trigger(CollisionObject* actor, CollisionObject* other);
};
