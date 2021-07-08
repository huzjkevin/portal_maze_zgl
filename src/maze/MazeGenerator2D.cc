#include "MazeGenerator2D.hh"

#include "Maze2D.hh"
#include "MazeFace2D.hh"
#include "MazeHalfedge2D.hh"
#include "WallState.hh"

#include <set>
#include <stack>

void DFSMazeGenerator::generate(SharedMaze2D maze) const
{
    int32_t initial = maze->getInitial();
    if (initial == NO_FACE)
    {
        initial = 0;
    }

    std::stack<uint32_t> stack;
    std::set<uint32_t> reached;

    stack.push(initial);
    reached.insert(initial);

    int32_t current;
    int32_t opposite;

    int32_t n;
    int32_t halfedge;

    std::vector<int32_t> possibilities;

    while (!stack.empty())
    {
        current = stack.top();

        n = maze->getFace(current)->getEdgeCount();
        possibilities.clear();

        for (int32_t i = 0; i < n; i++)
        {
            halfedge = maze->getFace(current)->getEdge(i);
            opposite = maze->getHalfedge(halfedge)->getOpposite();
            if (opposite != NO_HALFEDGE)
            {
                opposite = maze->getHalfedge(opposite)->getFace();
                if (opposite != NO_FACE && reached.find(opposite) == reached.end() && !maze->isBarrier(halfedge) && !maze->isCorridor(halfedge))
                {
                    possibilities.push_back(halfedge);
                }
            }
        }

        if (possibilities.empty())
        {
            stack.pop();
        }
        else
        {
            halfedge
                = possibilities[((int32_t)((static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * possibilities.size())) % possibilities.size()];
            maze->makeCorridor(halfedge);
            opposite = maze->getHalfedge(maze->getHalfedge(halfedge)->getOpposite())->getFace();
            stack.push(opposite);
            reached.insert(opposite);
        }
    }

    maze->resolveUndeterminedWalls(std::make_shared<WallStateChooser>(std::make_shared<WallState>(WALL_SOLID)), false);
}

void BFSMazeGenerator::generate(SharedMaze2D maze) const {}

void KruskalMazeGenerator::generate(SharedMaze2D maze) const {}

void PrimMazeGenerator::generate(SharedMaze2D maze) const {}

void ReverseDeleteMazeGenerator::generate(SharedMaze2D maze) const {}

void WilsonMazeGenerator::generate(SharedMaze2D maze) const {}

void AldousBroderMazeGenerator::generate(SharedMaze2D maze) const {}
