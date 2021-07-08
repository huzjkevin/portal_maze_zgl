#pragma once

#include "../forward.hh"

class DynamicObject
{
private:
    SharedCollisionObject collision;

    SharedMeshGroup mesh;
    SharedTextureGroup texture;
    SharedTransformGroup transform;
    SharedTransformGroup mirror;

public:
    DynamicObject(SharedCollisionObject c);
    virtual ~DynamicObject();

    SharedCollisionObject getCollision();
    SharedMeshGroup getNode();

    glow::SharedVertexArray getMesh();
    void setMesh(glow::SharedVertexArray m);

    glow::SharedTexture2D getAlbedo();
    void setAlbedo(glow::SharedTexture2D a);

    glow::SharedTexture2D getNormal();
    void setNormal(glow::SharedTexture2D a);

    SharedTransformGroup getTransformGroup();

    void toggleMirror();

    virtual void tick(float elapsed);
    virtual void update(float elapsed);
};
