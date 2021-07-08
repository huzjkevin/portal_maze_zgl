#pragma once

#include "../forward.hh"
#include <glow-extras/camera/Camera.hh>

class Portal
{
private:
    glow::SharedVertexArray mesh;

    glm::mat4 source;
    glm::mat4 target;
    glm::mat4 inverse;

    bool mirror;

    float width;
    float height;

public:
    Portal(bool m = false, float w = DEFAULT_PORTAL_WIDTH, float h = DEFAULT_PORTAL_HEIGHT);
    virtual ~Portal();

    glow::SharedVertexArray getMesh() { return mesh; }
    void setMesh(glow::SharedVertexArray m) { mesh = m; }

    glm::mat4 getSource() { return source; }
    void setSource(glm::mat4 s) { source = s; }

    glm::mat4 getTarget() { return target; }
    void setTarget(glm::mat4 t)
    {
        target = t;
        inverse = glm::inverse(t);
    }

    glm::mat4 getModel() { return source; }
    glm::mat4 getInverse() { return inverse; }

    bool isMirror() { return mirror; }
    void setMirror(bool m) { mirror = m; }

    float getWidth() { return width; }
    void setWidth(float w) { width = w; }

    float getHeight() { return height; }
    void setHeight(float h) { height = h; }

    void createQuadMesh();

    glm::mat4 transformView(glm::mat4 view) const;
    glm::vec4 calculateClipPlane(float guard) const;
};
