#include "TrickLevel.hh"

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

#include <GLFW/glfw3.h>

TrickLevel::TrickLevel() : oldClick(false), mazes(), marks(), touchedTrigger(false), swapped(false) {}

TrickLevel::~TrickLevel() {}

void TrickLevel::tick(float elapsed)
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

void TrickLevel::update(float elapsed) {}

void TrickLevel::trigger(CollisionObject* actor, CollisionObject* other)
{
    if (other == triggerIn.get())
    {
        touchedTrigger = true;
    }
    else
    {
        if (touchedTrigger)
        {
            swap();
            touchedTrigger = false;
        }
    }
}

void TrickLevel::swap()
{
    swapped = !swapped;

    marks->reset();

    if (swapped)
    {
        threePortal->disable();
        fourPortal->enable();
    }
    else
    {
        fourPortal->disable();
        threePortal->enable();
    }
}

void TrickLevel::init()
{
    float tileW = 8.0f, tileH = 8.0f;

    globalLight = initGlobalLight(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.2f);

    marks = std::make_shared<MazeMarkList>();
    RenderWorld::getInstance()->getRoot()->addChild(marks->getNode());

    initPlayer(0.3f, glm::vec3(tileW, 0.3f, -0.5 * tileH), 0.1f);
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

    mainMaze = std::make_shared<Maze2D>();
    mazes.push_back(mainMaze);

    for (int z = -2; z <= 0; z++)
    {
        for (int x = 2; x >= 0; x--)
        {
            mainMaze->createVertex({x * tileW, z * tileH});
        }
    }

    for (int z = 0; z < 2; z++)
    {
        for (int x = 0; x < 2; x++)
        {
            param.clear();
            param.push_back((x + 0) + (z + 0) * 3);
            param.push_back((x + 1) + (z + 0) * 3);
            param.push_back((x + 1) + (z + 1) * 3);
            param.push_back((x + 0) + (z + 1) * 3);
            mainMaze->createFace(param);
        }
    }

    mainMaze->connect();

    mainMaze->setCorridorChooser(none);
    mainMaze->makeCorridor(4 * 3 + 3, true, true);
    mainMaze->makeCorridor(4 * 3 + 2, false);
    mainMaze->makeCorridor(4 * 2 + 2, false);

    mainMaze->setCorridorChooser(door);
    mainMaze->makeCorridor(4 * 1 + 3, true, true);
    mainMaze->makeCorridor(4 * 2 + 0, true, true);
    mainMaze->makeCorridor(4 * 3 + 0, true, true);

    SharedDrawable mainDrawable = std::make_shared<Drawable>();
    glow::SharedTexture2D milanopavementAlbedo = glow::Texture2D::createFromFile("../data/textures/milanopavement.albedo.png", glow::ColorSpace::sRGB);
    glow::SharedTexture2D milanopavementNormal = glow::Texture2D::createFromFile("../data/textures/milanopavement.normal.png", glow::ColorSpace::Linear);
    mainDrawable->setAlbedo(milanopavementAlbedo);
    mainDrawable->setNormal(milanopavementNormal);
    RenderWorld::getInstance()->getRoot()->addChild(mainDrawable);

    SharedDrawable mainDrawableFloor = std::make_shared<Drawable>();
    mainDrawableFloor->setAlbedo(milanopavementAlbedo);
    mainDrawableFloor->setNormal(milanopavementNormal);
    RenderWorld::getInstance()->getRoot()->addChild(mainDrawableFloor);

    SharedDrawable mainDrawableCeil = std::make_shared<Drawable>();
    mainDrawableCeil->setAlbedo(milanopavementAlbedo);
    mainDrawableCeil->setNormal(milanopavementNormal);
    RenderWorld::getInstance()->getRoot()->addChild(mainDrawableCeil);

    converter->setMeshes(&meshes);
    converter->convert(mainMaze);

    mainDrawable->setMesh(meshes[0]);
    mainDrawableFloor->setMesh(meshes[1]);
    mainDrawableCeil->setMesh(meshes[2]);
    meshes.clear();

    sideMaze = std::make_shared<Maze2D>();
    mazes.push_back(sideMaze);

    for (int z = -2; z <= 0; z++)
    {
        for (int x = 2; x >= 0; x--)
        {
            sideMaze->createVertex({x * tileW, z * tileH - 3 * tileH});
        }
    }

    for (int z = 0; z < 2; z++)
    {
        for (int x = 0; x < 2; x++)
        {
            param.clear();
            param.push_back((x + 0) + (z + 0) * 3);
            param.push_back((x + 1) + (z + 0) * 3);
            param.push_back((x + 1) + (z + 1) * 3);
            param.push_back((x + 0) + (z + 1) * 3);
            sideMaze->createFace(param);
        }
    }

    sideMaze->connect();

    sideMaze->setCorridorChooser(door);
    sideMaze->makeCorridor(4 * 1 + 3, true, true);
    sideMaze->makeCorridor(4 * 2 + 0, true, true);
    sideMaze->makeCorridor(4 * 3 + 0, true, true);
    sideMaze->makeCorridor(4 * 3 + 3, true, true);

    SharedDrawable sideDrawable = std::make_shared<Drawable>();
    sideDrawable->setAlbedo(milanopavementAlbedo);
    sideDrawable->setNormal(milanopavementNormal);
    RenderWorld::getInstance()->getRoot()->addChild(sideDrawable);

    SharedDrawable sideDrawableFloor = std::make_shared<Drawable>();
    sideDrawableFloor->setAlbedo(milanopavementAlbedo);
    sideDrawableFloor->setNormal(milanopavementNormal);
    RenderWorld::getInstance()->getRoot()->addChild(sideDrawableFloor);

    SharedDrawable sideDrawableCeil = std::make_shared<Drawable>();
    sideDrawableCeil->setAlbedo(milanopavementAlbedo);
    sideDrawableCeil->setNormal(milanopavementNormal);
    RenderWorld::getInstance()->getRoot()->addChild(sideDrawableCeil);

    converter->setMeshes(&meshes);
    converter->convert(sideMaze);

    sideDrawable->setMesh(meshes[0]);
    sideDrawableFloor->setMesh(meshes[1]);
    sideDrawableCeil->setMesh(meshes[2]);
    meshes.clear();

    triggerMaze = std::make_shared<Maze2D>();
    mazes.push_back(triggerMaze);

    for (int z = -1; z <= 0; z++)
    {
        for (int x = 1; x >= 0; x--)
        {
            triggerMaze->createVertex({2 * x * tileW, 2 * z * tileH + 2 * tileH});
        }
    }

    for (int z = 0; z < 1; z++)
    {
        for (int x = 0; x < 1; x++)
        {
            param.clear();
            param.push_back((x + 0) + (z + 0) * 2);
            param.push_back((x + 1) + (z + 0) * 2);
            param.push_back((x + 1) + (z + 1) * 2);
            param.push_back((x + 0) + (z + 1) * 2);
            triggerMaze->createFace(param);
        }
    }

    triggerMaze->connect();

    triggerMaze->setCorridorChooser(none);
    triggerMaze->makeCorridor(4 * 0 + 0, false);

    SharedDrawable triggerDrawable = std::make_shared<Drawable>();
    glow::SharedTexture2D cubeAlbedo = glow::Texture2D::createFromFile("../data/textures/cube.albedo.png", glow::ColorSpace::sRGB);
    glow::SharedTexture2D cubeNormal = glow::Texture2D::createFromFile("../data/textures/cube.normal.png", glow::ColorSpace::Linear);
    triggerDrawable->setAlbedo(cubeAlbedo);
    triggerDrawable->setNormal(cubeNormal);
    RenderWorld::getInstance()->getRoot()->addChild(triggerDrawable);

    SharedDrawable triggerDrawableFloor = std::make_shared<Drawable>();
    triggerDrawableFloor->setAlbedo(cubeAlbedo);
    triggerDrawableFloor->setNormal(cubeNormal);
    RenderWorld::getInstance()->getRoot()->addChild(triggerDrawableFloor);

    SharedDrawable triggerDrawableCeil = std::make_shared<Drawable>();
    triggerDrawableCeil->setAlbedo(cubeAlbedo);
    triggerDrawableCeil->setNormal(cubeNormal);
    RenderWorld::getInstance()->getRoot()->addChild(triggerDrawableCeil);

    converter->setMeshes(&meshes);
    converter->convert(triggerMaze);

    triggerDrawable->setMesh(meshes[0]);
    triggerDrawableFloor->setMesh(meshes[1]);
    triggerDrawableCeil->setMesh(meshes[2]);
    meshes.clear();

    constPortal = std::make_shared<DynamicPortal>(mainMaze, 4 * 1 + 3, sideMaze, 4 * 1 + 2);
    constPortal->setWidth(DEFAULT_WIDTH);
    constPortal->setHeight(DEFAULT_HEIGHT - 0.002f);
    constPortal->setGuard(0.0f);
    constPortal->setSourceVOffset(0.001f);
    constPortal->setTargetVOffset(0.001f);
    constPortal->setSourceZAnchor(-1);
    constPortal->setTargetZAnchor(-1);

    constPortal->lockTransforms();
    constPortal->enable();

    /*createPortalPair(mainMaze, 4 * 1 + 3, nullptr, sideMaze, 4 * 1 + 2, nullptr, DEFAULT_WIDTH, DEFAULT_HEIGHT - 0.002f, 0, 0.0f, 0, 0.0f, -1, 0.001f,
                     -1, 0.001f, 0.0f, -1, 0.0f, -1, 0.0f);*/

    threePortal = std::make_shared<DynamicPortal>(sideMaze, 4 * 0 + 2, mainMaze, 4 * 2 + 0);
    threePortal->setWidth(DEFAULT_WIDTH);
    threePortal->setHeight(DEFAULT_HEIGHT - 0.002f);
    threePortal->setGuard(0.0f);
    threePortal->setSourceVOffset(0.001f);
    threePortal->setTargetVOffset(0.001f);
    threePortal->setSourceZAnchor(-1);
    threePortal->setTargetZAnchor(-1);

    threePortal->lockTransforms();
    threePortal->enable();

    fourPortal = std::make_shared<DynamicPortal>(sideMaze, 4 * 2 + 1, mainMaze, 4 * 2 + 0);
    fourPortal->setWidth(DEFAULT_WIDTH);
    fourPortal->setHeight(DEFAULT_HEIGHT - 0.002f);
    fourPortal->setGuard(0.0f);
    fourPortal->setSourceVOffset(0.001f);
    fourPortal->setTargetVOffset(0.001f);
    fourPortal->setSourceZAnchor(-1);
    fourPortal->setTargetZAnchor(-1);

    fourPortal->lockTransforms();
    //fourPortal->enable();

    triggerIn = std::make_shared<CollisionObject>();
    triggerIn->initTrigger(new btBoxShape(btVector3(tileW, DEFAULT_HEIGHT / 2.0f, 0.5f)),
                           triggerMaze->attachRectangle(4 * 0 + 2, 2 * tileW, DEFAULT_HEIGHT, PORTAL_SOURCE), COLLISION_TRIGGER);

    CollisionWorld::getInstance()->addObject(triggerIn);

    triggerOut = std::make_shared<CollisionObject>();
    triggerOut->initTrigger(new btBoxShape(btVector3(tileW, DEFAULT_HEIGHT / 2.0f, 0.5f)),
                            triggerMaze->attachRectangle(4 * 0 + 0, 2 * tileW, DEFAULT_HEIGHT, PORTAL_SOURCE), COLLISION_TRIGGER);

    CollisionWorld::getInstance()->addObject(triggerOut);
}
