#pragma once

#include "../forward.hh"

class RecursivePass
{
private:
    Renderer* renderer;

    SharedQueryTree queries;
    RecursivePass* parent;

    uint32_t depth;
    int32_t progress;

    Portal* portal;
    glm::vec4 clipPlane;

    glm::mat4 view;
    glm::mat4 proj;

    bool invertView;

public:
    RecursivePass(Renderer* r, SharedQueryTree q, RecursivePass* p = nullptr, uint32_t d = 0)
      : renderer(r), queries(q), parent(p), depth(d), progress(-1), portal(nullptr), clipPlane(), view(), proj(), invertView(false)
    {
    }

    virtual ~RecursivePass() {}

    Renderer* getRenderer() { return renderer; }
    RecursivePass* getParent() { return parent; }

    bool isRoot() { return (parent == nullptr); }
    bool isChild() { return !(parent == nullptr); }

    uint32_t getDepth() { return depth; }
    int32_t getProgress() { return progress; }

    Portal* getPortal() { return portal; }
    glm::vec4 getClipPlane() { return clipPlane; }

    glm::mat4 getView() { return view; }
    glm::mat4 getProj() { return proj; }

    bool invertsView() { return invertView; }

    void initRoot(glow::camera::Camera* camera);
    void initChild(Portal* portal);

    RecursivePass* step();
    void done();

    RecursivePass* launchChild(Portal* pair, SharedQueryTree query);

    void prepareFlags(GLboolean color, bool depth, GLenum depthFunc, GLint stencilRef, GLenum stencilOp, bool culling, bool clip, bool wireframe = false);

    void renderWorld();
    void renderLight();

    void renderGlobalLight(SharedGlobalLight globalLight, glow::UsedProgram* shader);
    void renderPointLight(SharedPointLight pointLight, glm::vec3 cameraPosition);
    void renderSpotLight(SharedSpotLight spotLight);
    void fade();

    void renderPortal(SharedPortal portal, SharedQueryTree query);
    void clearPortalDepth(glm::vec3 color);
    void resetPortalDepth(Portal* child);

    void renderBackground();
};
