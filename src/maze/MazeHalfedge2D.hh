#pragma once

#include <memory>

class MazeHalfedge2D
{

private:

    int32_t startVertex;
    int32_t endVertex;

    int32_t face;
    int32_t opposite;

public:

    MazeHalfedge2D(int32_t start = NO_VERTEX, int32_t end = NO_VERTEX, int32_t f = NO_FACE, int32_t opp = NO_HALFEDGE)
      : startVertex(start), endVertex(end), face(f), opposite(opp)
    {

    }

    virtual ~MazeHalfedge2D()
    {
        
    }

    int32_t getStartVertex() const { return startVertex; }
    int32_t getEndVertex() const { return endVertex; }

    int32_t getFace() const { return face; }
    int32_t getOpposite() const { return opposite; }

    void setStartVertex(int32_t start) { startVertex = start; }
    void setEndVertex(int32_t end) { endVertex = end; }

    void setFace(int32_t f) { face = f; }
    void setOpposite(int32_t opp) { opposite = opp; }

    bool isBoundary() const { return (opposite == NO_HALFEDGE); }

};
