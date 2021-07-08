#include "LogicWorld.hh"

#include "CameraObject.hh"
#include "DefaultLevel.hh"
#include "DynamicObject.hh"
#include "FinalLevel.hh"
#include "Level.hh"
#include "PlayerObject.hh"
#include "RandomLevel.hh"
#include "ReworkedLevel.hh"
#include "TrickLevel.hh"

#include "../collision/CollisionWorld.hh"
#include "../collision/Gate.hh"

#include "../Game.hh"

#include <GLFW/glfw3.h>

SharedLogicWorld LogicWorld::instance;

void LogicWorld::initInstance(Game* g)
{
    instance = std::make_shared<LogicWorld>(g);
    instance->init();
}

void LogicWorld::reset()
{
    instance = nullptr;
}

LogicWorld::LogicWorld(Game* g) : game(g), level(), player(), camera(), escape(false), objects(), oldEscape(false) {}

LogicWorld::~LogicWorld() {}

void LogicWorld::init()
{
    std::srand((unsigned int)std::time(0));
    std::rand();

    switch (game->getCurrentLevel())
    {
    case 0:
        level = std::make_shared<FinalLevel>();
        break;
    case 1:
        level = std::make_shared<RandomLevel>();
        break;
    case 2:
        level = std::make_shared<DefaultLevel>();
        break;
    case 3:
        level = std::make_shared<TrickLevel>();
        break;
    case 4:
        level = std::make_shared<ReworkedLevel>();
        break;
    default:
        level = std::make_shared<FinalLevel>();
        break;
    }

    level->init();
}

void LogicWorld::control(float elapsed)
{
    // if (game->isKeyPressed(GLFW_KEY_ESCAPE))
    //{
    //    //if (!oldEscape)
    //    {
    //        escape = !escape;
    //    }
    //}

    // oldEscape = game->isKeyPressed(GLFW_KEY_ESCAPE);

    // if (escape)
    //{
    //    game->setCursorMode(glow::glfw::CursorMode::Normal);
    //}
    // else
    //{
    //    game->setCursorMode(glow::glfw::CursorMode::Disabled);
    //}

    glm::vec3 direction(0.0f, 0.0f, 0.0f);
    if (game->isKeyPressed(GLFW_KEY_A))
        direction.z += elapsed * MOVE_FACTOR_SIDE;
    if (game->isKeyPressed(GLFW_KEY_D))
        direction.z -= elapsed * MOVE_FACTOR_SIDE;
    if (game->isKeyPressed(GLFW_KEY_W))
        direction.x -= elapsed * MOVE_FACTOR_STRAIGHT;
    if (game->isKeyPressed(GLFW_KEY_S))
        direction.x += elapsed * MOVE_FACTOR_STRAIGHT;
    if (game->isKeyPressed(GLFW_KEY_Q))
        direction.x -= elapsed * MOVE_FACTOR_VERTICAL;
    if (game->isKeyPressed(GLFW_KEY_E))
        direction.x += elapsed * MOVE_FACTOR_VERTICAL;

    if (game->isKeyPressed(GLFW_KEY_LEFT_SHIFT) || game->isKeyPressed(GLFW_KEY_RIGHT_SHIFT))
        direction *= MOVE_FACTOR_SHIFT;

    if (game->isKeyPressed(GLFW_KEY_LEFT_CONTROL) || game->isKeyPressed(GLFW_KEY_RIGHT_CONTROL))
        direction *= MOVE_FACTOR_CTRL;

    if (camera != nullptr)
    {
        if (direction[0] != 0.0f || direction[1] != 0.0f || direction[2] != 0.0f)
        {
            auto forward = camera->getForward();
            float angle = atan2(forward[2], -forward[0]);
            if (player != nullptr)
            {
                player->move(glm::rotateY(direction, angle));
            }
        }

        if (!escape)
        {
            glm::vec2 delta = game->input().getLastMouseDelta() / 100.0f;
            if (glm::abs(delta.x) < 1 && glm::abs(delta.y) < 1)
            {
                camera->rotate(delta.x, delta.y);
            }
        }
    }
}

void LogicWorld::resize(int w, int h)
{
    if (camera != nullptr)
    {
        camera->resize(w, h);
    }
}

void LogicWorld::tick(float elapsed)
{
    control(elapsed);

    for (size_t i = 0; i < objects.size(); i++)
    {
        objects[i]->tick(elapsed);
    }

    level->tick(elapsed);

    CollisionWorld::getInstance()->tick(elapsed);
}

void LogicWorld::update(float elapsed)
{
    camera->update(elapsed);

    for (size_t i = 0; i < objects.size(); i++)
    {
        objects[i]->update(elapsed);
    }

    level->update(elapsed);
}

void LogicWorld::trigger(CollisionObject* actor, CollisionObject* other)
{
    level->trigger(actor, other);
}
