#include "Level.hh"

#include "CameraObject.hh"
#include "LogicWorld.hh"
#include "PlayerObject.hh"

#include "../collision/CollisionObject.hh"
#include "../collision/CollisionWorld.hh"
#include "../collision/Gate.hh"
#include "../maze/Maze2D.hh"
#include "../maze/MazeConverter2D.hh"
#include "../maze/MazeGenerator2D.hh"
#include "../maze/WallState.hh"
#include "../render/Drawable.hh"
#include "../render/Portal.hh"
#include "../render/Light.hh"
#include "../render/QueryHandler.hh"
#include "../render/RenderWorld.hh"

#include "../Game.hh"
#include "../MeshHelper.hh"
#include "AnimationComponent.hh"
#include "load_mesh.hh"

void Level::init() {
    
}

void Level::tick(float elapsed) {

}

void Level::update(float elapsed) {
    
}

void Level::trigger(CollisionObject* actor, CollisionObject* other) {
    
}

SharedGlobalLight Level::initGlobalLight(glm::vec3 dir, glm::vec3 color, float ambient, float intensity) {
    SharedGlobalLight globalLight = std::make_shared<GlobalLight>(dir, color, ambient, intensity);
    RenderWorld::getInstance()->getGlobalLights()->push_back(globalLight);
    return globalLight;
}

void Level::initPlayer(float scale, glm::vec3 position, float cameraDistance) {
    SharedCollisionObject collision = std::make_shared<CollisionObject>();
    //collision->initDynamic(new btBoxShape(btVector3(1, 1, 1) * scale), glm::translate(position), 1.0f, 0.3f, 0.001f, 0.1f, 0.1f);
    collision->initDynamic(new btSphereShape(scale), glm::translate(position), 1.0f, 0.3f, 0.001f, 0.1f, 0.1f);
    collision->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));

    //glow::SharedTexture2D cubeAlbedo = glow::Texture2D::createFromFile("../data/textures/cube.albedo.png", glow::ColorSpace::sRGB);
    //glow::SharedTexture2D cubeNormal = glow::Texture2D::createFromFile("../data/textures/cube.normal.png", glow::ColorSpace::Linear);
    //glow::SharedTexture2D cubeAlbedo = glow::Texture2D::createFromFile("../data/models/floating_robot/textures/robot2_baseColor.png", glow::ColorSpace::sRGB);
    //glow::SharedTexture2D cubeAlbedo = glow::Texture2D::createFromFile("../data/models/floating_robot/textures/robot2_baseColor.png", glow::ColorSpace::sRGB);
    //glow::SharedTexture2D cubeNormal = glow::Texture2D::createFromFile("../data/models/floating_robot/textures/robot2_metallicRoughness.png", glow::ColorSpace::Linear);
    
    SharedPlayerObject player = std::make_shared<PlayerObject>(collision);
    animation = std::make_shared<AnimationComponent>();
    load_mesh_from_obj("../data/meshes/cube.obj");
    animation->loadGLTFModel("../data/models/floating_robot/scene.gltf", 0.008f);
    //player->setMesh(MeshHelper::createBox(scale, scale, scale));
    player->setMesh(animation->getMesh());
    //cubeAlbedo = glow::Texture2D::createFromFile("../data/models/key/textures/green_baseColor.png", glow::ColorSpace::sRGB);
    player->setAlbedo(animation->getAlbedoTex());
    player->setNormal(animation->getNormalTex());

    //player->setAlbedo(cubeAlbedo);
    //player->setNormal(cubeNormal);

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

void Level::initGround(float w, float h, float t, glm::vec3 pos)
{
    SharedCollisionObject collision = std::make_shared<CollisionObject>();
    collision->initStatic(new btBoxShape(btVector3(w, t, h)), glm::translate(pos));
    CollisionWorld::getInstance()->addObject(collision);
}

void Level::createPortalPair(SharedMaze2D sourceMaze,
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
    SharedPortal back = std::make_shared<Portal>(false, width, height);

    main->setSource(sourceMaze->attachRectangle(sourceWall, width, height, PORTAL_SOURCE, sourceHAnchor, sourceHOffset, sourceVAnchor, sourceVOffset, guard, NO_MIRROR, sourceZAnchor, sourceZOffset));
    main->setTarget(targetMaze->attachRectangle(targetWall, width, height, PORTAL_TARGET, targetHAnchor, targetHOffset, targetVAnchor, targetVOffset, guard, NO_MIRROR, targetZAnchor, targetZOffset));

    back->setSource(targetMaze->attachRectangle(targetWall, width, height, PORTAL_SOURCE, targetHAnchor, targetHOffset, targetVAnchor, targetVOffset, guard, NO_MIRROR, targetZAnchor, targetZOffset));
    back->setTarget(sourceMaze->attachRectangle(sourceWall, width, height, PORTAL_TARGET, sourceHAnchor, sourceHOffset, sourceVAnchor, sourceVOffset, guard, NO_MIRROR, sourceZAnchor, sourceZOffset));

    RenderWorld::getInstance()->addPortal(main);
    RenderWorld::getInstance()->addPortal(back);

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

    size_t mainIndex = RenderWorld::getInstance()->indexOf(main);
    size_t backIndex = RenderWorld::getInstance()->indexOf(back);

    mainGate->getQueryHandler()->setup(mainIndex, backIndex);
    backGate->getQueryHandler()->setup(backIndex, mainIndex);
}

void Level::createMirror(SharedMaze2D maze, int32_t wall, float width, float height, int hAnchor, float hOffset, int vAnchor, float vOffset)
{
    SharedPortal mirror = std::make_shared<Portal>(true, width, height);

    mirror->setSource(maze->attachRectangle(wall, width, height, PORTAL_SOURCE, hAnchor, hOffset, vAnchor, vOffset));
    mirror->setTarget(maze->attachRectangle(wall, width, height, PORTAL_TARGET, hAnchor, hOffset, vAnchor, vOffset, DEFAULT_GUARD, MIRROR));

    RenderWorld::getInstance()->addPortal(mirror);
}
