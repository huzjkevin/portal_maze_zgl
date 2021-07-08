#pragma once

#include "../forward.hh"

class MazeGenerator2D
{
public:
    MazeGenerator2D() {}
    virtual ~MazeGenerator2D() {}

    virtual void generate(SharedMaze2D maze) const = 0;
};

class DFSMazeGenerator : public MazeGenerator2D
{
public:
    DFSMazeGenerator() : MazeGenerator2D() {}
    virtual ~DFSMazeGenerator() {}

    virtual void generate(SharedMaze2D maze) const;
};

class BFSMazeGenerator : public MazeGenerator2D
{
public:
    BFSMazeGenerator() : MazeGenerator2D() {}
    virtual ~BFSMazeGenerator() {}

    virtual void generate(SharedMaze2D maze) const;
};

class KruskalMazeGenerator : public MazeGenerator2D
{
public:
    KruskalMazeGenerator() : MazeGenerator2D() {}
    virtual ~KruskalMazeGenerator() {}

    virtual void generate(SharedMaze2D maze) const;
};

class PrimMazeGenerator : public MazeGenerator2D
{
public:
    PrimMazeGenerator() : MazeGenerator2D() {}
    virtual ~PrimMazeGenerator() {}

    virtual void generate(SharedMaze2D maze) const;
};

class ReverseDeleteMazeGenerator : public MazeGenerator2D
{
public:
    ReverseDeleteMazeGenerator() : MazeGenerator2D() {}
    virtual ~ReverseDeleteMazeGenerator() {}

    virtual void generate(SharedMaze2D maze) const;
};

class WilsonMazeGenerator : public MazeGenerator2D
{
public:
    WilsonMazeGenerator() : MazeGenerator2D() {}
    virtual ~WilsonMazeGenerator() {}

    virtual void generate(SharedMaze2D maze) const;
};

class AldousBroderMazeGenerator : public MazeGenerator2D
{
public:
    AldousBroderMazeGenerator() : MazeGenerator2D() {}
    virtual ~AldousBroderMazeGenerator() {}

    virtual void generate(SharedMaze2D maze) const;
};
