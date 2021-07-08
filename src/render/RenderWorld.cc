#include "RenderWorld.hh"

SharedRenderWorld RenderWorld::instance;

void RenderWorld::initInstance()
{
    instance = std::make_shared<RenderWorld>();
    instance->init();
}

void RenderWorld::reset() {
    instance = nullptr;
}

void RenderWorld::init()
{
    root = std::make_shared<Root>();
}
