#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>
#include <glow/fwd.hh>
#include <glow/objects/ArrayBuffer.hh>
#include <glow/objects/ElementArrayBuffer.hh>
#include <glow/objects/VertexArray.hh>

struct MeshVertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 texcoord;
};

class MeshHelper
{
public:
    static glm::vec3 getTangent(std::vector<glm::vec3>* normals, std::vector<glm::vec3>* tangents, glm::vec3 normal);
    static glow::SharedVertexArray createBox(glm::vec3 min, glm::vec3 max);
    static glow::SharedVertexArray createBox(float x, float y, float z);
};
