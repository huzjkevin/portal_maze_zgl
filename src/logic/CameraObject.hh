#pragma once

#include "../forward.hh"
#include <glow-extras/camera/Camera.hh>

class CameraObject
{
private:
    glow::camera::SharedCamera camera;
    SharedCollisionObject owner;

    glm::vec3 offset;

    bool fps;
    float distance;

public:
    CameraObject();
    virtual ~CameraObject() {}

    glow::camera::SharedCamera getCamera() { return camera; }

    SharedCollisionObject getOwner() { return owner; }
    void setOwner(SharedCollisionObject o);

    glm::vec3 getOffset() { return offset; }
    void setOffset(glm::vec3 o) { offset = o; }

    float getDistance() const { return distance; }
    void setDistance(float d);

    void setFoV(float fov);
    void setPitch(float p);

    void resize(int w, int h);
    void rotate(float dx, float dy);

    glm::vec3 getForward() const { return camera->handle.getForward(); }
    glm::mat4 getViewMatrix() const { return camera->getViewMatrix(); }
    glm::mat4 getProjMatrix() const { return camera->getProjectionMatrix(); }

    glm::vec3 getOwnerPosition() const;

    void update(float elapsed);
    void snap();
};
