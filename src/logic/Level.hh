#pragma once

#include "../forward.hh"

class AnimationComponent;
class Level
{
public:
    Level() {}
    virtual ~Level() {}

    virtual void init();

    virtual void tick(float elapsed);
    virtual void update(float elapsed);

    virtual void trigger(CollisionObject* actor, CollisionObject* other);

    virtual SharedGlobalLight initGlobalLight(glm::vec3 dir, glm::vec3 color, float ambient, float intensity);

    virtual void initPlayer(float scale, glm::vec3 position, float cameraDistance);
    virtual void initGround(float w = 100.0f, float h = 100.0f, float t = 1.0f, glm::vec3 pos = glm::vec3(0.0f, -1.0f, 0.0f));
    virtual void createPortalPair(SharedMaze2D sourceMaze,
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
    virtual void createMirror(SharedMaze2D maze,
                              int32_t wall,
                              float width = DEFAULT_PORTAL_WIDTH,
                              float height = DEFAULT_PORTAL_HEIGHT,
                              int hAnchor = WALL_CENTER,
                              float hOffset = 0.0f,
                              int vAnchor = VERT_BOTTOM,
                              float vOffset = 0.01f);

private:
    std::shared_ptr<AnimationComponent> animation;
};
