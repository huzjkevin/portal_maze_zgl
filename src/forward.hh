#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

#include <glow/common/log.hh>
#include <glow/fwd.hh>

#include <glow-extras/camera/Camera.hh>
#include <glow/objects/Program.hh>

class Game;

// collision

#define COLLISION_STATIC 0
#define COLLISION_KINEMATIC 1
#define COLLISION_DYNAMIC 2
#define COLLISION_SOURCE 3
#define COLLISION_TARGET 4
#define COLLISION_TRIGGER 5

#define NOT_TRAVELLING 0
#define TRAVELLING_SOURCE_CONTACT 1
#define TRAVELLING_INTERMEDIATE 2
#define TRAVELLING_TARGET_CONTACT 3

#define DEFAULT_GRAVITY 9.81f
#define PORTAL_CONTACT_DISTANCE 0.1f

class Gate;
class CollisionObject;
class CollisionWorld;

typedef std::shared_ptr<Gate> SharedGate;
typedef std::shared_ptr<CollisionObject> SharedCollisionObject;
typedef std::shared_ptr<CollisionWorld> SharedCollisionWorld;

// logic

#define CAMERA_DEFAULT_FPS false
#define CAMERA_DEFAULT_FOV 80.0f

#define CAMERA_DEFAULT_DISTANCE 3.0f
#define CAMERA_DEFAULT_PITCH -30.0f

#define PORTAL_TRIGGER_DEPTH 0.5f

#define MOVE_FACTOR_STRAIGHT 4.0f
#define MOVE_FACTOR_SIDE 4.0f
#define MOVE_FACTOR_VERTICAL 0.0f

#define MOVE_FACTOR_SHIFT 2.0f
#define MOVE_FACTOR_CTRL 0.5f

class CameraObject;
class DynamicObject;
class PlayerObject;

class DynamicPortal;

class Level;
class LogicWorld;

typedef std::shared_ptr<CameraObject> SharedCameraObject;
typedef std::shared_ptr<DynamicObject> SharedDynamicObject;
typedef std::shared_ptr<PlayerObject> SharedPlayerObject;

typedef std::shared_ptr<DynamicPortal> SharedDynamicPortal;

typedef std::shared_ptr<Level> SharedLevel;
typedef std::shared_ptr<LogicWorld> SharedLogicWorld;

// maze

#define ONLY_INNER 1
#define INNER_AND_BOUNDARY 0
#define ONLY_BOUNDARY (-1)

#define PORTAL_SOURCE 1.0f
#define PORTAL_TARGET (-1.0f)

#define MIRROR (-1.0f)
#define NO_MIRROR 1.0f

#define VERT_BOTTOM (-1)
#define VERT_MIDDLE 0
#define VERT_TOP 1

#define DEFAULT_GUARD 0.01f

#define DEFAULT_TOLERANCE 0.00001f

#define NO_FACE (-1)
#define NO_HALFEDGE (-1)
#define NO_VERTEX (-1)

#define WALL_UNDETERMINED (-1)
#define WALL_SOLID 0
#define WALL_NONE 1
#define WALL_DOOR 2

#define WALL_LEFT 1
#define WALL_CENTER 0
#define WALL_RIGHT (-1)

#define DEFAULT_STATE 0

#define DEFAULT_THICKNESS 1.5f
#define DEFAULT_BASE 0.0f
#define DEFAULT_HEIGHT 3.0f

#define DEFAULT_WIDTH 2.0f
#define DEFAULT_OFFSET 0.0f

#define Z_ANCHOR_INNER 1
#define Z_ANCHOR_OUTER (-1)

#define DEFAULT_MARK_W 1.0f
#define DEFAULT_MARK_L 3.0f
#define DEFAULT_MARK_H 0.01f

struct MazeVertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 texcoord;
};

struct MarkVertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 texcoord;
};

class MazeFace2D;
class MazeHalfedge2D;
class MazeVertex2D;

class WallState;
class DoorWallState;
class WallStateChooser;

class Maze2D;
class MazeConverter2D;

class MazeGenerator2D;
class DFSMazeGenerator;
class BFSMazeGenerator;
class KruskalMazeGenerator;
class PrimMazeGenerator;
class ReverseDeleteMazeGenerator;
class AldousBroderMazeGenerator;

class MazeMark;
class MazeMarkList;

typedef std::shared_ptr<MazeFace2D> SharedMazeFace2D;
typedef std::shared_ptr<MazeHalfedge2D> SharedMazeHalfedge2D;
typedef std::shared_ptr<MazeVertex2D> SharedMazeVertex2D;

typedef std::shared_ptr<WallState> SharedWallState;
typedef std::shared_ptr<DoorWallState> SharedDoorWallState;
typedef std::shared_ptr<WallStateChooser> SharedWallStateChooser;

typedef std::shared_ptr<Maze2D> SharedMaze2D;
typedef std::shared_ptr<MazeConverter2D> SharedMazeConverter2D;

typedef std::shared_ptr<MazeGenerator2D> SharedMazeGenerator2D;
typedef std::shared_ptr<DFSMazeGenerator> SharedDFSMazeGenerator;
typedef std::shared_ptr<BFSMazeGenerator> SharedBFSMazeGenerator;
typedef std::shared_ptr<KruskalMazeGenerator> SharedKruskalMazeGenerator;
typedef std::shared_ptr<PrimMazeGenerator> SharedPrimMazeGenerator;
typedef std::shared_ptr<ReverseDeleteMazeGenerator> SharedReverseDeleteMazeGenerator;
typedef std::shared_ptr<AldousBroderMazeGenerator> SharedAldousBroderMazeGenerator;

typedef std::shared_ptr<MazeMark> SharedMazeMark;
typedef std::shared_ptr<MazeMarkList> SharedMazeMarkList;

// render

#define DEFAULT_PORTAL_WIDTH 2.0f
#define DEFAULT_PORTAL_HEIGHT 2.0f

#define STATE_NO_OBJECT 0
#define STATE_OBJECT_IDLE 1
#define STATE_OBJECT_BUSY 2
#define STATE_OBJECT_PENDING 3

#define DEFAULT_DEPTH 8
#define DEFAULT_FADE_DEPTH 5
#define DEFAULT_CLIPDIST 0.0001f
#define DEFAULT_WIREFRAME false
#define DEFAULT_FXAA true

#define DEFAULT_BACKGROUND_R 0.10f
#define DEFAULT_BACKGROUND_G 0.46f
#define DEFAULT_BACKGROUND_B 0.83f

#define DEFAULT_FADE_R 0.0f
#define DEFAULT_FADE_G 0.0f
#define DEFAULT_FADE_B 0.0f

#define LIGHT_DEFAULT_AMBIENT 0.5f
#define LIGHT_DEFAULT_DIRECTIONAL 0.1f

#define LIGHT_DEFAULT_INTENSITY 1.0f
#define LIGHT_DEFAULT_CUTOFF 0.0196f

#define LIGHT_DEFAULT_CONST 0.0f
#define LIGHT_DEFAULT_LIN 0.0f
#define LIGHT_DEFAULT_QUAD 1.0f

#define LIGHT_DEFAULT_GUARD 0.5f

struct PortalVertex
{
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec2 texcoord;
};

class Node;
class Leaf;
class Group;
class Root;
class TextureGroup;
class TransformGroup;
class MeshGroup;

class Light;
class GlobalLight;
class PointLight;
class SpotLight;

class Drawable;
class RenderWorld;

class Portal;

class Renderer;
class RecursivePass;

class QueryTree;
class QueryHandler;

typedef std::shared_ptr<Node> SharedNode;
typedef std::shared_ptr<Leaf> SharedLeaf;
typedef std::shared_ptr<Group> SharedGroup;
typedef std::shared_ptr<Root> SharedRoot;
typedef std::shared_ptr<TextureGroup> SharedTextureGroup;
typedef std::shared_ptr<TransformGroup> SharedTransformGroup;
typedef std::shared_ptr<MeshGroup> SharedMeshGroup;

typedef std::shared_ptr<Light> SharedLight;
typedef std::shared_ptr<GlobalLight> SharedGlobalLight;
typedef std::shared_ptr<PointLight> SharedPointLight;
typedef std::shared_ptr<SpotLight> SharedSpotLight;

typedef std::shared_ptr<Drawable> SharedDrawable;
typedef std::shared_ptr<RenderWorld> SharedRenderWorld;

typedef std::shared_ptr<Portal> SharedPortal;

typedef std::shared_ptr<Renderer> SharedRenderer;
typedef std::shared_ptr<RecursivePass> SharedRecursivePass;

typedef std::shared_ptr<QueryTree> SharedQueryTree;
typedef std::shared_ptr<QueryHandler> SharedQueryHandler;
