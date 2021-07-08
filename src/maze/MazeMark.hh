#pragma once

#include "../forward.hh"

#include <glow/objects/ArrayBuffer.hh>
#include <glow/objects/ElementArrayBuffer.hh>
#include <glow/objects/VertexArray.hh>

#include "../render/Drawable.hh"
#include "Maze2D.hh"

class MazeMark
{
private:
    SharedMaze2D maze;
    uint32_t face;

    glow::SharedVertexArray mesh;
    SharedMeshGroup node;

public:
    MazeMark(SharedMaze2D m, uint32_t f, float w = DEFAULT_MARK_W, float l = DEFAULT_MARK_L, float h = DEFAULT_MARK_H);
    virtual ~MazeMark();

    SharedMaze2D getMaze() { return maze; }
    uint32_t getFace() { return face; }

    void createMesh(float w, float l, float h);

    void add(SharedGroup group);

    void remove(SharedGroup group);
};

class MazeMarkList
{
private:
    std::vector<SharedMazeMark> list;
    SharedTextureGroup node;

public:
    MazeMarkList();
    virtual ~MazeMarkList();

    SharedTextureGroup getNode() { return node; }

    int32_t indexOf(SharedMaze2D maze, uint32_t face);

    bool isMarked(SharedMaze2D maze, uint32_t face);

    void mark(SharedMaze2D maze, uint32_t face, float w = DEFAULT_MARK_W, float l = DEFAULT_MARK_L, float h = DEFAULT_MARK_H);

    void remove(int32_t index);

    void flip(SharedMaze2D maze, uint32_t face, float w = DEFAULT_MARK_W, float l = DEFAULT_MARK_L, float h = DEFAULT_MARK_H);

    void reset();
};
