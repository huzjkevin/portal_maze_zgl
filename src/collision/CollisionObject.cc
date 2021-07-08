#include "CollisionObject.hh"

#include "Gate.hh"

#include "../logic/CameraObject.hh"
#include "../render/QueryHandler.hh"

#include "../bullet_helper.hh"

CollisionObject::CollisionObject()
  : shape(), state(), body(), type(COLLISION_STATIC), travelling(false), passed(false), progress(NOT_TRAVELLING), gate(nullptr), mirror(), camera(nullptr)
{

}

CollisionObject::~CollisionObject() {
    delete body;
    delete state;
    delete shape;
}

void CollisionObject::initStatic(btCollisionShape* s, glm::mat4 model)
{
    shape = s;
    state = new btDefaultMotionState(to_bullet(model));
    body = new btRigidBody(0.0f, state, shape);

    body->setUserPointer(this);
    type = COLLISION_STATIC;
}

void CollisionObject::initTrigger(btCollisionShape* s, glm::mat4 model, uint32_t t)
{
    shape = s;
    state = new btDefaultMotionState(to_bullet(model));
    body = new btRigidBody(0.0f, state, shape);

    body->setUserPointer(this);
    body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    type = t;
}

void CollisionObject::initDynamic(btCollisionShape* s, glm::mat4 model, btScalar mass, btScalar friction, btScalar restitution, btScalar linearDamping, btScalar angularDamping)
{
    shape = s;
    state = new btDefaultMotionState(to_bullet(model));

    btVector3 inertia;
    shape->calculateLocalInertia(mass, inertia);

    btRigidBody::btRigidBodyConstructionInfo info(mass, state, shape, inertia);
    info.m_friction = friction; // friction: will spin
    info.m_restitution = restitution; // bounciness: energy preserved on bounce
    info.m_linearDamping = linearDamping;
    info.m_angularDamping = angularDamping;
    body = new btRigidBody(info);

    body->setUserPointer(this);
    type = COLLISION_DYNAMIC;
}

void CollisionObject::setAngularFactor(btVector3 factor) {
    body->setAngularFactor(factor);
}

void CollisionObject::setAngularFactor(float factor) {
    body->setAngularFactor(factor);
}

glm::mat4 CollisionObject::getModel() const
{
    btTransform transform;
    state->getWorldTransform(transform);
    return to_glm(transform);
}

void CollisionObject::setModel(glm::mat4 model)
{
    btTransform transform = to_bullet(model);
    state->setWorldTransform(transform);
    body->setWorldTransform(transform);
    body->activate();
}

glm::vec3 CollisionObject::getPosition() const
{
    glm::mat4 model = getModel();
    return glm::vec3(model[3][0], model[3][1], model[3][2]);
}

void CollisionObject::enter(Gate* g)
{
    travelling = true;

    passed = false;
    progress = TRAVELLING_SOURCE_CONTACT;

    gate = g;
    mirror = g->teleport(getModel());
}

void CollisionObject::teleporting() {

    glm::mat4 model = getModel();

    if (passed)
    {
        if (gate->hasReturned(model))
        {
            setModel(gate->pullback(model));

            if (camera)
            {
                gate->pullbackCamera(camera->getCamera(), camera->getOwnerPosition());
                gate->getQueryHandler()->onPullback();
            }

            passed = false;
            mirror = model;
        }
        else
        {
            mirror = gate->pullback(model);
        }
    }
    else
    {
        if (gate->hasPassed(model))
        {
            setModel(gate->teleport(model));

            if (camera)
            {
                gate->teleportCamera(camera->getCamera(), camera->getOwnerPosition());
                gate->getQueryHandler()->onTeleport();
            }

            passed = true;
            mirror = model;
        }
        else
        {
            mirror = gate->teleport(model);
        }
    }
}

void CollisionObject::exit()
{
    travelling = false;
    passed = false;
    progress = NOT_TRAVELLING;
    gate = nullptr;
}

void CollisionObject::adjustToCamera() {
    if (camera != nullptr)
    {
        glm::mat3 rotation = glm::mat3(getModel());
        glm::mat3 rotationInverse = glm::inverse(rotation);

        glm::vec3 newX;
        glm::vec3 newY(0.0f, 1.0f, 0.0f);
        glm::vec3 newZ = camera->getForward();
        newZ.y = 0;
        newZ = glm::normalize(newZ);
        newX = glm::cross(newY, newZ);

        glm::mat3 newRotation = glm::mat3(
            newX.x, newX.y, newX.z,
            newY.x, newY.y, newY.z,
            newZ.x, newZ.y, newZ.z
        );

        glm::mat4 correction = glm::mat4(rotationInverse * newRotation);
        setModel(getModel() * correction);

        mirror = mirror * correction;
    }
}
