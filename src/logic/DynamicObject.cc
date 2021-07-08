#include "DynamicObject.hh"

#include <glow/objects/Texture2D.hh>
#include <glow/objects/VertexArray.hh>

#include "../collision/CollisionObject.hh"
#include "../render/Graph.hh"

DynamicObject::DynamicObject(SharedCollisionObject c) {
    collision = c;

    mesh = std::make_shared<MeshGroup>();
    texture = std::make_shared<TextureGroup>();
    transform = std::make_shared<TransformGroup>();
    mirror = std::make_shared<TransformGroup>();

    mesh->addChild(texture);
    texture->addChild(transform);
    transform->addChild(std::make_shared<Leaf>());
    mirror->addChild(std::make_shared<Leaf>());
}

DynamicObject::~DynamicObject() {
    
}

SharedCollisionObject DynamicObject::getCollision() {
    return collision;
}

SharedMeshGroup DynamicObject::getNode(){
    return mesh;
}

glow::SharedVertexArray DynamicObject::getMesh() {
    return mesh->getMesh();
}

void DynamicObject::setMesh(glow::SharedVertexArray m) {
    mesh->setMesh(m);
}

glow::SharedTexture2D DynamicObject::getAlbedo() {
    return texture->getAlbedo();
}

void DynamicObject::setAlbedo(glow::SharedTexture2D a) {
    texture->setAlbedo(a);
}

glow::SharedTexture2D DynamicObject::getNormal() {
    return texture->getNormal();
}

void DynamicObject::setNormal(glow::SharedTexture2D a) {
    texture->setNormal(a);
}

SharedTransformGroup DynamicObject::getTransformGroup() {
    return transform;
}

void DynamicObject::toggleMirror() {
    if (collision->isTravelling())
    {
        mirror->setTransform(collision->getMirror());
        if (texture->getChildCount() < 2)
        {
            texture->addChild(mirror);
        }
    }
    else
    {
        if (texture->getChildCount() > 1)
        {
            texture->removeChild(1);
        }
    }
}

void DynamicObject::tick(float elapsed) {
    if (collision->isTravelling())
    {
        collision->teleporting();
    }
}

void DynamicObject::update(float elapsed) {
    transform->setTransform(collision->getModel());
    toggleMirror();
}
