#pragma once

#include "../forward.hh"

class MazeFace2D
{
private:
    std::vector<int32_t> vertices;
    std::vector<int32_t> edges;

public:
    MazeFace2D() : vertices(), edges() {}

    MazeFace2D(size_t n) : vertices(), edges()
    {
        vertices.reserve(n);
        edges.reserve(n);
    }

    virtual ~MazeFace2D() {}

    size_t getVertexCount() const { return vertices.size(); }
    int32_t getVertex(size_t index) const { return vertices[index]; }

    void addVertex(int32_t vertex) { vertices.push_back(vertex); }
    void insertVertex(size_t index, int32_t vertex) { vertices.insert(vertices.begin() + index, vertex); }

    void removeVertex(size_t index) { vertices.erase(vertices.begin() + index); }
    void clearVertices() { vertices.clear(); }

    size_t getEdgeCount() const { return edges.size(); }
    int32_t getEdge(size_t index) const { return edges[index]; }

    void addEdge(int32_t edge) { edges.push_back(edge); }
    void insertEdge(size_t index, int32_t edge) { edges.insert(edges.begin() + index, edge); }

    void removeEdge(size_t index) { edges.erase(edges.begin() + index); }
    void clearEdges() { edges.clear(); }

    void shrink_to_fit()
    {
        vertices.shrink_to_fit();
        edges.shrink_to_fit();
    }

    bool isValid() const { return (vertices.size() == edges.size()); }
};
