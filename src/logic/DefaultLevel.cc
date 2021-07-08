#include "DefaultLevel.hh"

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
#include "../render/Graph.hh"
#include "../render/Light.hh"
#include "../render/Portal.hh"
#include "../render/QueryHandler.hh"
#include "../render/RenderWorld.hh"
#include "../render/Renderer.hh"

#include "../Game.hh"
#include "../MeshHelper.hh"
#include "../load_mesh.hh"

#include <GLFW/glfw3.h>

DefaultLevel::DefaultLevel() : won(false), oldClick(false), mazes(), marks() {}

DefaultLevel::~DefaultLevel() {}

void DefaultLevel::tick(float elapsed)
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

void DefaultLevel::update(float elapsed)
{
    /*
    float currentTime = LogicWorld::getInstance()->getGame()->getCurrentTime();
    globalLight->setDirection(glm::vec3(glm::cos(currentTime), 0, glm::sin(currentTime)));
    */
    float currentTime = LogicWorld::getInstance()->getGame()->getCurrentTime();
    // return;
    float angle = currentTime / 2.5f * 3.14f;
    drawableKey->setTransform(glm::translate(glm::vec3(4.5f * 8.0f, 1.0f, -3.5f * 8.0f)) * glm::rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f)) *
                              // glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) //key 1
                              // glm::translate(glm::vec3(0.0f, -0.3f, 0.0f))                  //key 1
                              glm::rotate(glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) // key 2
    );
}

void DefaultLevel::trigger(CollisionObject* actor, CollisionObject* other)
{
    if (!won)
    {
        Renderer::getInstance()->setBackgroundColor(glm::vec3(0.0f, 1.0f, 0.0f));
        glow::info() << "Game won!";
        won = true;
    }
}

void DefaultLevel::init()
{
    globalLight = std::make_shared<GlobalLight>();
    globalLight->setAmbient(1.0f);
    globalLight->setIntensity(0.1f);
    RenderWorld::getInstance()->getGlobalLights()->push_back(globalLight);

    marks = std::make_shared<MazeMarkList>();
    RenderWorld::getInstance()->getRoot()->addChild(marks->getNode());

    initPlayer(0.3f, glm::vec3(4.5f * 8.0f, 0.3f, -4.5f * 8.0f), 0.1f);
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

    SharedMazeConverter2D converter = std::make_shared<MazeConverter2D>();
    // converter->setWorld(mGameWorld);

    std::vector<glow::SharedVertexArray> meshes;

    std::vector<CollisionObject*> bodies;
    converter->setBodies(&bodies);

    SharedMaze2D maze = std::make_shared<Maze2D>();
    mazes.push_back(maze);

    float tileW = 8.0f, tileH = 8.0f;

    for (int z = -5; z <= 0; z++)
    {
        for (int x = 5; x >= 0; x--)
        {
            maze->createVertex({x * tileW, z * tileH});
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
    maze->makeCorridor(4 * 0 + 2, true, true);
    maze->makeCorridor(4 * 5 + 2, true, true);
    maze->makeCorridor(4 * 10 + 1, true, true);
    maze->makeCorridor(4 * 11 + 0, true, true);
    maze->makeCorridor(4 * 11 + 2, true, true);
    maze->makeCorridor(4 * 6 + 0, true, true);
    maze->makeCorridor(4 * 1 + 1, true, true);
    maze->makeCorridor(4 * 2 + 1, true, true);
    maze->makeCorridor(4 * 3 + 1, true, true);
    maze->makeCorridor(4 * 21 + 3, true, true);
    maze->makeCorridor(4 * 23 + 3, true, true);
    maze->makeCorridor(4 * 22 + 0, true, true);
    maze->makeCorridor(4 * 17 + 1, true, true);
    maze->makeCorridor(4 * 18 + 1, true, true);
    maze->makeCorridor(4 * 19 + 0, true, true);

    maze->setCorridorChooser(door);
    maze->makeCorridor(4 * 9 + 0, true, true);
    maze->makeCorridor(4 * 8 + 3, true, true);
    maze->makeCorridor(4 * 7 + 2, true, true);
    maze->makeCorridor(4 * 12 + 1, true, true);
    maze->makeCorridor(4 * 13 + 0, true, true);

    maze->setCorridorChooser(left);
    maze->makeCorridor(4 * 15 + 2, true, true);
    maze->makeCorridor(4 * 21 + 0, true, true);
    maze->makeCorridor(4 * 24 + 0, true, true);

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

    CollisionObject* source1Body = bodies[4 * 15 + 0];
    CollisionObject* target1Body = bodies[4 * 8 + 1];
    createPortalPair(maze, 4 * 15 + 0, source1Body, maze, 4 * 8 + 1, target1Body);

    CollisionObject* source2Body = bodies[4 * 13 + 1];
    CollisionObject* target2Body = bodies[4 * 23 + 2];
    createPortalPair(maze, 4 * 13 + 1, source2Body, maze, 4 * 23 + 2, target2Body);

    createMirror(maze, 4 * 9 + 1);
    createMirror(maze, 4 * 9 + 3);

    createMirror(maze, 4 * 7 + 3, DEFAULT_PORTAL_WIDTH, DEFAULT_PORTAL_HEIGHT, WALL_LEFT, -0.2f);
    createMirror(maze, 4 * 12 + 3, DEFAULT_PORTAL_WIDTH, DEFAULT_PORTAL_HEIGHT, WALL_RIGHT, 0.2f);

    createMirror(maze, 4 * 14 + 0);

    CollisionObject* source3Body = bodies[4 * 1 + 0];

    for (size_t i = 0; i < maze->getFaceCount(); i++)
    {
        glm::vec3 lightPos = maze->getCenter(i);
        lightPos.y += 2.0f;

        SharedPointLight light = std::make_shared<PointLight>(lightPos);
        light->setIntensity(3.0f);
        light->setAttenuation(15.0f);
        light->initObjects(64, 32);

        RenderWorld::getInstance()->getPointLights()->push_back(light);
    }

    SharedMaze2D hall = std::make_shared<Maze2D>();
    mazes.push_back(hall);

    hall->createVertex({-2 * tileW, -1.75f * tileH});
    hall->createVertex({-3 * tileW, -2 * tileH});
    hall->createVertex({-4 * tileW, -2.01 * tileH});
    hall->createVertex({-5 * tileW, -2 * tileH});
    hall->createVertex({-6 * tileW, -1.75f * tileH});
    hall->createVertex({-7 * tileW, -1 * tileH});
    hall->createVertex({-5 * tileW, 1 * tileH});
    hall->createVertex({-3 * tileW, 1 * tileH});
    hall->createVertex({-1 * tileW, -1 * tileH});
    hall->createVertex({-3 * tileW, -3 * tileH});
    hall->createVertex({-4 * tileW, -3 * tileH});
    hall->createVertex({-5 * tileW, -3 * tileH});

    param.clear();
    param.push_back(0);
    param.push_back(1);
    param.push_back(2);
    param.push_back(3);
    param.push_back(4);
    param.push_back(5);
    param.push_back(6);
    param.push_back(7);
    param.push_back(8);
    hall->createFace(param);

    param.clear();
    param.push_back(9);
    param.push_back(10);
    param.push_back(2);
    param.push_back(1);
    hall->createFace(param);

    param.clear();
    param.push_back(10);
    param.push_back(11);
    param.push_back(3);
    param.push_back(2);
    hall->createFace(param);

    hall->connect();

    hall->setCorridorChooser(door);
    hall->makeCorridor(1, true, true);
    hall->makeCorridor(2, true, true);
    hall->makeCorridor(16, true, true);

    SharedDrawable hallDrawable = std::make_shared<Drawable>();
    glow::SharedTexture2D milanopavementAlbedo = glow::Texture2D::createFromFile("../data/textures/milanopavement.albedo.png", glow::ColorSpace::sRGB);
    glow::SharedTexture2D milanopavementNormal = glow::Texture2D::createFromFile("../data/textures/milanopavement.normal.png", glow::ColorSpace::Linear);
    hallDrawable->setAlbedo(milanopavementAlbedo);
    hallDrawable->setNormal(milanopavementNormal);
    RenderWorld::getInstance()->getRoot()->addChild(hallDrawable);

    SharedDrawable hallDrawableFloor = std::make_shared<Drawable>();
    hallDrawableFloor->setAlbedo(milanopavementAlbedo);
    hallDrawableFloor->setNormal(milanopavementNormal);
    RenderWorld::getInstance()->getRoot()->addChild(hallDrawableFloor);

    SharedDrawable hallDrawableCeil = std::make_shared<Drawable>();
    hallDrawableCeil->setAlbedo(milanopavementAlbedo);
    hallDrawableCeil->setNormal(milanopavementNormal);
    RenderWorld::getInstance()->getRoot()->addChild(hallDrawableCeil);

    converter->setMeshes(&meshes);
    converter->convert(hall);

    hallDrawable->setMesh(meshes[0]);
    hallDrawableFloor->setMesh(meshes[1]);
    hallDrawableCeil->setMesh(meshes[2]);
    meshes.clear();

    CollisionObject* target3Body = bodies[6];

    createPortalPair(maze, 4 * 1 + 0, source3Body, hall, 6, target3Body);

    CollisionObject* source4Body = bodies[16];
    CollisionObject* target5Body = bodies[10];

    for (size_t i = 0; i < hall->getFaceCount(); i++)
    {
        glm::vec3 lightPos = hall->getCenter(i);
        lightPos.y += 2.0f;

        SharedPointLight light = std::make_shared<PointLight>(lightPos);
        if (i == 0)
        {
            light->setIntensity(5.0f);
            light->setAttenuation(30.0f);
        }
        else
        {
            light->setIntensity(3.0f);
            light->setAttenuation(15.0f);
        }
        light->initObjects(64, 32);

        RenderWorld::getInstance()->getPointLights()->push_back(light);
    }

    SharedMaze2D trick = std::make_shared<Maze2D>();
    mazes.push_back(trick);

    trick->createVertex({-3 * tileW, -10 * tileH});
    trick->createVertex({-3 * tileW, -11 * tileH});
    trick->createVertex({-3 * tileW, -12 * tileH});
    trick->createVertex({-4 * tileW, -12 * tileH});
    trick->createVertex({-4 * tileW, -11 * tileH});
    trick->createVertex({-4 * tileW, -10 * tileH});

    param.clear();
    param.push_back(0);
    param.push_back(1);
    param.push_back(4);
    param.push_back(5);
    trick->createFace(param);

    param.clear();
    param.push_back(1);
    param.push_back(2);
    param.push_back(3);
    param.push_back(4);
    trick->createFace(param);

    trick->connect();

    trick->setCorridorChooser(door);
    trick->makeCorridor(1, true, true);
    trick->makeCorridor(6, false);
    trick->makeCorridor(2, false);

    SharedDrawable trickDrawable = std::make_shared<Drawable>();
    trickDrawable->setAlbedo(milanopavementAlbedo);
    trickDrawable->setNormal(milanopavementNormal);
    RenderWorld::getInstance()->getRoot()->addChild(trickDrawable);

    SharedDrawable trickDrawableFloor = std::make_shared<Drawable>();
    trickDrawableFloor->setAlbedo(milanopavementAlbedo);
    trickDrawableFloor->setNormal(milanopavementNormal);
    RenderWorld::getInstance()->getRoot()->addChild(trickDrawableFloor);

    SharedDrawable trickDrawableCeil = std::make_shared<Drawable>();
    trickDrawableCeil->setAlbedo(milanopavementAlbedo);
    trickDrawableCeil->setNormal(milanopavementNormal);
    RenderWorld::getInstance()->getRoot()->addChild(trickDrawableCeil);

    converter->setMeshes(&meshes);
    converter->convert(trick);

    trickDrawable->setMesh(meshes[0]);
    trickDrawableFloor->setMesh(meshes[1]);
    trickDrawableCeil->setMesh(meshes[2]);
    meshes.clear();

    CollisionObject* target4Body = bodies[6];
    CollisionObject* source5Body = bodies[2];

    createPortalPair(hall, 16, nullptr, trick, 6, nullptr, DEFAULT_WIDTH, DEFAULT_HEIGHT + 0.01f, 0, 0.0f, 0, 0.0f, 0, 0.0f, 0, 0.0f, 0.01f, -1, 0.0f, -1, 0.0f);
    createPortalPair(trick, 2, nullptr, hall, 10, nullptr, DEFAULT_WIDTH, DEFAULT_HEIGHT + 0.01f, 0, 0.0f, 0, 0.0f, 0, 0.0f, 0, 0.0f, 0.01f, -1, 0.0f, -1, 0.0f);

    for (size_t i = 0; i < trick->getFaceCount(); i++)
    {
        glm::vec3 lightPos = trick->getCenter(i);
        lightPos.y += 2.0f;

        SharedPointLight light = std::make_shared<PointLight>(lightPos);
        light->setIntensity(3.0f);
        light->setAttenuation(15.0f);
        light->initObjects(64, 32);

        RenderWorld::getInstance()->getPointLights()->push_back(light);
    }

    drawableKey = std::make_shared<Drawable>();
    glow::SharedTexture2D keyAlbedo = glow::Texture2D::createFromFile("../data/textures/key2.albedo.jpg", glow::ColorSpace::sRGB);
    glow::SharedTexture2D keyNormal = glow::Texture2D::createFromFile("../data/textures/key2.normal.jpg", glow::ColorSpace::Linear);
    drawableKey->setTransform(glm::translate(glm::vec3(4.5f * 8.0f, 1.0f, -3.5f * 8.0f)) *
                              // glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)) //key 1
                              glm::rotate(glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f)) // key 2
    );
    drawableKey->setAlbedo(keyAlbedo);
    drawableKey->setNormal(keyNormal);
    drawableKey->setMesh(load_mesh_from_obj("../data/meshes/key2.obj"));
    RenderWorld::getInstance()->getRoot()->addChild(drawableKey);

    SharedDrawable drawableFence = std::make_shared<Drawable>();
    glow::SharedTexture2D fenceAlbedo = glow::Texture2D::createFromFile("../data/textures/fence.albedo.jpg", glow::ColorSpace::sRGB);
    glow::SharedTexture2D fenceNormal = glow::Texture2D::createFromFile("../data/textures/fence.normal.jpg", glow::ColorSpace::Linear);
    drawableFence->setTransform(glm::translate(glm::vec3(4.5f * 8.0f, 0.0f, -3.0f * 8.0f)));
    drawableFence->setAlbedo(fenceAlbedo);
    drawableFence->setNormal(fenceNormal);
    drawableFence->setMesh(load_mesh_from_obj("../data/meshes/fence.obj"));
    RenderWorld::getInstance()->getRoot()->addChild(drawableFence);
}
