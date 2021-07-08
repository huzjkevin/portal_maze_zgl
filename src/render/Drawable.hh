#pragma once

#include "../forward.hh"

#include "Graph.hh"

class Drawable : public TextureGroup
{
private:
    SharedTransformGroup transform;
    SharedMeshGroup mesh;
    SharedLeaf leaf;

public:
    Drawable();
    virtual ~Drawable();

    virtual glm::mat4 getTransform() const;
    virtual void setTransform(glm::mat4 t);

    virtual glow::SharedVertexArray getMesh();
    virtual void setMesh(glow::SharedVertexArray m);

    virtual void draw(glow::UsedProgram* shader);
};
