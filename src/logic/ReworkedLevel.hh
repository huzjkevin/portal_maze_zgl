#pragma once

#include "../forward.hh"

#include "Level.hh"

class ReworkedLevel : public Level
{
private:
    SharedGlobalLight globalLight;
    bool won;

    bool oldClick;
    std::vector<SharedMaze2D> mazes;

    SharedMazeMarkList marks;

public:
    ReworkedLevel();
    virtual ~ReworkedLevel();

    virtual void init();

    virtual void initSpherePlayer(float scale, glm::vec3 position, float cameraDistance);

    virtual void tick(float elapsed);
    virtual void update(float elapsed);

    virtual void trigger(CollisionObject* actor, CollisionObject* other);

    virtual void createPartialPair(
                                  bool visMain,
                                  bool visBack,
                                  bool colMain,
                                  bool colBack,
                                  SharedMaze2D sourceMaze,
                                  int32_t sourceWall,
                                  CollisionObject* sourceBody,
                                  SharedMaze2D targetMaze,
                                  int32_t targetWall,
                                  CollisionObject* targetBody,
                                  float width = DEFAULT_PORTAL_WIDTH,
                                  float height = DEFAULT_PORTAL_HEIGHT,
                                  int sourceHAnchor = WALL_CENTER,
                                  float sourceHOffset = 0.0f,
                                  int targetHAnchor = WALL_CENTER,
                                  float targetHOffset = 0.0f,
                                  int sourceVAnchor = VERT_BOTTOM,
                                  float sourceVOffset = 0.01f,
                                  int targetVAnchor = VERT_BOTTOM,
                                  float targetVOffset = 0.01f,
                                  float guard = DEFAULT_GUARD,
                                  int sourceZAnchor = Z_ANCHOR_INNER,
                                  float sourceZOffset = 0.0f,
                                  int targetZAnchor = Z_ANCHOR_INNER,
                                  float targetZOffset = 0.0f);
};
