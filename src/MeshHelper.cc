#include "MeshHelper.hh"

glm::vec3 MeshHelper::getTangent(std::vector<glm::vec3>* normals, std::vector<glm::vec3>* tangents, glm::vec3 normal)
{
    for (uint32_t i = 0; i < normals->size(); i++)
    {
        if (normal == (*normals)[i])
        {
            return (*tangents)[i];
        }
    }

    return glm::vec3(0.0f, 0.0f, 0.0f);
}

glow::SharedVertexArray MeshHelper::createBox(glm::vec3 min, glm::vec3 max)
{
    auto ab = glow::ArrayBuffer::create();
    auto eab = glow::ElementArrayBuffer::create();

    ab->defineAttributes(
        {{&MeshVertex::pos, "aPosition"}, {&MeshVertex::normal, "aNormal"}, {&MeshVertex::tangent, "aTangent"}, {&MeshVertex::texcoord, "aTexCoord"}});

    MeshVertex vertex;

    std::vector<glm::vec3> positions(8);
    positions[0] = {max.x, min.y, min.z};
    positions[1] = {max.x, min.y, max.z};
    positions[2] = {min.x, min.y, max.z};
    positions[3] = {min.x, min.y, min.z};
    positions[4] = {max.x, max.y, min.z};
    positions[5] = {max.x, max.y, max.z};
    positions[6] = {min.x, max.y, max.z};
    positions[7] = {min.x, max.y, min.z};

    std::vector<glm::vec2> texcoords(20);
    texcoords[0] = {1.0f, 0.0f};
    texcoords[1] = {0.0f, 1.0f};
    texcoords[2] = {0.0f, 0.0f};
    texcoords[3] = {1.0f, 0.0f};
    texcoords[4] = {0.0f, 1.0f};
    texcoords[5] = {0.0f, 0.0f};
    texcoords[6] = {1.0f, 0.0f};
    texcoords[7] = {0.0f, 1.0f};
    texcoords[8] = {1.0f, 0.0f};
    texcoords[9] = {0.0f, 1.0f};
    texcoords[10] = {0.0f, 0.0f};
    texcoords[11] = {0.0f, 0.0f};
    texcoords[12] = {1.0f, 1.0f};
    texcoords[13] = {1.0f, 0.0f};
    texcoords[14] = {0.0f, 1.0f};
    texcoords[15] = {1.0f, 1.0f};
    texcoords[16] = {1.0f, 1.0f};
    texcoords[17] = {1.0f, 1.0f};
    texcoords[18] = {1.0f, 0.0f};
    texcoords[19] = {1.0f, 1.0f};

    std::vector<glm::vec3> normals(6);
    normals[0] = {0.0f, -1.0f, 0.0f};
    normals[1] = {0.0f, 1.0f, 0.0f};
    normals[2] = {1.0f, 0.0f, 0.0f};
    normals[3] = {0.0f, 0.0f, 1.0f};
    normals[4] = {-1.0f, 0.0f, 0.0f};
    normals[5] = {0.0f, 0.0f, -1.0f};

    std::vector<glm::vec3> tangents(6);
    tangents[0] = {1.0f, 0.0f, 0.0f};
    tangents[1] = {1.0f, 0.0f, 0.0f};
    tangents[2] = {0.0f, 0.0f, -1.0f};
    tangents[3] = {1.0f, 0.0f, 0.0f};
    tangents[4] = {0.0f, 0.0f, 1.0f};
    tangents[5] = {-1.0f, 0.0f, 0.0f};

    std::vector<MeshVertex> points(36);

    vertex.pos = positions[1];
    vertex.texcoord = texcoords[0];
    vertex.normal = normals[0];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[0] = vertex;
    vertex.pos = positions[3];
    vertex.texcoord = texcoords[1];
    vertex.normal = normals[0];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[1] = vertex;
    vertex.pos = positions[0];
    vertex.texcoord = texcoords[2];
    vertex.normal = normals[0];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[2] = vertex;

    vertex.pos = positions[7];
    vertex.texcoord = texcoords[3];
    vertex.normal = normals[1];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[3] = vertex;
    vertex.pos = positions[5];
    vertex.texcoord = texcoords[4];
    vertex.normal = normals[1];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[4] = vertex;
    vertex.pos = positions[4];
    vertex.texcoord = texcoords[5];
    vertex.normal = normals[1];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[5] = vertex;

    vertex.pos = positions[4];
    vertex.texcoord = texcoords[6];
    vertex.normal = normals[2];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[6] = vertex;
    vertex.pos = positions[1];
    vertex.texcoord = texcoords[7];
    vertex.normal = normals[2];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[7] = vertex;
    vertex.pos = positions[0];
    vertex.texcoord = texcoords[2];
    vertex.normal = normals[2];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[8] = vertex;

    vertex.pos = positions[5];
    vertex.texcoord = texcoords[8];
    vertex.normal = normals[3];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[9] = vertex;
    vertex.pos = positions[2];
    vertex.texcoord = texcoords[9];
    vertex.normal = normals[3];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[10] = vertex;
    vertex.pos = positions[1];
    vertex.texcoord = texcoords[10];
    vertex.normal = normals[3];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[11] = vertex;

    vertex.pos = positions[2];
    vertex.texcoord = texcoords[11];
    vertex.normal = normals[4];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[12] = vertex;
    vertex.pos = positions[7];
    vertex.texcoord = texcoords[12];
    vertex.normal = normals[4];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[13] = vertex;
    vertex.pos = positions[3];
    vertex.texcoord = texcoords[1];
    vertex.normal = normals[4];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[14] = vertex;

    vertex.pos = positions[0];
    vertex.texcoord = texcoords[13];
    vertex.normal = normals[5];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[15] = vertex;
    vertex.pos = positions[7];
    vertex.texcoord = texcoords[14];
    vertex.normal = normals[5];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[16] = vertex;
    vertex.pos = positions[4];
    vertex.texcoord = texcoords[5];
    vertex.normal = normals[5];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[17] = vertex;

    vertex.pos = positions[1];
    vertex.texcoord = texcoords[0];
    vertex.normal = normals[0];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[18] = vertex;
    vertex.pos = positions[2];
    vertex.texcoord = texcoords[15];
    vertex.normal = normals[0];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[19] = vertex;
    vertex.pos = positions[3];
    vertex.texcoord = texcoords[1];
    vertex.normal = normals[0];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[20] = vertex;

    vertex.pos = positions[7];
    vertex.texcoord = texcoords[3];
    vertex.normal = normals[1];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[21] = vertex;
    vertex.pos = positions[6];
    vertex.texcoord = texcoords[16];
    vertex.normal = normals[1];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[22] = vertex;
    vertex.pos = positions[5];
    vertex.texcoord = texcoords[4];
    vertex.normal = normals[1];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[23] = vertex;

    vertex.pos = positions[4];
    vertex.texcoord = texcoords[6];
    vertex.normal = normals[2];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[24] = vertex;
    vertex.pos = positions[5];
    vertex.texcoord = texcoords[17];
    vertex.normal = normals[2];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[25] = vertex;
    vertex.pos = positions[1];
    vertex.texcoord = texcoords[7];
    vertex.normal = normals[2];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[26] = vertex;

    vertex.pos = positions[5];
    vertex.texcoord = texcoords[8];
    vertex.normal = normals[3];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[27] = vertex;
    vertex.pos = positions[6];
    vertex.texcoord = texcoords[16];
    vertex.normal = normals[3];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[28] = vertex;
    vertex.pos = positions[2];
    vertex.texcoord = texcoords[9];
    vertex.normal = normals[3];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[29] = vertex;

    vertex.pos = positions[2];
    vertex.texcoord = texcoords[11];
    vertex.normal = normals[4];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[30] = vertex;
    vertex.pos = positions[6];
    vertex.texcoord = texcoords[18];
    vertex.normal = normals[4];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[31] = vertex;
    vertex.pos = positions[7];
    vertex.texcoord = texcoords[12];
    vertex.normal = normals[4];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[32] = vertex;

    vertex.pos = positions[0];
    vertex.texcoord = texcoords[13];
    vertex.normal = normals[5];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[33] = vertex;
    vertex.pos = positions[3];
    vertex.texcoord = texcoords[19];
    vertex.normal = normals[5];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[34] = vertex;
    vertex.pos = positions[7];
    vertex.texcoord = texcoords[14];
    vertex.normal = normals[5];
    vertex.tangent = getTangent(&normals, &tangents, vertex.normal);
    points[35] = vertex;

    std::vector<uint32_t> indices(36);
    for (uint32_t i = 0; i < 36; i++)
    {
        indices[i] = i;
    }

    ab->bind().setData(points);
    eab->bind().setIndices(indices);

    return glow::VertexArray::create(ab, eab, GL_TRIANGLES);
}

glow::SharedVertexArray MeshHelper::createBox(float x, float y, float z)
{
    glm::vec3 min(-x, -y, -z);
    glm::vec3 max(x, y, z);
    return createBox(min, max);
}
