#include "CameraObject.hh"

#include "../collision/CollisionObject.hh"

CameraObject::CameraObject() : camera(), owner(), offset(0.0f, 0.0f, 0.0f), fps(CAMERA_DEFAULT_FPS), distance(CAMERA_DEFAULT_DISTANCE) {
    camera = glow::camera::Camera::create();

    camera->setFoV(CAMERA_DEFAULT_FOV);

    camera->handle.setTargetDistance(CAMERA_DEFAULT_DISTANCE);
    camera->handle.setPitchInDegree(CAMERA_DEFAULT_PITCH);
}

void CameraObject::setOwner(SharedCollisionObject o)
{
    owner = o;
}

void CameraObject::setDistance(float d) {
    distance = d;
    camera->handle.setTargetDistance(d);
}

void CameraObject::setFoV(float fov)
{
    camera->setFoV(fov);
}

void CameraObject::setPitch(float p)
{
    camera->handle.setPitchInDegree(p);
}

void CameraObject::resize(int w, int h) {
    camera->setViewportSize(w, h);
}

void CameraObject::rotate(float dx, float dy) {
    if (fps)
    {
        camera->handle.lookAround(dx, dy);
    }
    else
    {
        camera->handle.orbit(dx, dy);
    }
}

glm::vec3 CameraObject::getOwnerPosition() const {
    return (owner->getPosition() + offset);
}

void CameraObject::update(float elapsed) {
    camera->handle.translateWithTarget(getOwnerPosition());
    camera->update(elapsed);
    //camera->handle.snap();

    /* Camera can teleport on its own.

    if (owner->isTravelling())
    {
        Gate* gate = owner->getGate();
        glm::vec3 position = camera->handle.getPosition();
        glm::vec3 target = camera->handle.getTarget();

        if (owner->hasPassed())
        {
            if (gate->hasReturned(position))
            {
                gate->pullbackCamera(camera, glm::vec3(owner->getMirror() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
            }
        }
        else
        {
            if (gate->hasPassed(position))
            {
                gate->teleportCamera(camera, glm::vec3(owner->getMirror() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)));
            }
        }
    }

    /**/
}

void CameraObject::snap() {
    camera->handle.snap();
}
