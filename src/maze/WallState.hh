#pragma once

#include "../forward.hh"

class WallState
{
private:
    int32_t state;

    float thickness;
    float base;
    float height;

public:
    WallState(int32_t s = DEFAULT_STATE, float t = DEFAULT_THICKNESS, float b = DEFAULT_BASE, float h = DEFAULT_HEIGHT)
      : state(s), thickness(t), base(b), height(h)
    {
    }

    virtual ~WallState() {}

    virtual int32_t getState() const { return state; }
    virtual float getThickness() const { return thickness; }
    virtual float getBase() const { return base; }
    virtual float getHeight() const { return height; }

    virtual void setState(int32_t s = DEFAULT_STATE) { state = s; }
    virtual void setThickness(float t = DEFAULT_THICKNESS) { thickness = t; }
    virtual void setBase(float b = DEFAULT_BASE) { base = b; }
    virtual void setHeight(float h = DEFAULT_HEIGHT) { height = h; }

    virtual SharedWallState copy() const { return std::make_shared<WallState>(state, thickness, base, height); }
    virtual SharedWallState invert() const { return std::make_shared<WallState>(state, thickness, base, height); }
};

class DoorWallState : public WallState
{
private:
    int anchor;
    float width;
    float offset;

public:
    DoorWallState(int anch = WALL_CENTER, float w = DEFAULT_WIDTH, float off = DEFAULT_OFFSET)
      : WallState(WALL_DOOR), anchor(anch), width(w), offset(off)
    {
    }

    virtual ~DoorWallState() {}

    virtual int getAnchor() const { return anchor; }
    virtual float getWidth() const { return width; }
    virtual float getOffset() const { return offset; }

    virtual void setAnchor(int anch) { anchor = anch; }
    virtual void setWidth(float w) { width = w; }
    virtual void setOffset(float off) { offset = off; }

    virtual SharedWallState copy() const
    {
        std::shared_ptr<DoorWallState> result = std::make_shared<DoorWallState>(anchor, width, offset);
        result->setThickness(getThickness());
        result->setBase(getBase());
        result->setHeight(getHeight());
        return result;
    }

    virtual SharedWallState invert() const
    {
        std::shared_ptr<DoorWallState> result = std::make_shared<DoorWallState>(-anchor, width, -offset);
        result->setThickness(getThickness());
        result->setBase(getBase());
        result->setHeight(getHeight());
        return result;
    }
};

class WallStateChooser
{
private:
    std::vector<float> probabilities;
    std::vector<SharedWallState> states;

public:
    WallStateChooser() : probabilities(), states() { states.push_back(std::make_shared<WallState>(WALL_NONE)); }

    WallStateChooser(SharedWallState def) : probabilities(), states() { states.push_back(def); }

    virtual ~WallStateChooser() {}

    virtual void clear(SharedWallState def = nullptr)
    {
        probabilities.clear();
        states.clear();
        states.push_back(def == nullptr ? std::make_shared<WallState>(WALL_NONE) : def);
    }

    virtual void add(float probability, SharedWallState state)
    {
        probabilities.insert(probabilities.begin(), probability);
        states.insert(states.begin(), state);
    }

    virtual SharedWallState choose(float random = -1.0f) const
    {
        if (random < 0.0f)
        {
            random = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        }

        size_t i;
        float sum = 0.0f;

        for (i = 0; i < probabilities.size(); i++)
        {
            sum += probabilities[i];
            if (random < sum)
                break;
        }

        return states[i]->copy();
    }
};
