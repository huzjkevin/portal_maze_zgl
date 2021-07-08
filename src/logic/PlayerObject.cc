#include "PlayerObject.hh"

#include "DynamicObject.hh"

#include "../collision/CollisionObject.hh"
#include "../render/Graph.hh"

#include <glow/math/transform.hh>

PlayerObject::PlayerObject(SharedCollisionObject c) : DynamicObject(c) {

}

PlayerObject::~PlayerObject() {

}

void PlayerObject::move(glm::vec3 direction) {
    getCollision()->setModel(glm::translate(glm::mat4(), direction) * getCollision()->getModel());
}

void PlayerObject::update(float elapsed) {
    getCollision()->adjustToCamera();
    DynamicObject::update(elapsed);
}
