#include "Game.hh"

#include <glm/ext.hpp>
#include <imgui/imgui.h>

#include "load_mesh.hh"

/*
#include <glow/common/scoped_gl.hh>
#include <glow/objects/ArrayBuffer.hh>
#include <glow/objects/Framebuffer.hh>
#include <glow/objects/Program.hh>
#include <glow/objects/Texture2D.hh>
#include <glow/objects/TextureRectangle.hh>
#include <glow/objects/VertexArray.hh>
*/

/*
#include <glow-extras/geometry/Quad.hh>
#include <glow-extras/geometry/UVSphere.hh>
*/

#include "Stats.hh"

#include "render/Renderer.hh"
#include "render/RenderWorld.hh"
#include "collision/CollisionWorld.hh"
#include "logic/LogicWorld.hh"

#include <GLFW/glfw3.h> // window/input framework

const static ImVec2 BUTTON_SIZE(120, 30);

Game::Game() : GlfwApp(Gui::ImGui), resetOld(false), resetNew(false), levelOld(-1), levelNew(-1), currentLevel(0) {}

void Game::init()
{
    setVSync(true);
    GlfwApp::init();
    setTitle("Maze");

    Stats::initInstance();
    Renderer::initInstance();

    RenderWorld::initInstance();
    CollisionWorld::initInstance();
    LogicWorld::initInstance(this);
}

void Game::onClose()
{
    glow::glfw::GlfwApp::onClose();
}

Game::~Game()
{
    
}

void Game::update(float elapsedSeconds)
{
    LogicWorld::getInstance()->tick(elapsedSeconds);
}

void Game::render(float elapsedSeconds)
{
    LogicWorld::getInstance()->update(elapsedSeconds);
    Renderer::getInstance()->render();
}

void Game::onGui()
{
    if (ImGui::IsKeyReleased(ImGui::GetKeyIndex(ImGuiKey_Escape)))
    {
        switch (mShowUI)
        {
        case Game::None:
        case Game::Setting:
            mShowUI = SelectLevel;
            break;
        case Game::SelectLevel:
            mShowUI = None;
            break;
        default:
            mShowUI = None;
            break;
        }
    }
    LogicWorld::getInstance()->setEscape(mShowUI != None);
    setCursorMode(mShowUI != None ? glow::glfw::CursorMode::Normal : glow::glfw::CursorMode::Disabled);

    if (mShowUI == Setting)
    {
        ImGui::OpenPopup("ZGL Portal Maze");
        if (ImGui::BeginPopupModal("ZGL Portal Maze", 0, ImGuiWindowFlags_NoCollapse))
        {
            /*

            ImGui::Text("Objects :");
            {
            ImGui::Indent();
            ImGui::SliderFloat("Sphere Radius", &mSphereSize, 0.0f, 10.0f);
            ImGui::SliderFloat3("Sphere Position", &mSpherePosition.x, -5.0f, 5.0f);
            ImGui::Unindent();
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            */

            ImGui::Text("Graphics:");
            {
                ImGui::Indent();
                ImGui::Checkbox("Wireframe", Renderer::getInstance()->showWireframePtr());
                ImGui::Checkbox("FXAA", Renderer::getInstance()->enableFXAAPtr());
                ImGui::ColorEdit3("Background Color", Renderer::getInstance()->backgroundColorPtr());
                ImGui::ColorEdit3("Portal Fade Color", Renderer::getInstance()->fadeColorPtr());
                ImGui::Unindent();
            }
            ImGui::EndPopup();
        }
    }
    else if (mShowUI == SelectLevel)
    {
        ImGui::OpenPopup("ZGL Portal Maze");
        if (ImGui::BeginPopupModal("ZGL Portal Maze", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
        {
            ImGui::SetWindowSize(ImVec2(180, 300));
            ImGui::Text("Current Level: ");
            {
                ImGui::Indent();
                resetNew = ImGui::Button("Restart", BUTTON_SIZE);
                ImGui::Unindent();
            }
            ImGui::NewLine();
            ImGui::Text("Load Level: ");
            {
                ImGui::Indent();

                levelNew = -1;

                if (ImGui::Button("Final", BUTTON_SIZE))
                {
                    levelNew = 0;
                }
                if (ImGui::Button("Random", BUTTON_SIZE))
                {
                    levelNew = 1;
                }
                if (ImGui::Button("Test", BUTTON_SIZE))
                {
                    levelNew = 2;
                }
                if (ImGui::Button("Trick", BUTTON_SIZE))
                {
                    levelNew = 3;
                }
                if (ImGui::Button("MS 3", BUTTON_SIZE))
                {
                    levelNew = 4;
                }
                ImGui::Unindent();
            }
            ImGui::NewLine();
            ImGui::Text("Graphics: ");
            {
                ImGui::Indent();
                if (ImGui::Button("Settings", BUTTON_SIZE))
                {
                    mShowUI = Setting;
                }
                ImGui::Unindent();
            }
            ImGui::EndPopup();
        }
    }

    if (!resetOld && resetNew)
    {
        reset(currentLevel);
    }

    resetOld = resetNew;

    if (levelNew >= 0 && levelNew != levelOld)
    {
        reset(levelNew);
    }

    levelOld = levelNew;
}

void Game::reset(int level) {
    currentLevel = level;

    Stats::getInstance()->reset();
    Renderer::getInstance()->reset();

    LogicWorld::reset();
    CollisionWorld::reset();
    RenderWorld::reset();

    RenderWorld::initInstance();
    CollisionWorld::initInstance();
    LogicWorld::initInstance(this);

    LogicWorld::getInstance()->resize(width, height);
}

void Game::onResize(int w, int h)
{
    w = w > 0 ? w : 1;
    h = h > 0 ? h : 1;

    width = w;
    height = h;

    LogicWorld::getInstance()->resize(w, h);
    Renderer::getInstance()->resize(w, h);
}

int Game::getCurrentLevel() {
    return currentLevel;
}
