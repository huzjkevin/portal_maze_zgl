#include "FinalLevel.hh"

#include "CameraObject.hh"
#include "DynamicPortal.hh"
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
#include "../MeshHelper.hh"
#include "../load_mesh.hh"

#include <GLFW/glfw3.h>
#include <glow-extras/geometry/UVSphere.hh>

FinalLevel::FinalLevel() : tileW(8.0f), tileH(8.0f), won(false), key1Picked(false), key2Picked(false), oldClick(false), mazes(), marks() {}

FinalLevel::~FinalLevel() {}

void FinalLevel::tick(float elapsed)
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

void FinalLevel::update(float elapsed)
{
    float currentTime = LogicWorld::getInstance()->getGame()->getCurrentTime();
    float angle = currentTime / 2.5f * 3.14f;
    key1Object->setTransform(glm::translate(glm::vec3(2.5f * tileW, 1.0f, -0.5f * tileH)) * glm::rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f))
                             * glm::rotate(glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    key2Object->setTransform(glm::translate(glm::vec3(-2.5f * tileW, 1.0f, 2.5f * tileH)) * glm::rotate(angle, glm::vec3(0.0f, 1.0f, 0.0f))
                             * glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
}

void FinalLevel::trigger(CollisionObject* actor, CollisionObject* other)
{
    if (other == exitTrigger.get())
    {
        if (!won)
        {
            glow::info() << "Game won!";
            won = true;
        }
    }
    if (other == key1Trigger.get())
    {
        if (!key1Picked)
        {
            portal5->enable();
            RenderWorld::getInstance()->getRoot()->removeChild(key1Object);
            key1Picked = true;
        }
    }
    if (other == key2Trigger.get())
    {
        if (!key2Picked)
        {
            for (size_t i = 0; i < 10; i++)
            {
                if (i != 0 && i != 1 && i != 5 && i != 6)
                {
                    if (marks->isMarked(mazes[2], i))
                    {
                        marks->flip(mazes[2], i);
                    }
                }
            }

            portal9->disable();
            portal10->enable();

            portal6->enable();
            RenderWorld::getInstance()->getRoot()->removeChild(key2Object);
            key2Picked = true;
        }
    }
}

void FinalLevel::init()
{
    globalLight = initGlobalLight(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.1f);

    marks = std::make_shared<MazeMarkList>();
    RenderWorld::getInstance()->getRoot()->addChild(marks->getNode());

    initPlayer(0.3f, glm::vec3(6.5f * tileW, 0.3f, -2.5f * tileH), 0.1f);
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

    // CREATE MAIN MAZE

    SharedMaze2D maze = std::make_shared<Maze2D>();
    mazes.push_back(maze);

    for (int z = -5; z <= 0; z++)
    {
        for (int x = 7; x >= 0; x--)
        {
            maze->createVertex({x * tileW, z * tileH});
        }
    }

    for (int z = 0; z < 5; z++)
    {
        for (int x = 0; x < 7; x++)
        {
            param.clear();
            param.push_back((x + 0) + (z + 0) * 8);
            param.push_back((x + 1) + (z + 0) * 8);
            param.push_back((x + 1) + (z + 1) * 8);
            param.push_back((x + 0) + (z + 1) * 8);
            maze->createFace(param);
        }
    }

    maze->connect();

    maze->setCorridorChooser(none);
    maze->makeCorridor(4 * 0 + 2, true, true);
    maze->makeCorridor(4 * 21 + 2, true, true);
    maze->makeCorridor(4 * 0 + 1, true, true);
    maze->makeCorridor(4 * 14 + 1, true, true);
    maze->makeCorridor(4 * 28 + 1, true, true);
    maze->makeCorridor(4 * 1 + 1, true, true);
    maze->makeCorridor(4 * 15 + 1, true, true);
    maze->makeCorridor(4 * 29 + 1, true, true);
    maze->makeCorridor(4 * 2 + 2, true, true);
    maze->makeCorridor(4 * 9 + 2, true, true);
    maze->makeCorridor(4 * 16 + 2, true, true);
    maze->makeCorridor(4 * 23 + 2, true, true);
    maze->makeCorridor(4 * 10 + 2, true, true);
    maze->makeCorridor(4 * 24 + 2, true, true);
    maze->makeCorridor(4 * 3 + 1, true, true);
    maze->makeCorridor(4 * 17 + 1, true, true);
    maze->makeCorridor(4 * 24 + 1, true, true);
    maze->makeCorridor(4 * 4 + 1, true, true);
    maze->makeCorridor(4 * 18 + 1, true, true);
    maze->makeCorridor(4 * 25 + 1, true, true);
    maze->makeCorridor(4 * 12 + 2, true, true);
    maze->makeCorridor(4 * 26 + 1, true, true);
    maze->makeCorridor(4 * 20 + 2, true, true);
    maze->makeCorridor(4 * 27 + 2, true, true);

    maze->setCorridorChooser(door);
    maze->makeCorridor(4 * 7 + 1, true, true);
    maze->makeCorridor(4 * 21 + 1, true, true);
    maze->makeCorridor(4 * 31 + 1, true, true);
    maze->makeCorridor(4 * 4 + 2, true, true);
    maze->makeCorridor(4 * 33 + 2, false);

    maze->setCorridorChooser(left);
    maze->makeCorridor(4 * 6 + 3, true, true);
    maze->makeCorridor(4 * 13 + 0, true, true);

    glow::SharedTexture2D flatbrickAlbedo = glow::Texture2D::createFromFile("../data/textures/flatbrick.albedo.png", glow::ColorSpace::sRGB);
    glow::SharedTexture2D flatbrickNormal = glow::Texture2D::createFromFile("../data/textures/flatbrick.normal.png", glow::ColorSpace::Linear);

    glow::SharedTexture2D redtilesAlbedo = glow::Texture2D::createFromFile("../data/textures/redtiles.albedo.jpg", glow::ColorSpace::sRGB);
    glow::SharedTexture2D redtilesNormal = glow::Texture2D::createFromFile("../data/textures/redtiles.normal.jpg", glow::ColorSpace::Linear);

    glow::SharedTexture2D redasphaltAlbedo = glow::Texture2D::createFromFile("../data/textures/redasphalt.albedo.jpg", glow::ColorSpace::sRGB);
    glow::SharedTexture2D redasphaltNormal = glow::Texture2D::createFromFile("../data/textures/redasphalt.normal.png", glow::ColorSpace::Linear);

    SharedDrawable drawable = std::make_shared<Drawable>();
    drawable->setAlbedo(flatbrickAlbedo);
    drawable->setNormal(flatbrickNormal);
    RenderWorld::getInstance()->getRoot()->addChild(drawable);

    SharedDrawable drawableFloor = std::make_shared<Drawable>();
    drawableFloor->setAlbedo(redtilesAlbedo);
    drawableFloor->setNormal(redtilesNormal);
    RenderWorld::getInstance()->getRoot()->addChild(drawableFloor);

    SharedDrawable drawableCeil = std::make_shared<Drawable>();
    drawableCeil->setAlbedo(redasphaltAlbedo);
    drawableCeil->setNormal(redasphaltNormal);
    RenderWorld::getInstance()->getRoot()->addChild(drawableCeil);

    converter->setMeshes(&meshes);
    converter->convert(maze);

    drawable->setMesh(meshes[0]);
    drawableFloor->setMesh(meshes[1]);
    drawableCeil->setMesh(meshes[2]);
    meshes.clear();

    CollisionObject* source1Body = bodies[4 * 8 + 1];
    CollisionObject* target2Body = bodies[4 * 3 + 3];

    CollisionObject* source3Body = bodies[4 * 13 + 2];
    CollisionObject* target3Body = bodies[4 * 18 + 2];

    CollisionObject* source4Body = bodies[4 * 17 + 3];
    CollisionObject* target4Body = bodies[4 * 20 + 0];

    CollisionObject* source5Body = bodies[4 * 34 + 2];
    CollisionObject* target6Body = bodies[4 * 33 + 0];

    createMirror(maze, 4 * 22 + 1);
    createMirror(maze, 4 * 11 + 1);
    createMirror(maze, 4 * 11 + 3);
    createMirror(maze, 4 * 27 + 1);
    createMirror(maze, 4 * 10 + 0);
    createMirror(maze, 4 * 12 + 0);

    // CREATE OCT MAZE

    SharedMaze2D octMaze = std::make_shared<Maze2D>();
    mazes.push_back(octMaze);

    octMaze->createVertex({-3 * tileW, -4 * tileH});
    octMaze->createVertex({-4 * tileW, -4 * tileH});
    octMaze->createVertex({-5 * tileW, -3 * tileH});
    octMaze->createVertex({-5 * tileW, -2 * tileH});
    octMaze->createVertex({-4 * tileW, -1 * tileH});
    octMaze->createVertex({-3 * tileW, -1 * tileH});
    octMaze->createVertex({-2 * tileW, -2 * tileH});
    octMaze->createVertex({-2 * tileW, -3 * tileH});

    param.clear();
    param.push_back(0);
    param.push_back(1);
    param.push_back(2);
    param.push_back(3);
    param.push_back(4);
    param.push_back(5);
    param.push_back(6);
    param.push_back(7);
    octMaze->createFace(param);

    octMaze->connect();

    glow::SharedTexture2D offsetpavementAlbedo = glow::Texture2D::createFromFile("../data/textures/offsetpavement.albedo.jpg", glow::ColorSpace::sRGB);
    glow::SharedTexture2D offsetpavementNormal = glow::Texture2D::createFromFile("../data/textures/offsetpavement.normal.jpg", glow::ColorSpace::Linear);

    glow::SharedTexture2D checkertilesAlbedo = glow::Texture2D::createFromFile("../data/textures/checkertiles.albedo.png", glow::ColorSpace::sRGB);
    glow::SharedTexture2D checkertilesNormal = glow::Texture2D::createFromFile("../data/textures/checkertiles.normal.png", glow::ColorSpace::Linear);

    glow::SharedTexture2D stoneAlbedo = glow::Texture2D::createFromFile("../data/textures/stone.albedo.png", glow::ColorSpace::sRGB);
    glow::SharedTexture2D stoneNormal = glow::Texture2D::createFromFile("../data/textures/stone.normal.png", glow::ColorSpace::Linear);

    SharedDrawable octDrawable = std::make_shared<Drawable>();
    octDrawable->setAlbedo(offsetpavementAlbedo);
    octDrawable->setNormal(offsetpavementNormal);
    RenderWorld::getInstance()->getRoot()->addChild(octDrawable);

    SharedDrawable octDrawableFloor = std::make_shared<Drawable>();
    octDrawableFloor->setAlbedo(checkertilesAlbedo);
    octDrawableFloor->setNormal(checkertilesNormal);
    RenderWorld::getInstance()->getRoot()->addChild(octDrawableFloor);

    SharedDrawable octDrawableCeil = std::make_shared<Drawable>();
    octDrawableCeil->setAlbedo(stoneAlbedo);
    octDrawableCeil->setNormal(stoneNormal);
    RenderWorld::getInstance()->getRoot()->addChild(octDrawableCeil);

    converter->setMeshes(&meshes);
    converter->convert(octMaze);

    octDrawable->setMesh(meshes[0]);
    octDrawableFloor->setMesh(meshes[1]);
    octDrawableCeil->setMesh(meshes[2]);
    meshes.clear();

    CollisionObject* target1Body = bodies[4];
    CollisionObject* source2Body = bodies[1];

    // CREATE TRICK MAZE

    SharedMaze2D trickMaze = std::make_shared<Maze2D>();
    mazes.push_back(trickMaze);

    for (int z = -2; z <= 0; z++)
    {
        for (int x = 5; x >= 0; x--)
        {
            trickMaze->createVertex({x * tileW - 6.0f * tileW, z * tileH + 4.0f * tileH});
        }
    }

    for (int z = 0; z < 2; z++)
    {
        for (int x = 0; x < 5; x++)
        {
            param.clear();
            param.push_back((x + 0) + (z + 0) * 6);
            param.push_back((x + 1) + (z + 0) * 6);
            param.push_back((x + 1) + (z + 1) * 6);
            param.push_back((x + 0) + (z + 1) * 6);
            trickMaze->createFace(param);
        }
    }

    trickMaze->connect();

    trickMaze->setCorridorChooser(none);
    trickMaze->makeCorridor(4 * 0 + 2, true, true);
    trickMaze->makeCorridor(4 * 0 + 1, true, true);
    trickMaze->makeCorridor(4 * 5 + 1, true, true);
    trickMaze->makeCorridor(4 * 1 + 2, true, true);

    trickMaze->setCorridorChooser(door);
    trickMaze->makeCorridor(4 * 1 + 1, true, true);
    trickMaze->makeCorridor(4 * 6 + 1, true, true);
    trickMaze->makeCorridor(4 * 2 + 2, true, true);
    trickMaze->makeCorridor(4 * 3 + 1, true, true);
    trickMaze->makeCorridor(4 * 8 + 1, true, true);
    trickMaze->makeCorridor(4 * 3 + 2, true, true);
    trickMaze->makeCorridor(4 * 4 + 2, true, true);

    glow::SharedTexture2D milanopavementAlbedo = glow::Texture2D::createFromFile("../data/textures/milanopavement.albedo.png", glow::ColorSpace::sRGB);
    glow::SharedTexture2D milanopavementNormal = glow::Texture2D::createFromFile("../data/textures/milanopavement.normal.png", glow::ColorSpace::Linear);

    glow::SharedTexture2D rockAlbedo = glow::Texture2D::createFromFile("../data/textures/rock.albedo.png", glow::ColorSpace::sRGB);
    glow::SharedTexture2D rockNormal = glow::Texture2D::createFromFile("../data/textures/rock.normal.png", glow::ColorSpace::Linear);

    glow::SharedTexture2D marbletilesAlbedo = glow::Texture2D::createFromFile("../data/textures/marbletiles.albedo.jpg", glow::ColorSpace::sRGB);
    glow::SharedTexture2D marbletilesNormal = glow::Texture2D::createFromFile("../data/textures/marbletiles.normal.png", glow::ColorSpace::Linear);

    SharedDrawable trickDrawable = std::make_shared<Drawable>();
    trickDrawable->setAlbedo(milanopavementAlbedo);
    trickDrawable->setNormal(milanopavementNormal);
    RenderWorld::getInstance()->getRoot()->addChild(trickDrawable);

    SharedDrawable trickDrawableFloor = std::make_shared<Drawable>();
    trickDrawableFloor->setAlbedo(rockAlbedo);
    trickDrawableFloor->setNormal(rockNormal);
    RenderWorld::getInstance()->getRoot()->addChild(trickDrawableFloor);

    SharedDrawable trickDrawableCeil = std::make_shared<Drawable>();
    trickDrawableCeil->setAlbedo(marbletilesAlbedo);
    trickDrawableCeil->setNormal(marbletilesNormal);
    RenderWorld::getInstance()->getRoot()->addChild(trickDrawableCeil);

    converter->setMeshes(&meshes);
    converter->convert(trickMaze);

    trickDrawable->setMesh(meshes[0]);
    trickDrawableFloor->setMesh(meshes[1]);
    trickDrawableCeil->setMesh(meshes[2]);
    meshes.clear();

    CollisionObject* target5Body = bodies[4 * 0 + 0];
    CollisionObject* source6Body = bodies[4 * 5 + 2];

    // MAZES DONE

    portal1 = std::make_shared<DynamicPortal>(maze, 4 * 8 + 1, octMaze, 4);
    portal1->addObstacles(source1Body, target1Body);
    portal1->lockTransforms();
    portal1->enable(true, false, true, false);

    portal2 = std::make_shared<DynamicPortal>(octMaze, 1, maze, 4 * 3 + 3);
    portal2->addObstacles(source2Body, target2Body);
    portal2->lockTransforms();
    portal2->enable(false, true, true, false);

    portal3 = std::make_shared<DynamicPortal>(maze, 4 * 13 + 2, maze, 4 * 18 + 2);
    portal3->addObstacles(source3Body, target3Body);
    portal3->lockTransforms();
    portal3->enable();

    portal4 = std::make_shared<DynamicPortal>(maze, 4 * 17 + 3, maze, 4 * 20 + 0);
    portal4->addObstacles(source4Body, target4Body);
    portal4->lockTransforms();
    portal4->enable();

    portal5 = std::make_shared<DynamicPortal>(maze, 4 * 34 + 2, trickMaze, 4 * 0 + 0);
    portal5->addObstacles(source5Body, target5Body);
    portal5->lockTransforms();
    portal5->enable(true, true, false, false);

    portal6 = std::make_shared<DynamicPortal>(trickMaze, 4 * 5 + 2, maze, 4 * 33 + 0);
    portal6->addObstacles(source6Body, target6Body);
    portal6->lockTransforms();
    // portal6->enable();

    portal7 = std::make_shared<DynamicPortal>(maze, 4 * 19 + 1, maze, 4 * 20 + 0);
    portal7->lockTransforms();
    portal7->enable(true, false, false, false);

    portal8 = std::make_shared<DynamicPortal>(trickMaze, 4 * 6 + 1, trickMaze, 4 * 9 + 3);
    portal8->setWidth(DEFAULT_WIDTH);
    portal8->setHeight(DEFAULT_HEIGHT - 0.002f);
    portal8->setGuard(0.0f);
    portal8->setSourceVOffset(0.001f);
    portal8->setTargetVOffset(0.001f);
    portal8->setSourceZAnchor(-1);
    portal8->setTargetZAnchor(-1);
    portal8->lockTransforms();
    portal8->enable();

    portal9 = std::make_shared<DynamicPortal>(trickMaze, 4 * 4 + 3, trickMaze, 4 * 2 + 2);
    portal9->setWidth(DEFAULT_WIDTH);
    portal9->setHeight(DEFAULT_HEIGHT - 0.002f);
    portal9->setGuard(0.0f);
    portal9->setSourceVOffset(0.001f);
    portal9->setTargetVOffset(0.001f);
    portal9->setSourceZAnchor(-1);
    portal9->setTargetZAnchor(-1);
    portal9->lockTransforms();
    portal9->enable();

    portal10 = std::make_shared<DynamicPortal>(trickMaze, 4 * 3 + 2, trickMaze, 4 * 2 + 2);
    portal10->setWidth(DEFAULT_WIDTH);
    portal10->setHeight(DEFAULT_HEIGHT - 0.002f);
    portal10->setGuard(0.0f);
    portal10->setSourceVOffset(0.001f);
    portal10->setTargetVOffset(0.001f);
    portal10->setSourceZAnchor(-1);
    portal10->setTargetZAnchor(-1);
    portal10->lockTransforms();
    // portal10->enable();

    // KEYS

    key1Object = std::make_shared<Drawable>();
    glow::SharedTexture2D key1Albedo = glow::Texture2D::createFromFile("../data/textures/key2.albedo.jpg", glow::ColorSpace::sRGB);
    glow::SharedTexture2D key1Normal = glow::Texture2D::createFromFile("../data/textures/key2.normal.jpg", glow::ColorSpace::Linear);
    key1Object->setTransform(glm::translate(glm::vec3(2.5f * tileW, 1.0f, -0.5f * tileH)) * glm::rotate(glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    key1Object->setAlbedo(key1Albedo);
    key1Object->setNormal(key1Normal);
    key1Object->setMesh(load_mesh_from_obj("../data/meshes/key2.obj"));
    RenderWorld::getInstance()->getRoot()->addChild(key1Object);

    key2Object = std::make_shared<Drawable>();
    glow::SharedTexture2D key2Albedo = glow::Texture2D::createFromFile("../data/textures/key1.albedo.png", glow::ColorSpace::sRGB);
    glow::SharedTexture2D key2Normal = glow::Texture2D::createFromFile("../data/textures/key1.normal.png", glow::ColorSpace::Linear);
    key2Object->setTransform(glm::translate(glm::vec3(-2.5f * tileW, 1.0f, 2.5f * tileH)) * glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
    key2Object->setAlbedo(key2Albedo);
    key2Object->setNormal(key2Normal);
    key2Object->setMesh(load_mesh_from_obj("../data/meshes/key1.obj"));
    RenderWorld::getInstance()->getRoot()->addChild(key2Object);

    // TRIGGERS

    key1Trigger = std::make_shared<CollisionObject>();
    key1Trigger->initTrigger(new btBoxShape(btVector3(0.5f, 0.5f, 0.5f)), glm::translate(glm::vec3(2.5f * tileW, 0.5f, -0.5f * tileH)), COLLISION_TRIGGER);
    CollisionWorld::getInstance()->addObject(key1Trigger);

    key2Trigger = std::make_shared<CollisionObject>();
    key2Trigger->initTrigger(new btBoxShape(btVector3(0.5f, 0.5f, 0.5f)), glm::translate(glm::vec3(-2.5f * tileW, 0.5f, 2.5f * tileH)), COLLISION_TRIGGER);
    CollisionWorld::getInstance()->addObject(key2Trigger);

    exitTrigger = std::make_shared<CollisionObject>();
    exitTrigger->initTrigger(new btBoxShape(btVector3(DEFAULT_WIDTH / 2.0f, DEFAULT_HEIGHT / 2.0f, PORTAL_TRIGGER_DEPTH)),
                             maze->attachRectangle(4 * 33 + 2, DEFAULT_WIDTH, DEFAULT_HEIGHT, PORTAL_SOURCE), COLLISION_TRIGGER);
    CollisionWorld::getInstance()->addObject(exitTrigger);

    // FENCES

    glow::SharedTexture2D fenceAlbedo = glow::Texture2D::createFromFile("../data/textures/fence.albedo.jpg", glow::ColorSpace::sRGB);
    glow::SharedTexture2D fenceNormal = glow::Texture2D::createFromFile("../data/textures/fence.normal.jpg", glow::ColorSpace::Linear);
    glow::SharedVertexArray fenceMesh = load_mesh_from_obj("../data/meshes/fence.obj");

    SharedDrawable drawableFence1 = std::make_shared<Drawable>();
    drawableFence1->setTransform(glm::translate(glm::vec3(-2.5f * tileW, 0.0f, 3.0f * tileH)));
    drawableFence1->setAlbedo(fenceAlbedo);
    drawableFence1->setNormal(fenceNormal);
    drawableFence1->setMesh(fenceMesh);
    RenderWorld::getInstance()->getRoot()->addChild(drawableFence1);

    SharedCollisionObject collisionFence1 = std::make_shared<CollisionObject>();
    collisionFence1->initStatic(new btBoxShape(btVector3(4.0f, 1.5f, 0.1f)), glm::translate(glm::vec3(-2.5f * tileW, 1.5f, 3.0f * tileH)));
    CollisionWorld::getInstance()->addObject(collisionFence1);

    SharedDrawable drawableFence2 = std::make_shared<Drawable>();
    drawableFence2->setTransform(glm::translate(glm::vec3(-2.0f * tileW, 0.0f, 2.5f * tileH)) * glm::rotate(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
    drawableFence2->setAlbedo(fenceAlbedo);
    drawableFence2->setNormal(fenceNormal);
    drawableFence2->setMesh(fenceMesh);
    RenderWorld::getInstance()->getRoot()->addChild(drawableFence2);

    SharedCollisionObject collisionFence2 = std::make_shared<CollisionObject>();
    collisionFence2->initStatic(new btBoxShape(btVector3(0.1f, 1.5f, 4.0f)), glm::translate(glm::vec3(-2.0f * tileW, 0.0f, 2.5f * tileH)));
    CollisionWorld::getInstance()->addObject(collisionFence2);

    // LIGHTING

    for (size_t i = 0; i < maze->getFaceCount(); i++)
    {
        if (i == 0 || i == 2 || i == 8 || i == 14 || i == 16 || i == 22 || i == 28 || i == 30)
        {
            glm::vec3 lightPos = maze->getCenter(i);
            lightPos.y += 2.0f;

            SharedPointLight light = std::make_shared<PointLight>(lightPos);
            light->setIntensity(3.0f);
            light->setAttenuation(16.0f);
            light->initObjects(64, 32);
            RenderWorld::getInstance()->getPointLights()->push_back(light);
        }

        if (i == 6 || i == 11 || i == 24 || i == 27 || i == 32)
        {
            glm::vec3 lightPos = maze->getCenter(i);
            lightPos.y += 2.0f;

            SharedPointLight light = std::make_shared<PointLight>(lightPos);
            light->setIntensity(3.0f);
            light->setAttenuation(16.0f);
            light->initObjects(64, 32);
            RenderWorld::getInstance()->getPointLights()->push_back(light);
        }

        if (i == 17)
        {
            glm::vec3 lightPos = maze->getCenter(i);
            lightPos.y += 2.0f;

            SharedPointLight light = std::make_shared<PointLight>(lightPos);
            light->setColor(glm::vec3(1.0f, 0.0f, 0.0f));
            light->setIntensity(15.0f);
            light->setAttenuation(6.0f);
            light->initObjects(64, 32);
            RenderWorld::getInstance()->getPointLights()->push_back(light);
        }

        if (i == 19)
        {
            glm::vec3 lightPos = maze->getCenter(i);
            lightPos.y += 2.0f;

            SharedPointLight light = std::make_shared<PointLight>(lightPos);
            light->setColor(glm::vec3(0.0f, 0.0f, 1.0f));
            light->setIntensity(15.0f);
            light->setAttenuation(6.0f);
            light->initObjects(64, 32);
            RenderWorld::getInstance()->getPointLights()->push_back(light);
        }

        if (i == 33)
        {
            glm::vec3 lightPos = maze->getCenter(i);
            lightPos.y += 2.0f;

            SharedPointLight light = std::make_shared<PointLight>(lightPos);
            light->setIntensity(15.0f);
            light->setAttenuation(6.0f);
            light->initObjects(64, 32);
            RenderWorld::getInstance()->getPointLights()->push_back(light);
        }
    }

    for (size_t i = 0; i < octMaze->getFaceCount(); i++)
    {
        glm::vec3 lightPos = octMaze->getCenter(i);
        lightPos.y += 2.0f;

        SharedPointLight light = std::make_shared<PointLight>(lightPos);
        light->setIntensity(15.0f);
        light->setAttenuation(16.0f);
        light->initObjects(64, 32);

        RenderWorld::getInstance()->getPointLights()->push_back(light);
    }

    for (size_t i = 0; i < trickMaze->getFaceCount(); i++)
    {
        glm::vec3 lightPos = trickMaze->getCenter(i);
        lightPos.y += 2.0f;

        SharedPointLight light = std::make_shared<PointLight>(lightPos);
        light->setIntensity(3.0f);
        light->setAttenuation(3.0f);
        light->initObjects(64, 32);

        RenderWorld::getInstance()->getPointLights()->push_back(light);
    }
}
