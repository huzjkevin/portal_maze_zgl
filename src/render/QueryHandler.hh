#pragma once

#include "../forward.hh"

class QueryHandler
{
private:
    int source;
    int target;

    bool cancel;
    bool shift;
    bool back;

public:
    QueryHandler() : source(-1), target(-1), cancel(true), shift(true), back(true) {}
    virtual ~QueryHandler() {}

    int getSource() { return source; }
    void setSource(int s) { source = s; }

    int getTarget() { return target; }
    void setTarget(int t) { target = t; }

    bool doCancel() { return cancel; }
    void setCancel(bool c) { cancel = c; }

    bool doShift() { return shift; }
    void setShift(bool s) { shift = s; }

    bool showBack() { return back; }
    void setBack(bool b) { back = b; }

    void setup(int src = -1, int tar = -1, bool c = true, bool s = true, bool b = true);

    void onJump();
    void onTeleport();
    void onPullback();
};
