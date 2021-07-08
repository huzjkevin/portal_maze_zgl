#pragma once

#include "../forward.hh"

#include <glow/objects/Program.hh>
#include <glow/objects/Texture2D.hh>
#include <glow/objects/VertexArray.hh>

class Node
{
protected:
    Node* parent;

public:
    Node() : parent() {}
    virtual ~Node() {}

    virtual Node* getParent() { return parent; }

    virtual void setParent(Node* p) { parent = p; }

    virtual bool isAttached() { return (parent != nullptr && parent->isAttached()); }

    virtual glm::mat4 getModel() { return parent->getModel(); }

    virtual glow::SharedTexture2D getAlbedo() { return parent->getAlbedo(); }

    virtual glow::SharedTexture2D getNormal() { return parent->getNormal(); }

    virtual glow::SharedVertexArray getMesh() { return parent->getMesh(); }

    virtual void draw(glow::UsedProgram* shader) = 0;
};

class Leaf : public Node
{
public:
    Leaf() : Node() {}
    virtual ~Leaf() {}

    virtual void draw(glow::UsedProgram* shader) { getMesh()->bind().draw(); }
};

class Group : public Node
{
protected:
    std::vector<SharedNode> children;

public:
    Group() : Node(), children() {}
    virtual ~Group() {}

    virtual std::vector<SharedNode>* getChildren() { return &children; }

    virtual size_t getChildCount() const { return children.size(); }
    virtual SharedNode getChild(size_t index) { return children[index]; }

    virtual void insertChild(size_t index, SharedNode child)
    {
        children.insert(children.begin() + index, child);
        child->setParent(this);
    }

    virtual void addChild(SharedNode child)
    {
        children.push_back(child);
        child->setParent(this);
    }

    virtual void removeChild(size_t index)
    {
        children[index]->setParent(nullptr);
        children.erase(children.begin() + index);
    }

    virtual void removeChild(SharedNode child)
    {
        std::vector<SharedNode>::iterator iterator = std::find(children.begin(), children.end(), child);
        if (iterator != children.end())
        {
            child->setParent(nullptr);
            children.erase(iterator);
        }
    }

    virtual void clearChildren()
    {
        for (size_t i = 0; i < children.size(); i++)
        {
            children[i]->setParent(nullptr);
        }
        children.clear();
    }

    virtual void draw(glow::UsedProgram* shader)
    {
        for (size_t i = 0; i < children.size(); i++)
        {
            children[i]->draw(shader);
        }
    }
};

class Root : public Group
{
public:
    Root() : Group() {}
    virtual ~Root() {}

    virtual bool isAttached() { return true; }
    virtual glm::mat4 getModel() { return glm::mat4(); }

    virtual void draw(glow::UsedProgram* shader)
    {
        shader->setUniform("uModel", glm::mat4());
        Group::draw(shader);
    }
};

class TransformGroup : public Group
{
protected:
    glm::mat4 transform;

public:
    TransformGroup() : Group(), transform() {}
    TransformGroup(glm::mat4 t) : Group(), transform(t) {}
    virtual ~TransformGroup() {}

    virtual glm::mat4 getTransform() const { return transform; }
    virtual void setTransform(glm::mat4 t) { transform = t; }

    virtual glm::mat4 getModel() { return parent->getModel() * transform; }

    virtual void draw(glow::UsedProgram* shader)
    {
        shader->setUniform("uModel", getModel());
        Group::draw(shader);
    }
};

class TextureGroup : public Group
{
protected:
    glow::SharedTexture2D albedo;
    glow::SharedTexture2D normal;

public:
    TextureGroup() : Group(), albedo(), normal() {}
    TextureGroup(glow::SharedTexture2D a) : Group(), albedo(a), normal() {}
    TextureGroup(glow::SharedTexture2D a, glow::SharedTexture2D n) : Group(), albedo(a), normal(n) {}
    virtual ~TextureGroup() {}

    virtual glow::SharedTexture2D getAlbedo() { return albedo; }
    virtual glow::SharedTexture2D getNormal() { return normal; }

    virtual void setAlbedo(glow::SharedTexture2D a) { albedo = a; }
    virtual void setNormal(glow::SharedTexture2D n) { normal = n; }

    virtual void draw(glow::UsedProgram* shader)
    {
        shader->setTexture("uTexAlbedo", albedo);
        shader->setTexture("uTexNormal", normal);
        Group::draw(shader);
    }
};

class MeshGroup : public Group
{
protected:
    glow::SharedVertexArray mesh;

public:
    MeshGroup() : Group(), mesh() {}
    MeshGroup(glow::SharedVertexArray m) : Group(), mesh(m) {}
    virtual ~MeshGroup() {}

    virtual glow::SharedVertexArray getMesh() { return mesh; }
    virtual void setMesh(glow::SharedVertexArray m) { mesh = m; }
};
