#include "Portal.hh"

#include <glow/objects/ArrayBuffer.hh>
#include <glow/objects/ElementArrayBuffer.hh>
#include <glow/objects/Program.hh>
#include <glow/objects/VertexArray.hh>

Portal::Portal(bool m, float w, float h) : mesh(), source(), target(), inverse(), mirror(m), width(w), height(h) {
    createQuadMesh();
}

Portal::~Portal() {

}

void Portal::createQuadMesh() {
    auto ab = glow::ArrayBuffer::create();
    auto eab = glow::ElementArrayBuffer::create();

    ab->defineAttributes(
        {{&PortalVertex::pos, "aPosition"}, {&PortalVertex::normal, "aNormal"}, {&PortalVertex::tangent, "aTangent"}, {&PortalVertex::texcoord, "aTexCoord"}});

    PortalVertex vertex;
    vertex.normal = {0.0f, 0.0f, 1.0f};
    vertex.tangent = {1.0f, 0.0f, 0.0f};

    std::vector<PortalVertex> points(4);
    vertex.pos = {-width / 2.0f, -height / 2.0f, 0.0f};
    vertex.texcoord = {0.0, 1.0};
    points[0] = vertex;
    vertex.pos = {-width / 2.0f, height / 2.0f, 0.0f};
    vertex.texcoord = {0.0, 0.0};
    points[1] = vertex;
    vertex.pos = {width / 2.0f, -height / 2.0f, 0.0f};
    vertex.texcoord = {1.0, 1.0};
    points[2] = vertex;
    vertex.pos = {width / 2.0f, height / 2.0f, 0.0f};
    vertex.texcoord = {1.0, 0.0};
    points[3] = vertex;

    std::vector<uint32_t> indices(6);
    indices[0] = 0;
    indices[1] = 2;
    indices[2] = 3;
    indices[3] = 0;
    indices[4] = 3;
    indices[5] = 1;

    ab->bind().setData(points);
    eab->bind().setIndices(indices);

    mesh = glow::VertexArray::create(ab, eab, GL_TRIANGLES);
}

glm::mat4 Portal::transformView(glm::mat4 view) const
{
    return view * source * inverse;
}

glm::vec4 Portal::calculateClipPlane(float guard) const
{
    glm::vec4 clipPlane;
    glm::vec3 position = target * glm::vec4(0, 0, 0, 1);
    glm::vec3 normal = target * glm::vec4(0, 0, 1, 0);
    
    clipPlane.x = normal.x;
    clipPlane.y = normal.y;
    clipPlane.z = normal.z;
    clipPlane.w = -glm::dot(position, normal) - guard;

    return clipPlane;
}
