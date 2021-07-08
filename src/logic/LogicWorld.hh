#pragma once

#include "../forward.hh"

class LogicWorld
{
private:
    static SharedLogicWorld instance;

    Game* game;
    SharedLevel level;

    SharedPlayerObject player;
    SharedCameraObject camera;

    std::vector<SharedDynamicObject> objects;
    std::vector<SharedGate> gates;

    bool escape;
    bool oldEscape;

public:
    LogicWorld(Game* g);
    virtual ~LogicWorld();

    static SharedLogicWorld getInstance() { return instance; }

    static void initInstance(Game* g);
    static void reset();

    void init();

    Game* getGame() { return game; }

    SharedPlayerObject getPlayer() { return player; }
    void setPlayer(SharedPlayerObject p) { player = p; }

    SharedCameraObject getCamera() { return camera; }
    void setCamera(SharedCameraObject c) { camera = c; }

    std::vector<SharedDynamicObject>* getObjects() { return &objects; }
    std::vector<SharedGate>* getGates() { return &gates; }

    void setEscape(bool escape) { this->escape = escape; }

    void control(float elapsed);
    void resize(int w, int h);

    void tick(float elapsed);
    void update(float elapsed);

    void trigger(CollisionObject* actor, CollisionObject* other);
};
