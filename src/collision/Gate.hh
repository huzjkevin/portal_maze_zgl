#pragma once

#include "../forward.hh"
#include <glow-extras/camera/Camera.hh>

class Gate
{
private:
    CollisionObject* source;
    CollisionObject* target;

    glm::mat4 sourceInverse;
    glm::mat4 targetInverse;

    std::vector<CollisionObject*> obstacles;
    SharedQueryHandler queryHandler;

public:
    Gate(CollisionObject* s, CollisionObject* t);
    virtual ~Gate() {}

    CollisionObject* getSource() { return source; }
    CollisionObject* getTarget() { return target; }

    glm::mat4 getSourceModel() const;
    glm::mat4 getTargetModel() const;

    glm::mat4 getSourceInverse() const;
    glm::mat4 getTargetInverse() const;

    std::vector<CollisionObject*>* getObstacles() { return &obstacles; }
    SharedQueryHandler getQueryHandler() { return queryHandler; }

    bool hasPassed(glm::vec3 position) const;
    bool hasPassed(glm::mat4 transform) const;

    bool hasReturned(glm::vec3 position) const;
    bool hasReturned(glm::mat4 transform) const;

    glm::vec3 teleport(glm::vec3 position) const;
    glm::mat4 teleport(glm::mat4 model) const;

    glm::vec3 pullback(glm::vec3 position) const;
    glm::mat4 pullback(glm::mat4 model) const;

    glm::vec3 adjust(glm::vec3 position, bool passed) const;
    glm::mat4 adjust(glm::mat4 position, bool passed) const;

    void teleportCamera(glow::camera::SharedCamera camera, glm::vec3 object) const;
    void pullbackCamera(glow::camera::SharedCamera camera, glm::vec3 object) const;
};
