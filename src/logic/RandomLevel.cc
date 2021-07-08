#include "RandomLevel.hh"

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
#include "../MeshHelper.hh"
#include "../load_mesh.hh"

#include <GLFW/glfw3.h>
#include <glow-extras/geometry/UVSphere.hh>

RandomLevel::RandomLevel() : won(false), oldClick(false), mazes(), marks() {}

RandomLevel::~RandomLevel() {}

void RandomLevel::tick(float elapsed)
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

void RandomLevel::update(float elapsed)
{
    
}

void RandomLevel::trigger(CollisionObject* actor, CollisionObject* other)
{
    if (!won)
    {
        //Renderer::getInstance()->setBackgroundColor(glm::vec3(0.0f, 1.0f, 0.0f));
        glow::info() << "Game won!";
        won = true;
    }
}

void RandomLevel::init()
{
    float tileW = 8.0f, tileH = 8.0f;

    globalLight = initGlobalLight(glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.1f);

    marks = std::make_shared<MazeMarkList>();
    RenderWorld::getInstance()->getRoot()->addChild(marks->getNode());

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

    SharedMaze2D maze = Maze2D::createGrid(8, 8, 8.0f, 8.0f);
    mazes.push_back(maze);

    maze->setCorridorChooser(random);
    maze->applyWallState(undetermined);

    float randFloat = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    int initial = (int)(randFloat * 63.99f);
    maze->setInitial(initial);

    SharedMazeGenerator2D generator = std::make_shared<DFSMazeGenerator>();
    generator->generate(maze);

    maze->setCorridorChooser(door);
    maze->makeCorridor(0, false);

    SharedCollisionObject trigger = std::make_shared<CollisionObject>();
    trigger->initTrigger(new btBoxShape(btVector3(DEFAULT_WIDTH / 2.0f, DEFAULT_HEIGHT / 2.0f, PORTAL_TRIGGER_DEPTH)),
                         maze->attachRectangle(0, DEFAULT_WIDTH, DEFAULT_HEIGHT, PORTAL_SOURCE), COLLISION_TRIGGER);

    CollisionWorld::getInstance()->addObject(trigger);

    glm::vec3 startPos = maze->getCenter(initial);
    startPos.y = 0.3f;
    initPlayer(0.3f, startPos, 0.1f);

    glow::SharedTexture2D wallAlbedo;
    glow::SharedTexture2D wallNormal;

    glow::SharedTexture2D floorAlbedo;
    glow::SharedTexture2D floorNormal;

    glow::SharedTexture2D ceilAlbedo;
    glow::SharedTexture2D ceilNormal;

    randFloat = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);

    if (randFloat < 0.3f)
    {
        wallAlbedo = glow::Texture2D::createFromFile("../data/textures/flatbrick.albedo.png", glow::ColorSpace::sRGB);
        wallNormal = glow::Texture2D::createFromFile("../data/textures/flatbrick.normal.png", glow::ColorSpace::Linear);

        floorAlbedo = glow::Texture2D::createFromFile("../data/textures/redtiles.albedo.jpg", glow::ColorSpace::sRGB);
        floorNormal = glow::Texture2D::createFromFile("../data/textures/redtiles.normal.jpg", glow::ColorSpace::Linear);

        ceilAlbedo = glow::Texture2D::createFromFile("../data/textures/redasphalt.albedo.jpg", glow::ColorSpace::sRGB);
        ceilNormal = glow::Texture2D::createFromFile("../data/textures/redasphalt.normal.png", glow::ColorSpace::Linear);
    }
    else if (randFloat < 0.6f)
    {
        wallAlbedo = glow::Texture2D::createFromFile("../data/textures/offsetpavement.albedo.jpg", glow::ColorSpace::sRGB);
        wallNormal = glow::Texture2D::createFromFile("../data/textures/offsetpavement.normal.jpg", glow::ColorSpace::Linear);

        floorAlbedo = glow::Texture2D::createFromFile("../data/textures/checkertiles.albedo.png", glow::ColorSpace::sRGB);
        floorNormal = glow::Texture2D::createFromFile("../data/textures/checkertiles.normal.png", glow::ColorSpace::Linear);

        ceilAlbedo = glow::Texture2D::createFromFile("../data/textures/stone.albedo.png", glow::ColorSpace::sRGB);
        ceilNormal = glow::Texture2D::createFromFile("../data/textures/stone.normal.png", glow::ColorSpace::Linear);
    }
    else
    {
        wallAlbedo = glow::Texture2D::createFromFile("../data/textures/milanopavement.albedo.png", glow::ColorSpace::sRGB);
        wallNormal = glow::Texture2D::createFromFile("../data/textures/milanopavement.normal.png", glow::ColorSpace::Linear);

        floorAlbedo = glow::Texture2D::createFromFile("../data/textures/rock.albedo.png", glow::ColorSpace::sRGB);
        floorNormal = glow::Texture2D::createFromFile("../data/textures/rock.normal.png", glow::ColorSpace::Linear);

        ceilAlbedo = glow::Texture2D::createFromFile("../data/textures/marbletiles.albedo.jpg", glow::ColorSpace::sRGB);
        ceilNormal = glow::Texture2D::createFromFile("../data/textures/marbletiles.normal.png", glow::ColorSpace::Linear);
    }

    SharedDrawable drawable = std::make_shared<Drawable>();
    drawable->setAlbedo(wallAlbedo);
    drawable->setNormal(wallNormal);
    RenderWorld::getInstance()->getRoot()->addChild(drawable);

    SharedDrawable drawableFloor = std::make_shared<Drawable>();
    drawableFloor->setAlbedo(floorAlbedo);
    drawableFloor->setNormal(floorNormal);
    RenderWorld::getInstance()->getRoot()->addChild(drawableFloor);

    SharedDrawable drawableCeil = std::make_shared<Drawable>();
    drawableCeil->setAlbedo(ceilAlbedo);
    drawableCeil->setNormal(ceilNormal);
    RenderWorld::getInstance()->getRoot()->addChild(drawableCeil);

    converter->setMeshes(&meshes);
    converter->convert(maze);

    drawable->setMesh(meshes[0]);
    drawableFloor->setMesh(meshes[1]);
    drawableCeil->setMesh(meshes[2]);
    meshes.clear();

    // LIGHTING

    for (size_t i = 0; i < maze->getFaceCount(); i++)
    {
        glm::vec3 lightPos = maze->getCenter(i);
        lightPos.y += 2.0f;

        SharedPointLight light = std::make_shared<PointLight>(lightPos);
        light->setIntensity(3.0f);
        light->setAttenuation(15.0f);
        light->initObjects(64, 32);
        randFloat = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        if (randFloat < 0.3f)
        {
            RenderWorld::getInstance()->getPointLights()->push_back(light);
        }
    }
}
