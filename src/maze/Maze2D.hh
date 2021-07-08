#pragma once

#include "../forward.hh"

class Maze2D
{
private:
    std::vector<SharedMazeVertex2D> vertices;
    std::vector<SharedMazeFace2D> faces;

    std::vector<SharedMazeHalfedge2D> halfedges;
    std::vector<SharedWallState> walls;

    int32_t initial;
    SharedWallStateChooser corridor;
    SharedWallStateChooser barrier;

public:
    Maze2D();
    Maze2D(size_t v);
    Maze2D(size_t v, size_t f);
    Maze2D(size_t v, size_t f, size_t h);
    virtual ~Maze2D();

    size_t getVertexCount() const;
    SharedMazeVertex2D getVertex(size_t index) const;

    int32_t addVertex(SharedMazeVertex2D vertex);
    int32_t insertVertex(size_t index, SharedMazeVertex2D vertex);

    void removeVertex(size_t index);
    void clearVertices();

    size_t getFaceCount() const;
    SharedMazeFace2D getFace(size_t index) const;

    int32_t addFace(SharedMazeFace2D face);
    int32_t insertFace(size_t index, SharedMazeFace2D face);

    void removeFace(size_t index);
    void clearFaces();

    size_t getHalfedgeCount() const;
    SharedMazeHalfedge2D getHalfedge(size_t index) const;

    int32_t addHalfedge(SharedMazeHalfedge2D edge);
    int32_t insertHalfedge(size_t index, SharedMazeHalfedge2D edge);

    void removeHalfedge(size_t index);
    void clearHalfedges();

    SharedWallState getWallState(size_t index) const;
    void setWallState(size_t index, SharedWallState state, bool invert = true);

    int32_t getInitial() const;
    void setInitial(int32_t init);

    SharedWallStateChooser getBarrierChooser();
    SharedWallStateChooser getCorridorChooser();

    void setBarrierChooser(SharedWallStateChooser bar);
    void setCorridorChooser(SharedWallStateChooser cor);

    int32_t createVertex(glm::vec2 location);
    int32_t createFace(std::vector<int32_t> vertices);

    int32_t createHalfedge(int32_t start = NO_VERTEX, int32_t end = NO_VERTEX, int32_t face = NO_FACE, int32_t opposite = NO_HALFEDGE);
    void connectHalfedge(int32_t halfedge, int32_t opposite);

    void connect();

    glm::vec2 getDirection(size_t halfedge) const;
    glm::vec2 inverseDirection(size_t halfedge) const;

    float getCosAngle(size_t first, size_t second) const;
    float getSinAngle(size_t first, size_t second) const;
    float getAngle(size_t first, size_t second) const;

    glm::vec2 getInnerStart(size_t halfedge, size_t prev) const;
    glm::vec2 getInnerEnd(size_t halfedge, size_t next) const;

    glm::vec2 getInnerStart(size_t halfedge) const;
    glm::vec2 getInnerEnd(size_t halfedge) const;

    float projectInnerStart(size_t halfedge) const;
    float projectInnerEnd(size_t halfedge) const;

    float getLeftmost(size_t halfedge) const;
    float getRightmost(size_t halfedge) const;

    bool isCorridor(size_t halfedge, bool opposite = true) const;
    bool isBarrier(size_t halfedge, bool opposite = true) const;

    void makeCorridor(size_t halfedge, bool opposite = true, bool force = false);
    void makeBarrier(size_t halfedge, bool opposite = true, bool force = false);

    void applyWallState(SharedWallStateChooser chooser, int mode = ONLY_INNER);
    void resolveUndeterminedWalls(SharedWallStateChooser chooser, bool twoside = true);

    void setWallBase(float base);
    void setWallHeight(float height);
    void setWallThickness(float thickness);

    glm::mat4 attachRectangle(size_t halfedge,
                              float width,
                              float height,
                              float sgn,
                              int anchor = WALL_CENTER,
                              float offset = 0.0f,
                              int vAnchor = VERT_BOTTOM,
                              float vOffset = 0.0f,
                              float guard = DEFAULT_GUARD,
                              float mirror = NO_MIRROR,
                              int zAnchor = Z_ANCHOR_INNER,
                              float zOffset = 0.0f) const;

    glm::vec3 getLeftDirection(size_t halfedge) const;
    glm::vec3 getRightDirection(size_t halfedge) const;

    glm::vec3 getUpDirection(size_t halfedge) const;
    glm::vec3 getDownDirection(size_t halfedge) const;

    glm::vec3 getInDirection(size_t halfedge) const;
    glm::vec3 getOutDirection(size_t halfedge) const;

    int32_t findVertex(glm::vec2 location, float tolerance = DEFAULT_TOLERANCE) const;
    int32_t findHalfedge(int32_t start, int32_t end) const;
    int32_t findFace(glm::vec2 location) const;

    glm::vec3 getCenter(size_t face) const;
    uint32_t lookAtHalfedge(size_t face, glm::vec2 location, glm::vec2 direction) const;
    glm::vec3 directionToHalfedge(size_t face, size_t halfedge) const;

    static std::shared_ptr<Maze2D> createGrid(uint32_t xTiles, uint32_t zTiles, float xTileWidth, float zTileWidth, float xCenter = 0.0f, float zCenter = 0.0f);
    static std::shared_ptr<Maze2D> createHexMaze(uint32_t xTiles, uint32_t zTiles, float xWidth, float zWidth, float xCenter = 0.0f, float zCenter = 0.0f);
};
