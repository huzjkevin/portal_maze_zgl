#include "Drawable.hh"

Drawable::Drawable() : TextureGroup(), transform(), mesh(), leaf() {
    transform = std::make_shared<TransformGroup>();
    mesh = std::make_shared<MeshGroup>();
    leaf = std::make_shared<Leaf>();

    mesh->addChild(leaf);
    transform->addChild(mesh);
    TextureGroup::addChild(transform);
}

Drawable::~Drawable() {

}

glm::mat4 Drawable::getTransform() const {
    return transform->getTransform();
}

void Drawable::setTransform(glm::mat4 t) {
    transform->setTransform(t);
}

glow::SharedVertexArray Drawable::getMesh() {
    return mesh->getMesh();
}

void Drawable::setMesh(glow::SharedVertexArray m) {
    mesh->setMesh(m);
}

void Drawable::draw(glow::UsedProgram* shader) {
    if (getMesh() != nullptr)
    {
        TextureGroup::draw(shader);
    }
}
