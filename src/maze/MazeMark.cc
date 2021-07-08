#include "MazeMark.hh"

#include "Maze2D.hh"

#include "../render/Graph.hh"

#include <glow/objects/ArrayBuffer.hh>
#include <glow/objects/ElementArrayBuffer.hh>
#include <glow/objects/VertexArray.hh>

MazeMark::MazeMark(SharedMaze2D m, uint32_t f, float w, float l, float h) : maze(m), face(f), mesh() {
    createMesh(w, l, h);
}

MazeMark::~MazeMark() {

}

void MazeMark::createMesh(float w, float l, float h) {
    auto ab = glow::ArrayBuffer::create();
    auto eab = glow::ElementArrayBuffer::create();

    ab->defineAttributes(
        {{&MarkVertex::pos, "aPosition"}, {&MarkVertex::normal, "aNormal"}, {&MarkVertex::tangent, "aTangent"}, {&MarkVertex::texcoord, "aTexCoord"}});

    glm::vec3 center = maze->getCenter(face);

    MarkVertex vertex;
    vertex.normal = {0.0f, 1.0f, 0.0f};
    vertex.tangent = {1.0f, 0.0f, 0.0f};
    vertex.texcoord = {0.0f, 0.0f};

    std::vector<MarkVertex> vertices(12);
    vertex.pos = {-w / 2.0f - l / 2.0f, 0.0f, w / 2.0f};
    vertices[0] = vertex;
    vertex.pos = {-w / 2.0f - l / 2.0f, 0.0f, -w / 2.0f};
    vertices[1] = vertex;
    vertex.pos = {-w / 2.0f, 0.0f, -w / 2.0f - l / 2.0f};
    vertices[2] = vertex;
    vertex.pos = {w / 2.0f, 0.0f, -w / 2.0f - l / 2.0f};
    vertices[3] = vertex;
    vertex.pos = {w / 2.0f + l / 2.0f, 0.0f, -w / 2.0f};
    vertices[4] = vertex;
    vertex.pos = {w / 2.0f + l / 2.0f, 0.0f, w / 2.0f};
    vertices[5] = vertex;
    vertex.pos = {w / 2.0f, 0.0f, w / 2.0f + l / 2.0f};
    vertices[6] = vertex;
    vertex.pos = {-w / 2.0f, 0.0f, w / 2.0f + l / 2.0f};
    vertices[7] = vertex;
    vertex.pos = {-w / 2.0f, 0.0f, w / 2.0f};
    vertices[8] = vertex;
    vertex.pos = {-w / 2.0f, 0.0f, -w / 2.0f};
    vertices[9] = vertex;
    vertex.pos = {w / 2.0f, 0.0f, -w / 2.0f};
    vertices[10] = vertex;
    vertex.pos = {w / 2.0f, 0.0f, w / 2.0f};
    vertices[11] = vertex;

    for (int i = 0; i < 12; i++)
    {
        vertices[i].pos = vertices[i].pos + center + h * glm::vec3(0.0f, 1.0f, 0.0f);
    }

    std::vector<uint32_t> indices(30);

    indices[0] = 0;
    indices[1] = 8;
    indices[2] = 9;
    indices[3] = 0;
    indices[4] = 9;
    indices[5] = 1;

    indices[6] = 9;
    indices[7] = 10;
    indices[8] = 3;
    indices[9] = 9;
    indices[10] = 3;
    indices[11] = 2;

    indices[12] = 11;
    indices[13] = 5;
    indices[14] = 4;
    indices[15] = 11;
    indices[16] = 4;
    indices[17] = 10;

    indices[18] = 7;
    indices[19] = 6;
    indices[20] = 11;
    indices[21] = 7;
    indices[22] = 11;
    indices[23] = 8;

    indices[24] = 8;
    indices[25] = 11;
    indices[26] = 10;
    indices[27] = 8;
    indices[28] = 10;
    indices[29] = 9;

    ab->bind().setData(vertices);
    eab->bind().setIndices(indices);

    mesh = glow::VertexArray::create(ab, eab, GL_TRIANGLES);

    node = std::make_shared<MeshGroup>();
    node->setMesh(mesh);

    node->addChild(std::make_shared<Leaf>());
}

void MazeMark::add(SharedGroup group) {
    group->addChild(node);
}

void MazeMark::remove(SharedGroup group) {
    group->removeChild(node);
}

MazeMarkList::MazeMarkList() : list(), node() {
    node = std::make_shared<TextureGroup>();
    glow::SharedTexture2D redAlbedo = glow::Texture2D::createFromFile("../data/textures/red.albedo.png", glow::ColorSpace::sRGB);
    glow::SharedTexture2D redNormal = glow::Texture2D::createFromFile("../data/textures/red.normal.png", glow::ColorSpace::Linear);
    node->setAlbedo(redAlbedo);
    node->setNormal(redNormal);
}

MazeMarkList::~MazeMarkList() {

}

int32_t MazeMarkList::indexOf(SharedMaze2D maze, uint32_t face)
{
    for (size_t i = 0; i < list.size(); i++)
    {
        SharedMazeMark mark = list[i];
        if (mark->getMaze() == maze && mark->getFace() == face)
        {
            return i;
        }
    }
    return -1;
}

bool MazeMarkList::isMarked(SharedMaze2D maze, uint32_t face)
{
    return indexOf(maze, face) != -1;
}

void MazeMarkList::mark(SharedMaze2D maze, uint32_t face, float w, float l, float h)
{
    SharedMazeMark mark = std::make_shared<MazeMark>(maze, face, w, l, h);
    mark->add(node);
    list.push_back(mark);
}

void MazeMarkList::remove(int32_t index)
{
    SharedMazeMark mark = list[index];
    mark->remove(node);
    list.erase(list.begin() + index);
}

void MazeMarkList::flip(SharedMaze2D maze, uint32_t face, float w, float l, float h)
{
    int32_t index = indexOf(maze, face);
    if (index < 0)
    {
        mark(maze, face, w, l, h);
    }
    else
    {
        remove(index);
    }
}

void MazeMarkList::reset() {
    while (list.size() > 0)
    {
        remove(0);
    }
}
