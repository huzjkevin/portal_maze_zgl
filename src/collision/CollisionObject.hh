#pragma once

#include "../forward.hh"

class CollisionObject
{
private:
    btCollisionShape* shape;
    btDefaultMotionState* state;
    btRigidBody* body;

    uint32_t type;
    bool travelling;

    bool passed;
    int progress;

    Gate* gate;
    glm::mat4 mirror;

    CameraObject* camera;

public:
    CollisionObject();
    virtual ~CollisionObject();

    void initStatic(btCollisionShape* s, glm::mat4 model);
    void initTrigger(btCollisionShape* s, glm::mat4 model, uint32_t t = COLLISION_TRIGGER);
    void initDynamic(btCollisionShape* s, glm::mat4 model, btScalar mass, btScalar friction, btScalar restitution, btScalar linearDamping, btScalar angularDamping);

    void setAngularFactor(btVector3 factor);
    void setAngularFactor(float factor);

    btCollisionShape* getShape() { return shape; }
    btDefaultMotionState* getState() { return state; }
    btRigidBody* getBody() { return body; }

    uint32_t getType() const { return type; }
    void setType(uint32_t t) { type = t; }

    bool isTravelling() const { return travelling; }
    void setTravelling(bool t) { travelling = t; }

    bool hasPassed() const { return passed; }
    void setPassed(bool p) { passed = p; }

    int getProgress() { return progress; }
    void setProgress(int p) { progress = p; }

    Gate* getGate() { return gate; }
    void setGate(Gate* g) { gate = g; }

    glm::mat4 getModel() const;
    void setModel(glm::mat4 model);

    glm::vec3 getPosition() const;

    glm::mat4 getMirror() const { return mirror; }

    CameraObject* getCamera() { return camera; }
    void setCamera(CameraObject* c) { camera = c; }

    void enter(Gate* g);
    void teleporting();
    void exit();

    void adjustToCamera();
};
