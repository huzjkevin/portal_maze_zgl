#include "QueryHandler.hh"

#include "QueryTree.hh"
#include "Renderer.hh"

void QueryHandler::setup(int src, int tar, bool c, bool s, bool b) {
    source = src;
    target = tar;
    cancel = c;
    shift = s;
    back = b;
}

void QueryHandler::onJump() {
    if (cancel)
    {
        Renderer::getInstance()->getQueryTree()->cancelAll();
    }
}

void QueryHandler::onTeleport() {
    onJump();
    if (source >= 0 && shift)
    {
        Renderer::getInstance()->getQueryTree()->shift(source);
    }
    if (target >= 0 && back)
    {
        Renderer::getInstance()->getQueryTree()->makeVisible(target);
    }
}

void QueryHandler::onPullback() {
    onJump();
    if (target >= 0 && shift)
    {
        Renderer::getInstance()->getQueryTree()->shift(target);
    }
    if (source >= 0 && back)
    {
        Renderer::getInstance()->getQueryTree()->makeVisible(source);
    }
}
