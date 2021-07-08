#include "CollisionWorld.hh"

#include "CollisionObject.hh"
#include "Gate.hh"

#include "../logic/LogicWorld.hh"

static bool collides[64] = {
    false, true, true, false, false, false,
    true, true, true, true, true, true,
    true, true, true, true, true, true,
    false, true, true, false, false, false,
    false, true, true, false, false, false,
    false, true, true, false, false, false
};

inline static int index(int row, int col) {
    return (6 * row + col);
}

bool CustomBroadphaseFilter::needBroadphaseCollision(btBroadphaseProxy* proxy0, btBroadphaseProxy* proxy1) const {
    CollisionObject* obj0 = (CollisionObject*)((btCollisionObject*)(proxy0->m_clientObject))->getUserPointer();
    CollisionObject* obj1 = (CollisionObject*)((btCollisionObject*)(proxy1->m_clientObject))->getUserPointer();

    int entry = index(obj0->getType(), obj1->getType());
    return collides[entry];
}

static void nearCallback(btBroadphasePair& pair, btCollisionDispatcher& dispatcher, const btDispatcherInfo& info) {
    
    CollisionObject* obj0 = (CollisionObject*)((btCollisionObject*)(pair.m_pProxy0->m_clientObject))->getUserPointer();
    CollisionObject* obj1 = (CollisionObject*)((btCollisionObject*)(pair.m_pProxy1->m_clientObject))->getUserPointer();
    CollisionObject* temp;

    bool travel0 = obj0->isTravelling();
    bool travel1 = obj1->isTravelling();

    if (travel1 && !travel0)
    {
        temp = obj0;
        obj0 = obj1;
        obj1 = temp;

        travel0 = true;
        travel1 = false;
    }

    bool result = true;

    if (travel0 && obj1->getType() == COLLISION_STATIC)
    {
        std::vector<CollisionObject*>* obstacles = obj0->getGate()->getObstacles();
        for (size_t i = 0; i < obstacles->size(); i++)
        {
            if (obstacles->at(i) == obj1)
            {
                result = false;
            }
        }
    }

    if(result) {
        dispatcher.defaultNearCallback(pair, dispatcher, info);
    }
}

static void tickCallback(btDynamicsWorld* world, btScalar time) {

    CollisionWorld::getInstance()->collisionStart();

    int manifolds = world->getDispatcher()->getNumManifolds();

    for (int i = 0; i < manifolds; i++) {
        btPersistentManifold* manifold = world->getDispatcher()->getManifoldByIndexInternal(i);

        CollisionObject* obj0 = (CollisionObject*)((btCollisionObject*)(manifold->getBody0()))->getUserPointer();
        CollisionObject* obj1 = (CollisionObject*)((btCollisionObject*)(manifold->getBody1()))->getUserPointer();

        int contacts = manifold->getNumContacts();
        bool collision = false;

        for (int j = 0; j < contacts; j++)
        {
            if (manifold->getContactPoint(j).getDistance() < PORTAL_CONTACT_DISTANCE)
            {
                collision = true;
            }
        }
        
        if (collision)
        {
            uint32_t type0 = obj0->getType();
            uint32_t type1 = obj1->getType();
            if ((type0 == COLLISION_KINEMATIC || type0 == COLLISION_DYNAMIC) && (type1 == COLLISION_SOURCE || type1 == COLLISION_TARGET || type1 == COLLISION_TRIGGER))
            {
                CollisionWorld::getInstance()->collisionDetected(obj0, obj1);
            }
            if ((type1 == COLLISION_KINEMATIC || type1 == COLLISION_DYNAMIC) && (type0 == COLLISION_SOURCE || type0 == COLLISION_TARGET || type0 == COLLISION_TRIGGER))
            {
                CollisionWorld::getInstance()->collisionDetected(obj1, obj0);
            }
        }
    }

    CollisionWorld::getInstance()->collisionEnd();
}

SharedCollisionWorld CollisionWorld::instance;

CollisionWorld::CollisionWorld() : filter(), cache(), broadphase(), config(), dispatcher(), solver(), world(), objects(), travelling() {}

CollisionWorld::~CollisionWorld() {
    for (size_t i = 0; i < objects.size(); i++)
    {
        world->removeRigidBody(objects[i]->getBody());
    }

    delete world;
    delete solver;
    delete dispatcher;
    delete config;
    delete broadphase;
    delete cache;
    delete filter;
}

void CollisionWorld::initInstance() {
    instance = std::make_shared<CollisionWorld>();
    instance->init();
}

void CollisionWorld::reset() {
    instance = nullptr;
}

void CollisionWorld::init() {
    filter = new CustomBroadphaseFilter;
    cache = new btHashedOverlappingPairCache();
    cache->setOverlapFilterCallback(filter);
    broadphase = new btDbvtBroadphase(cache);

    config = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(config);
    dispatcher->setNearCallback(&nearCallback);
    solver = new btSequentialImpulseConstraintSolver();

    world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, config);
    world->setInternalTickCallback(tickCallback);
    world->setGravity(btVector3(0.0f, -DEFAULT_GRAVITY, 0.0f));
}

btVector3 CollisionWorld::getGravity() const {
    return world->getGravity();
}

void CollisionWorld::setGravity(btVector3 gravity) {
    world->setGravity(gravity);
}

void CollisionWorld::tick(float elapsed) {
    world->stepSimulation(elapsed);
}

void CollisionWorld::collisionStart() {
    for (size_t i = 0; i < confirmed.size(); i++)
    {
        confirmed[i] = false;
    }
}

void CollisionWorld::collisionDetected(CollisionObject* actor, CollisionObject* other) {

    if (other->getType() == COLLISION_TRIGGER)
    {
        LogicWorld::getInstance()->trigger(actor, other);
        return;
    }

    if (actor->isTravelling())
    {
        int index = std::distance(travelling.begin(), std::find(travelling.begin(), travelling.end(), actor));
        if (other == actor->getGate()->getSource())
        {
            if (!actor->hasPassed())
            {
                actor->setProgress(TRAVELLING_SOURCE_CONTACT);
                confirmed[index] = true;
            }
        }
        else if (other == actor->getGate()->getTarget())
        {
            if (actor->hasPassed())
            {
                actor->setProgress(TRAVELLING_TARGET_CONTACT);
                confirmed[index] = true;
            }
        }
    }
    else
    {
        if (other->getType() == COLLISION_SOURCE)
        {
            actor->enter(other->getGate());
            travelling.push_back(actor);
            confirmed.push_back(true);
        }
    }
}

void CollisionWorld::collisionEnd() {
    for (int i = travelling.size() - 1; i >= 0; i--)
    {
        if (confirmed[i] == false)
        {
            CollisionObject* actor = travelling[i];

            if ((actor->getProgress() == TRAVELLING_SOURCE_CONTACT && actor->hasPassed()) || (actor->getProgress() == TRAVELLING_TARGET_CONTACT && !actor->hasPassed()))
            {
                actor->setProgress(TRAVELLING_INTERMEDIATE);
            }
            else if (actor->getProgress() == TRAVELLING_INTERMEDIATE)
            {
                // do nothing
            }
            else
            {
                actor->exit();
                confirmed.erase(confirmed.begin() + i);
                travelling.erase(travelling.begin() + i);
            }
        }
    }
}

std::vector<SharedCollisionObject>* CollisionWorld::getObjects() {
    return &objects;
}

size_t CollisionWorld::getObjectCount() const {
    return objects.size();
}
SharedCollisionObject CollisionWorld::getObject(size_t index) {
    return objects[index];
}

void CollisionWorld::insertObject(size_t index, SharedCollisionObject object) {
    objects.insert(objects.begin() + index, object);
    world->addRigidBody(object->getBody());
}

void CollisionWorld::addObject(SharedCollisionObject object) {
    objects.push_back(object);
    world->addRigidBody(object->getBody());
}

void CollisionWorld::removeObject(size_t index) {
    world->removeRigidBody(objects[index]->getBody());
    objects.erase(objects.begin() + index);
}

void CollisionWorld::removeObject(SharedCollisionObject object) {
    std::vector<SharedCollisionObject>::iterator iterator = std::find(objects.begin(), objects.end(), object);
    if (iterator != objects.end())
    {
        world->removeRigidBody(object->getBody());
        objects.erase(iterator);
    }
}

void CollisionWorld::clearPortals() {
    for (size_t i = 0; i < objects.size(); i++)
    {
        world->removeRigidBody(objects[i]->getBody());
    }

    objects.clear();
}

