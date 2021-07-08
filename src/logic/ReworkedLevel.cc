#include "ReworkedLevel.hh"

#include "CameraObject.hh"
#include "LogicWorld.hh"
#include "PlayerObject.hh"

#include "../collision/CollisionObject.hh"
#include "../collision/CollisionWorld.hh"
#include "../collision/Gate.hh"
#include "../maze/Maze2D.hh"
#include "../maze/MazeConverter2D.hh"
#include "../maze/MazeGenerator2D.hh"
#include "../maze/MazeMark.hh"
#include "../maze/WallState.hh"
#include "../render/Drawable.hh"
#include "../render/Light.hh"
#include "../render/Portal.hh"
#include "../render/QueryHandler.hh"
#include "../render/RenderWorld.hh"
#include "../render/Renderer.hh"

#include "../Game.hh"
#include "../load_mesh.hh"
#include "../MeshHelper.hh"

#include <GLFW/glfw3.h>
#include <glow-extras/geometry/UVSphere.hh>

ReworkedLevel::ReworkedLevel() : won(false), oldClick(false), mazes(), marks() {}

ReworkedLevel::~ReworkedLevel() {}

void ReworkedLevel::tick(float elapsed)
{
    bool newClick = LogicWorld::getInstance()->getGame()->isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);

    if (!oldClick && newClick)
    {
        glm::vec3 loc3d = LogicWorld::getInstance()->getCamera()->getOwner()->getPosition();
        glm::vec2 location(loc3d.x, loc3d.z);

        int face = NO_FACE;
        bool hit = false;

        SharedMaze2D maze;

        for (uint32_t i = 0; i < mazes.size(); i++)
        {
            face = mazes[i]->findFace(location);
            if (face >= 0)
            {
                hit = true;
                maze = mazes[i];
                break;
            }
        }

        if (hit)
        {
            marks->flip(maze, face);
        }
    }

    oldClick = newClick;
}

void ReworkedLevel::update(float elapsed)
{
    /*
    float currentTime = LogicWorld::getInstance()->getGame()->getCurrentTime();
    globalLight->setDirection(glm::vec3(glm::cos(currentTime), 0, glm::sin(currentTime)));
    */
}

void ReworkedLevel::trigger(CollisionObject* actor, CollisionObject* other)
{
    if (!won)
    {
        Renderer::getInstance()->setBackgroundColor(glm::vec3(0.0f, 1.0f, 0.0f));
        glow::info() << "Game won!";
        won = true;
    }
}

void ReworkedLevel::initSpherePlayer(float scale, glm::vec3 position, float cameraDistance)
{
    SharedCollisionObject collision = std::make_shared<CollisionObject>();
    collision->initDynamic(new btSphereShape(scale), glm::translate(position), 1.0f, 20.0f, 0.9f, 0.0f, 0.0f);

    glow::SharedTexture2D cubeAlbedo = glow::Texture2D::createFromFile("../data/textures/cube.albedo.png", glow::ColorSpace::sRGB);
    glow::SharedTexture2D cubeNormal = glow::Texture2D::createFromFile("../data/textures/cube.normal.png", glow::ColorSpace::Linear);

    SharedPlayerObject player = std::make_shared<PlayerObject>(collision);
    player->setMesh(glow::geometry::UVSphere<>(glow::geometry::UVSphere<>::attributesOf(nullptr), 64, 32, scale).generate());
    player->setAlbedo(cubeAlbedo);
    player->setNormal(cubeNormal);

    RenderWorld::getInstance()->getRoot()->addChild(player->getNode());
    CollisionWorld::getInstance()->addObject(collision);

    LogicWorld::getInstance()->setPlayer(player);
    LogicWorld::getInstance()->getObjects()->push_back(player);

    SharedCameraObject camera = std::make_shared<CameraObject>();
    camera->setOwner(collision);
    camera->setDistance(cameraDistance);
    camera->setFoV(80.0f);

    camera->setOffset(glm::vec3(0.0f, 0.8f, 0.0f));

    collision->setCamera(camera.get());
    LogicWorld::getInstance()->setCamera(camera);
}

void ReworkedLevel::init()
{
    float tileW = 8.0f, tileH = 8.0f;

    globalLight = initGlobalLight(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.1f);

    marks = std::make_shared<MazeMarkList>();
    RenderWorld::getInstance()->getRoot()->addChild(marks->getNode());

    initSpherePlayer(0.3f, glm::vec3(4.5f * tileW, 0.3f, -4.5f * tileH + 3 * tileH), 0.1f);
    initGround();

    SharedWallStateChooser random = std::make_shared<WallStateChooser>();
    random->add(0.3f, std::make_shared<DoorWallState>());
    random->add(0.1f, std::make_shared<DoorWallState>(WALL_LEFT, DEFAULT_WIDTH, -0.2f));
    random->add(0.1f, std::make_shared<DoorWallState>(WALL_RIGHT, DEFAULT_WIDTH, 0.2f));

    SharedWallStateChooser undetermined = std::make_shared<WallStateChooser>(std::make_shared<WallState>(WALL_UNDETERMINED));
    SharedWallStateChooser solid = std::make_shared<WallStateChooser>(std::make_shared<WallState>(WALL_SOLID));
    SharedWallStateChooser none = std::make_shared<WallStateChooser>(std::make_shared<WallState>(WALL_NONE));
    SharedWallStateChooser door = std::make_shared<WallStateChooser>(std::make_shared<DoorWallState>());
    SharedWallStateChooser left = std::make_shared<WallStateChooser>(std::make_shared<DoorWallState>(WALL_LEFT, DEFAULT_WIDTH, -0.2f));
    SharedWallStateChooser right = std::make_shared<WallStateChooser>(std::make_shared<DoorWallState>(WALL_RIGHT, DEFAULT_WIDTH, 0.2f));

    std::vector<int32_t> param;
    std::vector<glow::SharedVertexArray> meshes;
    std::vector<CollisionObject*> bodies;

    SharedMazeConverter2D converter = std::make_shared<MazeConverter2D>();
    converter->setBodies(&bodies);

    SharedMaze2D maze = std::make_shared<Maze2D>();
    mazes.push_back(maze);

    for (int z = -5; z <= 0; z++)
    {
        for (int x = 5; x >= 0; x--)
        {
            maze->createVertex({x * tileW, z * tileH + 3 * tileH});
        }
    }

    for (int z = 0; z < 5; z++)
    {
        for (int x = 0; x < 5; x++)
        {
            param.clear();
            param.push_back((x + 0) + (z + 0) * 6);
            param.push_back((x + 1) + (z + 0) * 6);
            param.push_back((x + 1) + (z + 1) * 6);
            param.push_back((x + 0) + (z + 1) * 6);
            maze->createFace(param);
        }
    }

    maze->connect();

    maze->setCorridorChooser(none);
    maze->makeCorridor(4 * 5 + 2, true, true);
    maze->makeCorridor(4 * 10 + 2, true, true);
    maze->makeCorridor(4 * 15 + 1, true, true);
    maze->makeCorridor(4 * 1 + 2, true, true);
    maze->makeCorridor(4 * 1 + 1, true, true);
    maze->makeCorridor(4 * 11 + 2, true, true);
    maze->makeCorridor(4 * 11 + 1, true, true);
    maze->makeCorridor(4 * 16 + 1, true, true);
    maze->makeCorridor(4 * 21 + 1, true, true);
    maze->makeCorridor(4 * 12 + 2, true, true);
    maze->makeCorridor(4 * 2 + 1, true, true);
    maze->makeCorridor(4 * 7 + 1, true, true);
    maze->makeCorridor(4 * 8 + 2, true, true);
    maze->makeCorridor(4 * 18 + 2, true, true);
    maze->makeCorridor(4 * 3 + 1, true, true);
    maze->makeCorridor(4 * 18 + 1, true, true);
    maze->makeCorridor(4 * 14 + 2, true, true);

    maze->setCorridorChooser(door);
    maze->makeCorridor(4 * 0 + 2, true, true);
    maze->makeCorridor(4 * 15 + 2, true, true);
    maze->makeCorridor(4 * 22 + 1, true, true);
    maze->makeCorridor(4 * 0 + 2, true, true);
    maze->makeCorridor(4 * 8 + 1, true, true);
    maze->makeCorridor(4 * 19 + 2, true, true);

    maze->setCorridorChooser(left);
    maze->makeCorridor(4 * 6 + 1, true, true);
    maze->makeCorridor(4 * 13 + 2, true, true);

    maze->setCorridorChooser(door);
    maze->makeCorridor(4 * 24 + 2, false);

    SharedCollisionObject trigger = std::make_shared<CollisionObject>();
    trigger->initTrigger(new btBoxShape(btVector3(DEFAULT_WIDTH / 2.0f, DEFAULT_HEIGHT / 2.0f, PORTAL_TRIGGER_DEPTH)),
                         maze->attachRectangle(4 * 24 + 2, DEFAULT_WIDTH, DEFAULT_HEIGHT, PORTAL_SOURCE), COLLISION_TRIGGER);

    CollisionWorld::getInstance()->addObject(trigger);

    SharedDrawable drawable = std::make_shared<Drawable>();
    glow::SharedTexture2D flatbrickAlbedo = glow::Texture2D::createFromFile("../data/textures/flatbrick.albedo.png", glow::ColorSpace::sRGB);
    glow::SharedTexture2D flatbrickNormal = glow::Texture2D::createFromFile("../data/textures/flatbrick.normal.png", glow::ColorSpace::Linear);
    drawable->setAlbedo(flatbrickAlbedo);
    drawable->setNormal(flatbrickNormal);
    RenderWorld::getInstance()->getRoot()->addChild(drawable);

    SharedDrawable drawableFloor = std::make_shared<Drawable>();
    drawableFloor->setAlbedo(flatbrickAlbedo);
    drawableFloor->setNormal(flatbrickNormal);
    RenderWorld::getInstance()->getRoot()->addChild(drawableFloor);

    SharedDrawable drawableCeil = std::make_shared<Drawable>();
    drawableCeil->setAlbedo(flatbrickAlbedo);
    drawableCeil->setNormal(flatbrickNormal);
    RenderWorld::getInstance()->getRoot()->addChild(drawableCeil);

    converter->setMeshes(&meshes);
    converter->convert(maze);

    drawable->setMesh(meshes[0]);
    drawableFloor->setMesh(meshes[1]);
    drawableCeil->setMesh(meshes[2]);
    meshes.clear();

    CollisionObject* source1Body = bodies[4 * 0 + 3];
    CollisionObject* target1Body = bodies[4 * 24 + 1];

    CollisionObject* source2Body = bodies[4 * 20 + 2];
    CollisionObject* target2Body = bodies[4 * 24 + 3];

    CollisionObject* source3Body = bodies[4 * 11 + 0];

    CollisionObject* source4Body = bodies[4 * 12 + 1];
    CollisionObject* target4Body = bodies[4 * 4 + 1];

    CollisionObject* target5Body = bodies[4 * 21 + 2];

    createMirror(maze, 4 * 6 + 2);
    createMirror(maze, 4 * 9 + 0);
    createMirror(maze, 4 * 9 + 2);
    createMirror(maze, 4 * 14 + 1);

    float triW = 16.0f;
    float triH = 16.0f;

    SharedMaze2D triMaze = std::make_shared<Maze2D>();
    // mazes.push_back(triMaze);

    for (int x = 5; x >= 0; x--)
    {
        triMaze->createVertex({x * triW, -6 * tileH});
    }

    for (int x = 5; x >= 1; x--)
    {
        triMaze->createVertex({x * triW - triW * 0.5f, -6 * tileH - glm::sqrt(triH * triH - (triW / 2.0f) * (triW / 2.0f))});
    }

    for (int x = 0; x < 5; x++)
    {
        param.clear();
        param.push_back(x);
        param.push_back(x + 6);
        param.push_back(x + 1);
        triMaze->createFace(param);
    }

    for (int x = 0; x < 4; x++)
    {
        param.clear();
        param.push_back(x + 6);
        param.push_back(x + 7);
        param.push_back(x + 1);
        triMaze->createFace(param);
    }

    triMaze->connect();

    triMaze->setCorridorChooser(left);
    triMaze->makeCorridor(4 * 3 + 0, true, true);
    triMaze->makeCorridor(3 * 3 + 1, true, true);
    triMaze->makeCorridor(3 * 3 + 0, true, true);
    triMaze->makeCorridor(2 * 3 + 1, true, true);
    triMaze->makeCorridor(2 * 3 + 0, true, true);
    triMaze->makeCorridor(1 * 3 + 1, true, true);
    triMaze->makeCorridor(1 * 3 + 0, true, true);
    triMaze->makeCorridor(0 * 3 + 1, true, true);

    SharedDrawable triDrawable = std::make_shared<Drawable>();
    glow::SharedTexture2D milanopavementAlbedo = glow::Texture2D::createFromFile("../data/textures/milanopavement.albedo.png", glow::ColorSpace::sRGB);
    glow::SharedTexture2D milanopavementNormal = glow::Texture2D::createFromFile("../data/textures/milanopavement.normal.png", glow::ColorSpace::Linear);
    triDrawable->setAlbedo(milanopavementAlbedo);
    triDrawable->setNormal(milanopavementNormal);
    RenderWorld::getInstance()->getRoot()->addChild(triDrawable);

    SharedDrawable triDrawableFloor = std::make_shared<Drawable>();
    triDrawableFloor->setAlbedo(milanopavementAlbedo);
    triDrawableFloor->setNormal(milanopavementNormal);
    RenderWorld::getInstance()->getRoot()->addChild(triDrawableFloor);

    SharedDrawable triDrawableCeil = std::make_shared<Drawable>();
    triDrawableCeil->setAlbedo(milanopavementAlbedo);
    triDrawableCeil->setNormal(milanopavementNormal);
    RenderWorld::getInstance()->getRoot()->addChild(triDrawableCeil);

    converter->setMeshes(&meshes);
    converter->convert(triMaze);

    triDrawable->setMesh(meshes[0]);
    triDrawableFloor->setMesh(meshes[1]);
    triDrawableCeil->setMesh(meshes[2]);
    meshes.clear();

    CollisionObject* target3Body = bodies[3 * 4 + 2];
    CollisionObject* source5Body = bodies[3 * 0 + 2];

    createMirror(triMaze, 2 * 3 + 2);

    // MAZES DONE

    // createPortalPair(maze, 4 * 0 + 3, source1Body, maze, 4 * 24 + 1, target1Body);
    createPartialPair(true, false, false, false, maze, 4 * 0 + 3, source1Body, maze, 4 * 24 + 1, target1Body);
    // createPortalPair(maze, 4 * 20 + 2, source2Body, maze, 4 * 24 + 3, target2Body);
    createPartialPair(false, false, true, true, maze, 4 * 20 + 2, source2Body, maze, 4 * 24 + 3, target2Body);
    // createPortalPair(maze, 4 * 11 + 0, source3Body, triMaze, 3 * 4 + 2, target3Body);
    createPartialPair(true, true, true, false, maze, 4 * 11 + 0, source3Body, triMaze, 3 * 4 + 2, target3Body);
    createPortalPair(maze, 4 * 12 + 1, source4Body, maze, 4 * 4 + 1, target4Body);
    // createPortalPair(triMaze, 3 * 0 + 2, source5Body, maze, 4 * 21 + 3, target5Body);
    createPartialPair(true, false, true, false, triMaze, 3 * 0 + 2, source5Body, maze, 4 * 21 + 3, target5Body);

    // LIGHTING

    for (size_t i = 0; i < maze->getFaceCount(); i++)
    {
        glm::vec3 lightPos = maze->getCenter(i);
        lightPos.y += 2.0f;

        SharedPointLight light = std::make_shared<PointLight>(lightPos);
        light->setIntensity(3.0f);
        light->setAttenuation(15.0f);
        light->initObjects(64, 32);

        if (i == 20)
        {
            light->setColor(glm::vec3(0.0f, 0.0f, 1.0f));
        }

        if (i == 11 || i == 12 || i == 16 || i == 17)
        {
            light->setColor(glm::vec3(1.0f, 0.0f, 0.0f));
        }

        if (i != 2 && i != 3 && i != 5 && i != 15)
        {
            RenderWorld::getInstance()->getPointLights()->push_back(light);
        }
    }

    for (size_t i = 0; i < triMaze->getFaceCount(); i++)
    {
        glm::vec3 lightPos = triMaze->getCenter(i);
        lightPos.y += 2.0f;

        SharedPointLight light = std::make_shared<PointLight>(lightPos);
        light->setIntensity(6.0f);
        light->setAttenuation(tileW);
        light->initObjects(64, 32);

        RenderWorld::getInstance()->getPointLights()->push_back(light);
    }
}

void ReworkedLevel::createPartialPair(bool visMain,
                                      bool visBack,
                                      bool colMain,
                                      bool colBack,
                                      SharedMaze2D sourceMaze,
                                      int32_t sourceWall,
                                      CollisionObject* sourceBody,
                                      SharedMaze2D targetMaze,
                                      int32_t targetWall,
                                      CollisionObject* targetBody,
                                      float width,
                                      float height,
                                      int sourceHAnchor,
                                      float sourceHOffset,
                                      int targetHAnchor,
                                      float targetHOffset,
                                      int sourceVAnchor,
                                      float sourceVOffset,
                                      int targetVAnchor,
                                      float targetVOffset,
                                      float guard,
                                      int sourceZAnchor,
                                      float sourceZOffset,
                                      int targetZAnchor,
                                      float targetZOffset)
{
    SharedPortal main = std::make_shared<Portal>(false, width, height);
    main->setSource(sourceMaze->attachRectangle(sourceWall, width, height, PORTAL_SOURCE, sourceHAnchor, sourceHOffset, sourceVAnchor, sourceVOffset,
                                                guard, NO_MIRROR, sourceZAnchor, sourceZOffset));
    main->setTarget(targetMaze->attachRectangle(targetWall, width, height, PORTAL_TARGET, targetHAnchor, targetHOffset, targetVAnchor, targetVOffset,
                                                guard, NO_MIRROR, targetZAnchor, targetZOffset));

    if (visMain)
    {
        RenderWorld::getInstance()->addPortal(main);
    }

    SharedPortal back = std::make_shared<Portal>(false, width, height);
    back->setSource(targetMaze->attachRectangle(targetWall, width, height, PORTAL_SOURCE, targetHAnchor, targetHOffset, targetVAnchor, targetVOffset,
                                                guard, NO_MIRROR, targetZAnchor, targetZOffset));
    back->setTarget(sourceMaze->attachRectangle(sourceWall, width, height, PORTAL_TARGET, sourceHAnchor, sourceHOffset, sourceVAnchor, sourceVOffset,
                                                guard, NO_MIRROR, sourceZAnchor, sourceZOffset));

    if (visBack)
    {
        RenderWorld::getInstance()->addPortal(back);
    }

    int mainIndex = visMain ? RenderWorld::getInstance()->indexOf(main) : -1;
    int backIndex = visBack ? RenderWorld::getInstance()->indexOf(back) : -1;

    if (colMain)
    {
        SharedCollisionObject mainSourceCollision = std::make_shared<CollisionObject>();
        mainSourceCollision->initTrigger(new btBoxShape(btVector3(width / 2.0f, height / 2.0f, PORTAL_TRIGGER_DEPTH)), main->getSource(), COLLISION_SOURCE);

        SharedCollisionObject mainTargetCollision = std::make_shared<CollisionObject>();
        mainTargetCollision->initTrigger(new btBoxShape(btVector3(width / 2.0f, height / 2.0f, PORTAL_TRIGGER_DEPTH)), main->getTarget(), COLLISION_TARGET);

        SharedGate mainGate = std::make_shared<Gate>(mainSourceCollision.get(), mainTargetCollision.get());
        mainSourceCollision->setGate(mainGate.get());
        mainTargetCollision->setGate(mainGate.get());

        CollisionWorld::getInstance()->addObject(mainSourceCollision);
        CollisionWorld::getInstance()->addObject(mainTargetCollision);
        LogicWorld::getInstance()->getGates()->push_back(mainGate);

        if (sourceBody != nullptr)
        {
            mainGate->getObstacles()->push_back(sourceBody);
        }

        if (targetBody != nullptr)
        {
            mainGate->getObstacles()->push_back(targetBody);
        }

        mainGate->getQueryHandler()->setup(mainIndex, backIndex);
    }

    if (colBack)
    {
        SharedCollisionObject backSourceCollision = std::make_shared<CollisionObject>();
        backSourceCollision->initTrigger(new btBoxShape(btVector3(width / 2.0f, height / 2.0f, PORTAL_TRIGGER_DEPTH)), back->getSource(), COLLISION_SOURCE);

        SharedCollisionObject backTargetCollision = std::make_shared<CollisionObject>();
        backTargetCollision->initTrigger(new btBoxShape(btVector3(width / 2.0f, height / 2.0f, PORTAL_TRIGGER_DEPTH)), back->getTarget(), COLLISION_TARGET);

        SharedGate backGate = std::make_shared<Gate>(backSourceCollision.get(), backTargetCollision.get());
        backSourceCollision->setGate(backGate.get());
        backTargetCollision->setGate(backGate.get());

        CollisionWorld::getInstance()->addObject(backSourceCollision);
        CollisionWorld::getInstance()->addObject(backTargetCollision);
        LogicWorld::getInstance()->getGates()->push_back(backGate);

        if (sourceBody != nullptr)
        {
            backGate->getObstacles()->push_back(sourceBody);
        }

        if (targetBody != nullptr)
        {
            backGate->getObstacles()->push_back(targetBody);
        }

        backGate->getQueryHandler()->setup(backIndex, mainIndex);
    }
}
