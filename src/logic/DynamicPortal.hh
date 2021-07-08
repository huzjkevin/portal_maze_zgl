#pragma once

#include "../forward.hh"

class DynamicPortal
{
private:
    float width;
    float height;
    float guard;

    bool mainRenderEnabled;
    bool backRenderEnabled;

    bool mainCollisionEnabled;
    bool backCollisionEnabled;

    SharedMaze2D sourceMaze;
    int32_t sourceWall;
    int sourceHAnchor;
    float sourceHOffset;
    int sourceVAnchor;
    float sourceVOffset;
    int sourceZAnchor;
    float sourceZOffset;

    SharedMaze2D targetMaze;
    int32_t targetWall;
    int targetHAnchor;
    float targetHOffset;
    int targetVAnchor;
    float targetVOffset;
    int targetZAnchor;
    float targetZOffset;

    std::vector<CollisionObject*> obstacles;

    bool transformsLocked;

    glm::mat4 mainSourceTransform;
    glm::mat4 mainTargetTransform;

    glm::mat4 backSourceTransform;
    glm::mat4 backTargetTransform;

    bool mainRenderActive;
    bool backRenderActive;

    SharedPortal main;
    SharedPortal back;

    SharedCollisionObject mainSourceCollision;
    SharedCollisionObject mainTargetCollision;

    SharedCollisionObject backSourceCollision;
    SharedCollisionObject backTargetCollision;

    bool mainCollisionActive;
    bool backCollisionActive;

    SharedGate mainGate;
    SharedGate backGate;

public:
    DynamicPortal(SharedMaze2D sMaze, int32_t sWall, SharedMaze2D tMaze, int32_t tWall);
    virtual ~DynamicPortal();

    float getWidth() { return width; }
    void setWidth(float w) { width = w; }

    float getHeight() { return height; }
    void setHeight(float h) { height = h; }

    float getGuard() { return guard; }
    void setGuard(float g) { guard = g; }

    bool isMainRenderingEnabled() { return mainRenderEnabled; }
    void setMainRendering(bool m) { mainRenderEnabled = m; }

    bool isBackRenderingEnabled() { return backRenderEnabled; }
    void setBackRendering(bool b) { backRenderEnabled = b; }

    SharedMaze2D getSourceMaze() { return sourceMaze; }
    void setSourceMaze(SharedMaze2D s) { sourceMaze = s; }

    int32_t getSourceWall() { return sourceWall; }
    void setSourceWall(int32_t s) { sourceWall = s; }

    int getSourceHAnchor() { return sourceHAnchor; }
    void setSourceHAnchor(int s) { sourceHAnchor = s; }

    float getSourceHOffset() { return sourceHOffset; }
    void setSourceHOffset(float s) { sourceHOffset = s; }

    int getSourceVAnchor() { return sourceVAnchor; }
    void setSourceVAnchor(int s) { sourceVAnchor = s; }

    float getSourceVOffset() { return sourceVOffset; }
    void setSourceVOffset(float s) { sourceVOffset = s; }

    int getSourceZAnchor() { return sourceZAnchor; }
    void setSourceZAnchor(int s) { sourceZAnchor = s; }
    
    float getSourceZOffset() { return sourceZOffset; }
    void setSourceZOffset(float s) { sourceZOffset = s; }

    SharedMaze2D getTargetMaze() { return targetMaze; }
    void setTargetMaze(SharedMaze2D t) { targetMaze = t; }

    int32_t getTargetWall() { return targetWall; }
    void setTargetWall(int32_t t) { targetWall = t; }

    int getTargetHAnchor() { return targetHAnchor; }
    void setTargetHAnchor(int t) { targetHAnchor = t; }

    float getTargetHOffset() { return targetHOffset; }
    void setTargetHOffset(float t) { targetHOffset = t; }

    int getTargetVAnchor() { return targetVAnchor; }
    void setTargetVAnchor(int t) { targetVAnchor = t; }

    float getTargetVOffset() { return targetVOffset; }
    void setTargetVOffset(float t) { targetVOffset = t; }

    int getTargetZAnchor() { return targetZAnchor; }
    void setTargetZAnchor(int t) { targetZAnchor = t; }

    float getTargetZOffset() { return targetZOffset; }
    void setTargetZOffset(float t) { targetZOffset = t; }

    std::vector<CollisionObject*>* getObstacles() { return &obstacles; }

    bool areTransformsLocked() { return transformsLocked; }

    glm::mat4 getMainSourceTransform() { return mainSourceTransform; }
    glm::mat4 getMainTargetTransform() { return mainTargetTransform; }
    glm::mat4 getBackSourceTransform() { return backSourceTransform; }
    glm::mat4 getBackTargetTransform() { return backTargetTransform; }

    bool isMainRenderingActive() { return mainRenderActive; }
    bool isBackRenderingActive() { return backRenderActive; }

    SharedPortal getMain() { return main; }
    SharedPortal getBack() { return back; }

    SharedCollisionObject getMainSourceCollision() { return mainSourceCollision; }
    SharedCollisionObject getMainTargetCollision() { return mainTargetCollision; }
    SharedCollisionObject getBackSourceCollision() { return backSourceCollision; }
    SharedCollisionObject getBackTargetCollision() { return backTargetCollision; }

    bool isMainCollisionActive() { return mainCollisionActive; }
    bool isBackCollisionActive() { return backCollisionActive; }

    SharedGate getMainGate() { return mainGate; }
    SharedGate getBackGare() { return backGate; }

    void addObstacle(CollisionObject* o);
    void addObstacles(CollisionObject* s, CollisionObject* t);

    void enable(bool mainRender = true, bool backRender = true, bool mainCollision = true, bool backCollision = true);
    void disable(bool mainRender = false, bool backRender = false, bool mainCollision = false, bool backCollision = false);

    void lockTransforms();
    void update();

    void updateMainRender();
    void updateBackRender();

    void updateMainCollision();
    void updateBackCollision();

    void updateQueryHandlers();
    void updateObstacles();
};
