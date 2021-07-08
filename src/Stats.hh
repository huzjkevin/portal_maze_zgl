#pragma once

#include <memory>

#include <glow/common/log.hh>

class Stats;

typedef std::shared_ptr<Stats> SharedStats;

class Stats
{
private:
    static SharedStats instance;

    uint32_t frame;
    uint32_t passes;

public:
    Stats() : frame(0), passes(0) {}
    virtual ~Stats() {}

    static SharedStats getInstance() { return instance; }

    static void initInstance();

    void init();
    void reset();

    uint32_t getFrame() { return frame; }
    void setFrame(uint32_t f) { frame = f; }

    void onFrameStart();
    void onFrameEnd();

    void countPass();
};
