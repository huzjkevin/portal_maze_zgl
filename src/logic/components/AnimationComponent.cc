#include "AnimationComponent.hh"
#include "../../render/Renderer.hh"
#include "load_mesh.hh"
#include <glow/objects/VertexArray.hh>
#include <glow/objects/Texture2D.hh>
#include <glow/objects/ArrayBuffer.hh>
#include <glow/objects/ElementArrayBuffer.hh>
#include <glow/data/TextureData.hh>
#include "../glow/extern/lodepng/lodepng/lodepng.h"
#include <glow/common/log.hh>
using namespace example;
using namespace tinygltf;

AnimationComponent::AnimationComponent()
{
}


void AnimationComponent::loadGLTFModel(const char* fileName, float scale)
{
    mScale = scale;
    bool ret = LoadGLTF(fileName, 1.f, mModelData.model, &mModelData.meshes, &mModelData.materials, &mModelData.textures);
    if (ret)
    {
        bindModel(mModelData);
    }
    else
    {
        printf("Load GLTF Model Error: Failed to parse glTF file %s\n", fileName);
    }
}

void AnimationComponent::bindModel(ModelData &modelData)
{
    // If the glTF asset has at least one scene, and doesn't define a default one
    // just show the first one we can find
    auto& model = modelData.model;

    //assert(model.meshes.size() > 0);
    //glm::mat4 nodeMatrix;
    //bindMesh(modelData, 0, nodeMatrix);
    
    if (model.images.size() > 0)
    {
        auto textData = glow::TextureData::createFromRawData(model.images[0].image, model.images[0].width, model.images[0].height, glow::ColorSpace::sRGB);
        mNormalTexture = mAlbedoTexture = glow::Texture2D::createFromData(textData);
    }
    if (model.images.size() > 1)
    {
        auto textData = glow::TextureData::createFromRawData(model.images[1].image, model.images[1].width, model.images[1].height, glow::ColorSpace::Linear);
        mNormalTexture = glow::Texture2D::createFromData(textData);
    }

    assert(model.scenes.size() > 0);
    int scene_to_display = model.defaultScene > -1 ? model.defaultScene : 0;
    const tinygltf::Scene &scene = model.scenes[scene_to_display];
    glm::mat4 modelMatrix = glm::scale(glm::vec3(mScale, mScale, mScale));

    for (size_t i = 0; i < scene.nodes.size(); i++) {
        bindNode(modelData, scene.nodes[i], modelMatrix);
    }
};

void AnimationComponent::bindNode(ModelData &modelData, int nodeIdx, const glm::mat4& modelMatrix)
{
    auto& model = modelData.model;
    auto& node = model.nodes[nodeIdx];
    glm::mat4 nodeMatrix;
    if (node.matrix.size() == 16)
    {
        // Use `matrix' attribute
        auto data = node.matrix.data();
        nodeMatrix = glm::mat4(data[0], data[1], data[2], data[3],
                               data[4], data[5], data[6], data[7],
                               data[8], data[9], data[10], data[11],
                               data[12], data[13], data[14], data[15]);
    }
    else
    {
        // Assume Trans x Rotate x Scale order
        if (node.scale.size() == 3)
        {
            nodeMatrix = glm::scale(nodeMatrix, glm::vec3(node.scale[0], node.scale[1], node.scale[2]));
        }

        if (node.rotation.size() == 4)
        {
            auto angle = acosf((float)node.rotation[3]);
            auto sina = sin(angle);
            nodeMatrix = glm::rotate(nodeMatrix, angle * 2, glm::vec3(node.rotation[0] / sina, node.rotation[1] / sina, node.rotation[2] / sina));
        }

        if (node.translation.size() == 3)
        {
            nodeMatrix = glm::translate(nodeMatrix, glm::vec3(node.translation[0], node.translation[1], node.translation[2]));
        }
    }

    glm::mat4 newMatrix = nodeMatrix * modelMatrix;
    if (node.mesh > -1) {
        assert(node.mesh >= 0 && ((size_t)node.mesh) < model.meshes.size());
        bindMesh(modelData, node.mesh, node.skin, newMatrix);
    }

    // Draw child nodes.
    for (size_t i = 0; i < node.children.size(); i++) {
        assert(node.children[i] >= 0 && ((size_t)node.children[i]) < model.nodes.size());
        bindNode(modelData, node.children[i], newMatrix);
    }
}

void AnimationComponent::bindMesh(ModelData &modelData, int meshIdx, int skinIdx, const glm::mat4& modelMatrix)
{
    auto& model = modelData.model;
    auto& mesh = model.meshes[meshIdx];
    auto& meshData = modelData.meshes[meshIdx];

    std::vector<glm::vec3> aPos;
    std::vector<glm::vec3> aNormal;
    std::vector<glm::vec2> aTexCoord;

    for (size_t i = 0; i < meshData.vertices.size(); i += 3)
    {
        auto pos = glm::vec3(meshData.vertices[i], meshData.vertices[i + 1], meshData.vertices[i + 2]);// + glm::vec3(0, 46, 0);
        pos = modelMatrix * glm::vec4(pos, 1);
        aPos.push_back(pos);
    }
    for (size_t i = 0; i < meshData.normals.size(); i += 3)
    {
        aNormal.push_back(glm::vec3(meshData.normals[i], meshData.normals[i + 1], meshData.normals[i + 2]));
    }
    for (size_t i = 0; i < meshData.uvs.size(); i += 2)
    {
        aTexCoord.push_back(glm::vec2(meshData.uvs[i], meshData.uvs[i + 1]));
    }

    std::vector<glm::vec3> aTangent;
    for (size_t i = 0; i < meshData.tangents.size(); i += 4)
    {
        aTangent.push_back(glm::vec3(meshData.tangents[i], meshData.tangents[i + 1], meshData.tangents[i + 2]));
    }

    //auto one_over_sqrt3 = 0.57735f;
    //glm::vec3 tan(one_over_sqrt3, one_over_sqrt3, one_over_sqrt3);
    //tan = tan / tan.length();
    //for (int i = 0; i < meshData.normals.size() / 3; ++i)
    //{
    //    aTangent.push_back(tan);
    //}

    auto abPos = glow::ArrayBuffer::create("aPosition", aPos);
    auto abNormal = glow::ArrayBuffer::create("aNormal", aNormal);
    auto abTexCoord = glow::ArrayBuffer::create("aTexCoord", aTexCoord);
    auto abTangent = glow::ArrayBuffer::create("aTangent", aTangent);

    auto eab = glow::ElementArrayBuffer::create(meshData.faces);
    auto meshVertices = glow::VertexArray::create({abPos, abNormal, abTangent, abTexCoord}, eab, GL_TRIANGLES);
   
    mMesh = meshVertices;
    //modelData.modelMatrices.push_back(modelMatrix);
}
