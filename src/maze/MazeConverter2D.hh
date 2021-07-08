#pragma once

#include "../forward.hh"

class MazeConverter2D
{
private:
    bool ceiling;
    bool collision;

    std::vector<CollisionObject*>* bodies;
    std::vector<glow::SharedVertexArray>* meshes;

    std::vector<MazeVertex> vertexBuffer;
    std::vector<uint32_t> indexBuffer;

public:
    MazeConverter2D() : ceiling(true), collision(true), meshes(nullptr), vertexBuffer(), indexBuffer() {}
    MazeConverter2D(std::vector<glow::SharedVertexArray>* m) : meshes(m) {}

    virtual ~MazeConverter2D() {}

    bool getCeiling() { return ceiling; }
    void setCeiling(bool c) { ceiling = c; }

    bool getCollision() { return collision; }
    void setCollision(bool c) { collision = c; }

    void setBodies(std::vector<CollisionObject*>* b) { bodies = b; }
    void setMeshes(std::vector<glow::SharedVertexArray> * m) { meshes = m; }

    void add(std::vector<MazeVertex>* vertices, std::vector<uint32_t>* indices);
    void flush();

    void convert(SharedMaze2D maze);
    void convertHalfedge(SharedMaze2D maze, uint32_t halfedge, glm::vec2 start, glm::vec2 end, glm::vec2 innerStart, glm::vec2 innerEnd);

    float getCenter(SharedMaze2D maze, uint32_t halfedge, int anchor, float offset, float width) const;

    void makeWall(glm::vec2 left, glm::vec2 right, float base, float height, int32_t halfedge = -1);
    void makeRectangle(glm::vec3 tl, glm::vec3 tr, glm::vec3 bl, int32_t halfedge = -1, float texWidth = 1.0f, float texHeight = 1.0f, float texOffX = 0.0f, float texOffY = 0.0f);

    void rasterizeFace(std::vector<glm::vec2>* face,
                       float y,
                       glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f),
                       glm::vec3 tangent = glm::vec3(1.0f, 0.0f, 0.0f),
                       float texWidth = 1.0f,
                       float texHeight = 1.0f);
};
