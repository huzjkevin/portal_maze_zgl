#include "Stats.hh"

SharedStats Stats::instance;

void Stats::initInstance() {
    instance = std::make_shared<Stats>();
    instance->init();
}

void Stats::init() {

}

void Stats::reset() {

}

void Stats::onFrameStart() {
    frame++;
    passes = 0;
}

void Stats::onFrameEnd() {
    //glow::info() << "Rendering passes: " << passes;
}

void Stats::countPass() {
    passes++;
}
