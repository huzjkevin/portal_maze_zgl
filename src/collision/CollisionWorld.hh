#pragma once

#include "../forward.hh"

struct CustomBroadphaseFilter : public btOverlapFilterCallback
{
    virtual bool needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const;
};

class CollisionWorld
{
private:
    static SharedCollisionWorld instance;

    CustomBroadphaseFilter* filter;
    btHashedOverlappingPairCache* cache;
    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* config;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* world;

    std::vector<SharedCollisionObject> objects;

    std::vector<CollisionObject*> travelling;
    std::vector<bool> confirmed;

public:
    CollisionWorld();
    virtual ~CollisionWorld();

    static SharedCollisionWorld getInstance() { return instance; }

    static void initInstance();
    static void reset();

    void init();

    btVector3 getGravity() const;
    void setGravity(btVector3 gravity);

    void tick(float elapsed);

    void collisionStart();
    void collisionDetected(CollisionObject* actor, CollisionObject* other);
    void collisionEnd();

    std::vector<SharedCollisionObject>* getObjects();

    virtual size_t getObjectCount() const;
    virtual SharedCollisionObject getObject(size_t index);

    virtual void insertObject(size_t index, SharedCollisionObject object);
    virtual void addObject(SharedCollisionObject object);

    virtual void removeObject(size_t index);
    virtual void removeObject(SharedCollisionObject object);

    virtual void clearPortals();
};
