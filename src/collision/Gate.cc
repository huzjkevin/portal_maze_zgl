#include "Gate.hh"

#include "CollisionObject.hh"

#include "../render/QueryHandler.hh"

Gate::Gate(CollisionObject* s, CollisionObject* t) : source(s), target(t), sourceInverse(), targetInverse(), obstacles(), queryHandler() {
    sourceInverse = glm::inverse(source->getModel());
    targetInverse = glm::inverse(target->getModel());
    queryHandler = std::make_shared<QueryHandler>();
}

glm::mat4 Gate::getSourceModel() const {
    return source->getModel();
}

glm::mat4 Gate::getTargetModel() const {
    return target->getModel();
}

glm::mat4 Gate::getSourceInverse() const {
    return sourceInverse;
}

glm::mat4 Gate::getTargetInverse() const {
    return targetInverse;
}

bool Gate::hasPassed(glm::vec3 position) const {
    return (sourceInverse * glm::vec4(position, 1.0f)).z > 0.0f;
}

bool Gate::hasPassed(glm::mat4 transform) const {
    return (sourceInverse * transform)[3][2] > 0.0f;
}

bool Gate::hasReturned(glm::vec3 position) const {
    return (targetInverse * glm::vec4(position, 1.0f)).z <= 0.0f;
}

bool Gate::hasReturned(glm::mat4 transform) const {
    return (targetInverse * transform)[3][2] <= 0.0f;
}

glm::vec3 Gate::teleport(glm::vec3 position) const {
    return glm::vec3(target->getModel() * sourceInverse * glm::vec4(position, 1.0f));
}

glm::mat4 Gate::teleport(glm::mat4 model) const {
    return target->getModel() * sourceInverse * model;
}

glm::vec3 Gate::pullback(glm::vec3 position) const {
    return glm::vec3(source->getModel() * targetInverse * glm::vec4(position, 1.0f));
}

glm::mat4 Gate::pullback(glm::mat4 model) const {
    return source->getModel() * targetInverse * model;
}

glm::vec3 Gate::adjust(glm::vec3 position, bool passed) const {
    if (passed)
    {
        if (hasReturned(position))
        {
            return pullback(position);
        }
    }
    else
    {
        if (hasPassed(position))
        {
            return teleport(position);
        }
    }
    return position;
}

glm::mat4 Gate::adjust(glm::mat4 model, bool passed) const {
    if (passed)
    {
        if (hasReturned(model))
        {
            return pullback(model);
        }
    }
    else
    {
        if (hasPassed(model))
        {
            return teleport(model);
        }
    }
    return model;
}

void Gate::teleportCamera(glow::camera::SharedCamera camera, glm::vec3 object) const {
    glm::vec3 forward = camera->handle.getForward();
    forward = glm::vec3(target->getModel() * sourceInverse * glm::vec4(forward, 0.0f));
    camera->handle.setLookAt(object - forward, object, true);
    camera->handle.snap();
}

void Gate::pullbackCamera(glow::camera::SharedCamera camera, glm::vec3 object) const {
    glm::vec3 forward = camera->handle.getForward();
    forward = glm::vec3(source->getModel() * targetInverse * glm::vec4(forward, 0.0f));
    camera->handle.setLookAt(object - forward, object, true);
    camera->handle.snap();
}
