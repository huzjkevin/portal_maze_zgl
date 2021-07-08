#pragma once

#include "../forward.hh"

class MazeVertex2D
{
private:
    glm::vec2 location;

    std::vector<int32_t> faces;

    std::vector<int32_t> incoming;
    std::vector<int32_t> outgoing;

public:
    MazeVertex2D(glm::vec2 loc) : location(loc), faces(), incoming(), outgoing() {}

    MazeVertex2D(glm::vec2 loc, size_t n) : location(loc), faces(), incoming(), outgoing()
    {
        faces.reserve(n);
        incoming.reserve(n);
        outgoing.reserve(n);
    }

    virtual ~MazeVertex2D() {}

    glm::vec2 getLocation() const { return location; }
    void setLocation(glm::vec2 loc) { location = loc; }

    size_t getFaceCount() const { return faces.size(); }
    int32_t getFace(size_t index) const { return faces[index]; }

    void addFace(int32_t face) { faces.push_back(face); }
    void insertFace(size_t index, int32_t face) { faces.insert(faces.begin() + index, face); }

    void removeFace(size_t index) { faces.erase(faces.begin() + index); }
    void clearFaces() { faces.clear(); }

    size_t getIncomingCount() const { return incoming.size(); }
    int32_t getIncomingEdge(size_t index) const { return incoming[index]; }

    void addIncomingEdge(int32_t edge) { incoming.push_back(edge); }
    void insertIncomingEdge(size_t index, int32_t edge) { incoming.insert(incoming.begin() + index, edge); }

    void removeIncomingEdge(size_t index) { incoming.erase(incoming.begin() + index); }
    void clearIncoming() { incoming.clear(); }

    size_t getOutgoingCount() const { return outgoing.size(); }
    int32_t getOutgoingEdge(size_t index) const { return outgoing[index]; }

    void addOutgoingEdge(int32_t edge) { outgoing.push_back(edge); }
    void insertOutgoingEdge(size_t index, int32_t edge) { outgoing.insert(outgoing.begin() + index, edge); }

    void removeOutgoingEdge(size_t index) { outgoing.erase(outgoing.begin() + index); }
    void clearOutgoing() { outgoing.clear(); }

    void shrink_to_fit()
    {
        faces.shrink_to_fit();
        incoming.shrink_to_fit();
        outgoing.shrink_to_fit();
    }

    bool isValid() const { return ((faces.size() == incoming.size()) && (incoming.size() == outgoing.size())); }
};
