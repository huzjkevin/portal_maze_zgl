#include "DynamicPortal.hh"

#include "LogicWorld.hh"

#include "../collision/CollisionObject.hh"
#include "../collision/CollisionWorld.hh"
#include "../collision/Gate.hh"
#include "../maze/Maze2D.hh"
#include "../render/Portal.hh"
#include "../render/QueryHandler.hh"
#include "../render/RenderWorld.hh"

DynamicPortal::DynamicPortal(SharedMaze2D sMaze, int32_t sWall, SharedMaze2D tMaze, int32_t tWall)
  : width(DEFAULT_PORTAL_WIDTH),
    height(DEFAULT_PORTAL_HEIGHT),
    guard(DEFAULT_GUARD),
    mainRenderEnabled(true),
    backRenderEnabled(true),
    mainCollisionEnabled(true),
    backCollisionEnabled(true),
    sourceMaze(sMaze),
    sourceWall(sWall),
    sourceHAnchor(WALL_CENTER),
    sourceHOffset(0.0f),
    sourceVAnchor(VERT_BOTTOM),
    sourceVOffset(0.01f),
    sourceZAnchor(Z_ANCHOR_INNER),
    sourceZOffset(0.0f),
    targetMaze(tMaze),
    targetWall(tWall),
    targetHAnchor(WALL_CENTER),
    targetHOffset(0.0f),
    targetVAnchor(VERT_BOTTOM),
    targetVOffset(0.0f),
    targetZAnchor(Z_ANCHOR_INNER),
    targetZOffset(0.0f),
    obstacles(),
    transformsLocked(false),
    mainSourceTransform(),
    mainTargetTransform(),
    backSourceTransform(),
    backTargetTransform(),
    mainRenderActive(false),
    backRenderActive(false),
    main(),
    back(),
    mainSourceCollision(),
    mainTargetCollision(),
    backSourceCollision(),
    backTargetCollision(),
    mainCollisionActive(false),
    backCollisionActive(false),
    mainGate(),
    backGate()
{
}

DynamicPortal::~DynamicPortal() {}

void DynamicPortal::addObstacle(CollisionObject* o)
{
    obstacles.push_back(o);
}

void DynamicPortal::addObstacles(CollisionObject* s, CollisionObject* t)
{
    obstacles.push_back(s);
    obstacles.push_back(t);
}

void DynamicPortal::enable(bool mainRender, bool backRender, bool mainCollision, bool backCollision)
{
    mainRenderEnabled = mainRender;
    backRenderEnabled = backRender;
    mainCollisionEnabled = mainCollision;
    backCollisionEnabled = backCollision;
    update();
}

void DynamicPortal::disable(bool mainRender, bool backRender, bool mainCollision, bool backCollision)
{
    mainRenderEnabled = mainRender;
    backRenderEnabled = backRender;
    mainCollisionEnabled = mainCollision;
    backCollisionEnabled = backCollision;
    update();
}

void DynamicPortal::lockTransforms()
{
    mainSourceTransform = sourceMaze->attachRectangle(sourceWall, width, height, PORTAL_SOURCE, sourceHAnchor, sourceHOffset, sourceVAnchor,
                                                      sourceVOffset, guard, NO_MIRROR, sourceZAnchor, sourceZOffset);
    mainTargetTransform = targetMaze->attachRectangle(targetWall, width, height, PORTAL_TARGET, targetHAnchor, targetHOffset, targetVAnchor,
                                                      targetVOffset, guard, NO_MIRROR, targetZAnchor, targetZOffset);
    backSourceTransform = targetMaze->attachRectangle(targetWall, width, height, PORTAL_SOURCE, targetHAnchor, targetHOffset, targetVAnchor,
                                                      targetVOffset, guard, NO_MIRROR, targetZAnchor, targetZOffset);
    backTargetTransform = sourceMaze->attachRectangle(sourceWall, width, height, PORTAL_TARGET, sourceHAnchor, sourceHOffset, sourceVAnchor,
                                                      sourceVOffset, guard, NO_MIRROR, sourceZAnchor, sourceZOffset);
    transformsLocked = true;
}

void DynamicPortal::update()
{
    updateMainRender();
    updateBackRender();
    updateMainCollision();
    updateBackCollision();
}

void DynamicPortal::updateMainRender()
{
    if (mainRenderEnabled && !mainRenderActive)
    {
        main = std::make_shared<Portal>(false, width, height);
        main->setSource(mainSourceTransform);
        main->setTarget(mainTargetTransform);

        RenderWorld::getInstance()->addPortal(main);
        mainRenderActive = true;

        updateQueryHandlers();
    }
    else if (mainRenderActive && !mainRenderEnabled)
    {
        RenderWorld::getInstance()->removePortal(main);
        mainRenderActive = false;
        main = nullptr;

        updateQueryHandlers();
    }
}

void DynamicPortal::updateBackRender()
{
    if (backRenderEnabled && !backRenderActive)
    {
        back = std::make_shared<Portal>(false, width, height);
        back->setSource(backSourceTransform);
        back->setTarget(backTargetTransform);

        RenderWorld::getInstance()->addPortal(back);
        backRenderActive = true;

        updateQueryHandlers();
    }
    else if (backRenderActive && !backRenderEnabled)
    {
        RenderWorld::getInstance()->removePortal(back);
        backRenderActive = false;
        back = nullptr;

        updateQueryHandlers();
    }
}

void DynamicPortal::updateMainCollision()
{
    if (mainCollisionEnabled && !mainCollisionActive)
    {
        mainSourceCollision = std::make_shared<CollisionObject>();
        mainSourceCollision->initTrigger(new btBoxShape(btVector3(width / 2.0f, height / 2.0f, PORTAL_TRIGGER_DEPTH)), mainSourceTransform, COLLISION_SOURCE);

        mainTargetCollision = std::make_shared<CollisionObject>();
        mainTargetCollision->initTrigger(new btBoxShape(btVector3(width / 2.0f, height / 2.0f, PORTAL_TRIGGER_DEPTH)), mainTargetTransform, COLLISION_TARGET);

        mainGate = std::make_shared<Gate>(mainSourceCollision.get(), mainTargetCollision.get());
        mainSourceCollision->setGate(mainGate.get());
        mainTargetCollision->setGate(mainGate.get());

        CollisionWorld::getInstance()->addObject(mainSourceCollision);
        CollisionWorld::getInstance()->addObject(mainTargetCollision);
        LogicWorld::getInstance()->getGates()->push_back(mainGate);

        for (size_t i = 0; i < obstacles.size(); i++)
        {
            mainGate->getObstacles()->push_back(obstacles[i]);
        }

        mainCollisionActive = true;

        updateQueryHandlers();
    }
    else if (mainCollisionActive && !mainCollisionEnabled)
    {
        CollisionWorld::getInstance()->removeObject(mainSourceCollision);
        CollisionWorld::getInstance()->removeObject(mainTargetCollision);

        std::vector<SharedGate>* gates = LogicWorld::getInstance()->getGates();
        auto begin = gates->begin();
        auto end = gates->end();
        auto resultMain = std::find(begin, end, mainGate);
        gates->erase(resultMain);

        mainCollisionActive = false;
    }
}

void DynamicPortal::updateBackCollision()
{
    if (backCollisionEnabled && !backCollisionActive)
    {
        backSourceCollision = std::make_shared<CollisionObject>();
        backSourceCollision->initTrigger(new btBoxShape(btVector3(width / 2.0f, height / 2.0f, PORTAL_TRIGGER_DEPTH)), backSourceTransform, COLLISION_SOURCE);

        backTargetCollision = std::make_shared<CollisionObject>();
        backTargetCollision->initTrigger(new btBoxShape(btVector3(width / 2.0f, height / 2.0f, PORTAL_TRIGGER_DEPTH)), backTargetTransform, COLLISION_TARGET);

        backGate = std::make_shared<Gate>(backSourceCollision.get(), backTargetCollision.get());
        backSourceCollision->setGate(backGate.get());
        backTargetCollision->setGate(backGate.get());

        CollisionWorld::getInstance()->addObject(backSourceCollision);
        CollisionWorld::getInstance()->addObject(backTargetCollision);
        LogicWorld::getInstance()->getGates()->push_back(backGate);

        for (size_t i = 0; i < obstacles.size(); i++)
        {
            backGate->getObstacles()->push_back(obstacles[i]);
        }

        backCollisionActive = true;

        updateQueryHandlers();
    }
    else if (backCollisionActive && !backCollisionEnabled)
    {
        CollisionWorld::getInstance()->removeObject(backSourceCollision);
        CollisionWorld::getInstance()->removeObject(backTargetCollision);

        std::vector<SharedGate>* gates = LogicWorld::getInstance()->getGates();
        auto begin = gates->begin();
        auto end = gates->end();
        auto resultBack = std::find(begin, end, backGate);
        gates->erase(resultBack);

        backCollisionActive = false;
    }
}

void DynamicPortal::updateQueryHandlers()
{
    int32_t mainIndex = mainRenderActive ? RenderWorld::getInstance()->indexOf(main) : (-1);
    int32_t backIndex = backRenderActive ? RenderWorld::getInstance()->indexOf(back) : (-1);

    if (mainCollisionActive)
    {
        mainGate->getQueryHandler()->setSource(mainIndex);
        mainGate->getQueryHandler()->setTarget(backIndex);
    }
    if (backCollisionActive)
    {
        backGate->getQueryHandler()->setSource(backIndex);
        backGate->getQueryHandler()->setTarget(mainIndex);
    }
}

void DynamicPortal::updateObstacles()
{
    if (mainCollisionActive)
    {
        mainGate->getObstacles()->clear();
        for (size_t i = 0; i < obstacles.size(); i++)
        {
            mainGate->getObstacles()->push_back(obstacles[i]);
        }
    }
    if (backCollisionActive)
    {
        backGate->getObstacles()->clear();
        for (size_t i = 0; i < obstacles.size(); i++)
        {
            backGate->getObstacles()->push_back(obstacles[i]);
        }
    }
}
