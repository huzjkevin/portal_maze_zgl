#pragma once

#include <glm/fwd.hpp>
#include <glow/fwd.hh>
#include <glow/math/transform.hh>

#include "../render/Drawable.hh"
#include "../render/Renderer.hh"
#include "tiny_gltf.h"
#include "gltf-loader.hh"

class AnimationComponent
{
protected:
    std::vector<SharedDrawable> mDrawables;
    glow::SharedVertexArray mMesh;
    glow::SharedTexture2D mAlbedoTexture;
    glow::SharedTexture2D mNormalTexture;
    float mScale = 1.f;

public:
    AnimationComponent();
    virtual ~AnimationComponent() {}

    void loadGLTFModel(const char* fileName, float scale);
    glow::SharedVertexArray& getMesh() { return mMesh; }
    glow::SharedTexture2D& getAlbedoTex() { return mAlbedoTexture; }
    glow::SharedTexture2D& getNormalTex() { return mNormalTexture; }

private:

    struct ModelData
    {
        std::vector<example::Mesh<float> > meshes;
        std::vector<example::Material> materials;
        std::vector<example::Texture> textures;
        tinygltf::Model model;
        std::vector<glm::mat4> modelMatrices;
    };

    ModelData mModelData;

    void bindModel(ModelData &modelData);
    void bindNode(ModelData &modelData, int nodeIdx, const glm::mat4& modelMatrix);
    void bindMesh(ModelData &modelData, int meshIdx, int skinIdx, const glm::mat4& modelMatrix);
};

