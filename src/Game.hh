#pragma once

#include "../forward.hh"
#include <glow-extras/glfw/GlfwApp.hh>

class Game : public glow::glfw::GlfwApp
{
private:
    bool resetOld;
    bool resetNew;

    int levelOld;
    int levelNew;

    int currentLevel;

    int width;
    int height;


    enum UIName
    {
        None,
        SelectLevel,
        Setting
    };
    UIName mShowUI = None;

public:
    Game();
    ~Game();

    void init() override;                       // called once after OpenGL is set up
    void update(float elapsedSeconds) override; // called in 60 Hz fixed timestep
    void render(float elapsedSeconds) override; // called once per frame (variable timestep)
    void onGui() override;                      // called once per frame to set up UI

    void onResize(int w, int h) override;       // called when window is resized

    void reset(int level);
    void onClose();

    int getCurrentLevel();
};
