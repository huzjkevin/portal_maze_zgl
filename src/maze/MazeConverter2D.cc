#include "MazeConverter2D.hh"

#include "Maze2D.hh"
#include "MazeFace2D.hh"
#include "MazeHalfedge2D.hh"
#include "MazeVertex2D.hh"
#include "WallState.hh"

#include "../collision/CollisionObject.hh"
#include "../collision/CollisionWorld.hh"

#include <glow/objects/ArrayBuffer.hh>
#include <glow/objects/ElementArrayBuffer.hh>
#include <glow/objects/VertexArray.hh>

void MazeConverter2D::add(std::vector<MazeVertex>* vertices, std::vector<uint32_t>* indices)
{
    size_t offset = vertexBuffer.size();

    for (size_t i = 0; i < vertices->size(); i++)
    {
        vertexBuffer.push_back(vertices->at(i));
    }

    for (size_t j = 0; j < indices->size(); j++)
    {
        indexBuffer.push_back(indices->at(j) + offset);
    }
}

void MazeConverter2D::flush()
{
    auto ab = glow::ArrayBuffer::create();
    auto eab = glow::ElementArrayBuffer::create();

    ab->defineAttributes(
        {{&MazeVertex::pos, "aPosition"}, {&MazeVertex::normal, "aNormal"}, {&MazeVertex::tangent, "aTangent"}, {&MazeVertex::texcoord, "aTexCoord"}});

    ab->bind().setData(vertexBuffer);
    eab->bind().setIndices(indexBuffer);

    meshes->push_back(glow::VertexArray::create(ab, eab, GL_TRIANGLES));

    vertexBuffer.clear();
    indexBuffer.clear();
}

void MazeConverter2D::convert(SharedMaze2D maze)
{
    uint32_t n;

    uint32_t prev;
    uint32_t curr;
    uint32_t next;

    // collision = false;

    if(bodies) bodies->resize(maze->getHalfedgeCount());

    for (size_t i = 0; i < maze->getFaceCount(); i++)
    {
        n = maze->getFace(i)->getEdgeCount();

        for (size_t j = 0; j < n; j++)
        {
            curr = maze->getFace(i)->getEdge(j);
            prev = maze->getFace(i)->getEdge(((j + n) - 1) % n);
            next = maze->getFace(i)->getEdge(((j + n) + 1) % n);

            convertHalfedge(maze, curr, maze->getVertex(maze->getHalfedge(curr)->getStartVertex())->getLocation(),
                            maze->getVertex(maze->getHalfedge(curr)->getEndVertex())->getLocation(), maze->getInnerStart(curr, prev),
                            maze->getInnerEnd(curr, next));
        }
    }

    if (!vertexBuffer.empty() && !indexBuffer.empty())
        flush();

    glm::vec2 min = maze->getVertex(0)->getLocation();
    glm::vec2 max = maze->getVertex(0)->getLocation();

    for (size_t i = 1; i < maze->getVertexCount(); i++)
    {
        min = glm::min(min, maze->getVertex(i)->getLocation());
        max = glm::max(max, maze->getVertex(i)->getLocation());
    }

    float base = maze->getWallState(0)->getBase();
    float ceil = base + maze->getWallState(0)->getHeight();

    for (size_t i = 1; i < maze->getHalfedgeCount(); i++)
    {
        base = glm::min(base, maze->getWallState(i)->getBase());
        ceil = glm::max(ceil, maze->getWallState(i)->getBase() + maze->getWallState(i)->getHeight());
    }

    // collision = true;

    glm::vec3 tlFloor(min.x, base, min.y);
    glm::vec3 trFloor(max.x, base, min.y);
    glm::vec3 blFloor(min.x, base, max.y);
    makeRectangle(tlFloor, trFloor, blFloor);

    if (!vertexBuffer.empty() && !indexBuffer.empty())
        flush();

    glm::vec3 tlCeil(min.x, ceil, max.y);
    glm::vec3 trCeil(max.x, ceil, max.y);
    glm::vec3 blCeil(min.x, ceil, min.y);
    makeRectangle(tlCeil, trCeil, blCeil);

    if (!vertexBuffer.empty() && !indexBuffer.empty())
        flush();
}

void MazeConverter2D::convertHalfedge(SharedMaze2D maze, uint32_t halfedge, glm::vec2 start, glm::vec2 end, glm::vec2 innerStart, glm::vec2 innerEnd)
{
    glm::vec2 direction = glm::normalize(end - start);

    glm::vec2 innerStartProjected, innerEndProjected;

    float center;
    glm::vec2 innerCenter, borderCenter;
    glm::vec2 innerLeft, innerRight, borderLeft, borderRight;

    SharedWallState state = maze->getWallState(halfedge);
    float base = state->getBase(), height = state->getHeight();

    DoorWallState* door;
    std::vector<glm::vec2> face;

    switch (state->getState())
    {
    case WALL_NONE:

        innerStartProjected = start + glm::dot(innerStart - start, direction) * direction;
        innerEndProjected = end + glm::dot(innerEnd - end, -direction) * (-direction);

        makeWall(end, innerEnd, base, height);
        makeWall(innerEnd, innerEndProjected, base, height);
        makeWall(innerEndProjected, end, base, height);

        face.clear();
        face.push_back(end);
        face.push_back(innerEnd);
        face.push_back(innerEndProjected);
        rasterizeFace(&face, base + height);

        makeWall(start, innerStartProjected, base, height);
        makeWall(innerStartProjected, innerStart, base, height);
        makeWall(innerStart, start, base, height);

        face.clear();
        face.push_back(start);
        face.push_back(innerStartProjected);
        face.push_back(innerStart);
        rasterizeFace(&face, base + height);

        break;

    case WALL_SOLID:

        makeWall(start, end, base, height);
        makeWall(end, innerEnd, base, height);
        makeWall(innerEnd, innerStart, base, height, halfedge);
        makeWall(innerStart, start, base, height);

        face.clear();
        face.push_back(start);
        face.push_back(end);
        face.push_back(innerEnd);
        face.push_back(innerStart);
        rasterizeFace(&face, base + height);

        break;

    case WALL_DOOR:

        door = (DoorWallState*)state.get();

        center = getCenter(maze, halfedge, door->getAnchor(), door->getOffset(), door->getWidth());
        innerCenter = innerStart + (center - maze->projectInnerStart(halfedge)) * direction;
        borderCenter = start + center * direction;

        innerLeft = innerCenter + (0.5f * door->getWidth()) * direction;
        innerRight = innerCenter - (0.5f * door->getWidth()) * direction;
        borderLeft = borderCenter + (0.5f * door->getWidth()) * direction;
        borderRight = borderCenter - (0.5f * door->getWidth()) * direction;

        makeWall(end, innerEnd, base, height);
        if (innerEnd != innerLeft)
            makeWall(innerEnd, innerLeft, base, height);
        makeWall(innerLeft, borderLeft, base, height);
        makeWall(borderLeft, end, base, height);

        face.clear();
        face.push_back(end);
        face.push_back(innerEnd);
        if (innerEnd != innerLeft)
            face.push_back(innerLeft);
        face.push_back(borderLeft);
        rasterizeFace(&face, base + height);

        if (innerStart != innerRight)
            makeWall(innerRight, innerStart, base, height);
        makeWall(innerStart, start, base, height);
        makeWall(start, borderRight, base, height);
        makeWall(borderRight, innerRight, base, height);

        face.clear();
        if (innerStart != innerRight)
            face.push_back(innerRight);
        face.push_back(innerStart);
        face.push_back(start);
        face.push_back(borderRight);
        rasterizeFace(&face, base + height);

        break;

    default:

        // unknown wall state

        break;
    }
}

float MazeConverter2D::getCenter(SharedMaze2D maze, uint32_t halfedge, int anchor, float offset, float width) const
{
    float min = maze->getRightmost(halfedge);
    float max = maze->getLeftmost(halfedge);
    float center;
    if (anchor < 0)
    {
        center = min + width / 2.0f;
    }
    else if (anchor > 0)
    {
        center = max - width / 2.0f;
    }
    else
    {
        center = (min + max) / 2.0f;
    }
    center += offset;
    return center;
}

void MazeConverter2D::makeWall(glm::vec2 left, glm::vec2 right, float base, float height, int32_t halfedge)
{
    glm::vec3 tl, tr, bl;

    tl.x = left.x;
    tl.y = base + height;
    tl.z = left.y;
    tr.x = right.x;
    tr.y = base + height;
    tr.z = right.y;
    bl.x = left.x;
    bl.y = base;
    bl.z = left.y;

    makeRectangle(tl, tr, bl, halfedge);
}

void MazeConverter2D::makeRectangle(glm::vec3 tl, glm::vec3 tr, glm::vec3 bl, int32_t halfedge, float texWidth, float texHeight, float texOffX, float texOffY)
{
    /*auto ab = glow::ArrayBuffer::create();
    auto eab = glow::ElementArrayBuffer::create();

    ab->defineAttributes(
        {{&MazeVertex::pos, "aPosition"}, {&MazeVertex::normal, "aNormal"}, {&MazeVertex::tangent, "aTangent"}, {&MazeVertex::texcoord, "aTexCoord"}});*/

    glm::vec3 pos;
    glm::vec3 normal = glm::normalize(glm::cross(tr - tl, tl - bl));
    glm::vec3 tangent = glm::normalize(tr - tl);
    glm::vec2 texcoord;

    float width = glm::distance(tl, tr);
    float height = glm::distance(tl, bl);

    glm::vec3 right = glm::normalize(tr - tl);
    glm::vec3 down = glm::normalize(bl - tl);

    bool mirrorX = false, mirrorY = false;
    int sampleWidth, sampleHeight;

    float firstTexWidth = (1 - texOffX) * texWidth;
    float firstTexHeight = (1 - texOffY) * texHeight;

    float lastTexWidth, lastTexHeight;

    if (firstTexWidth >= width)
    {
        sampleWidth = 2;
        firstTexWidth = width;
        lastTexWidth = firstTexWidth;
    }
    else
    {
        sampleWidth = std::lround(std::ceil((width - firstTexWidth) / texWidth)) + 2;
        lastTexWidth = width - firstTexWidth - (sampleWidth - 3) * texWidth;
    }

    if (firstTexHeight >= height)
    {
        sampleHeight = 2;
        firstTexHeight = height;
        lastTexHeight = firstTexHeight;
    }
    else
    {
        sampleHeight = std::lround(std::ceil((height - firstTexHeight) / texHeight)) + 2;
        lastTexHeight = height - firstTexHeight - (sampleHeight - 3) * texHeight;
    }

    float temp;

    std::vector<MazeVertex> vertices(sampleWidth * sampleHeight);
    std::vector<uint32_t> indices(3 * 2 * (sampleWidth - 1) * (sampleHeight - 1));

    pos = tl;
    texcoord.x = texOffX;
    texcoord.y = texOffY;
    vertices[0] = {pos, normal, tangent, texcoord};

    for (int y = 1; y < sampleHeight; y++)
    {
        if (y == sampleHeight - 1)
        {
            pos = bl;
            texcoord.y = lastTexHeight / texHeight;
            texcoord.y = mirrorY ? 1.0f - texcoord.y : texcoord.y;
        }
        else
        {
            pos = tl + (firstTexHeight + (y - 1) * texHeight) * down;
            texcoord.y = mirrorY ? 0.0f : 1.0f;
        }

        texcoord.x = texOffX;
        vertices[y * sampleWidth] = {pos, normal, tangent, texcoord};

        mirrorX = false;

        for (int x = 1; x < sampleWidth; x++)
        {
            if (x == sampleWidth - 1)
            {
                texcoord.x = lastTexWidth / texWidth;
                texcoord.x = mirrorX ? 1.0f - texcoord.x : texcoord.x;
            }
            else
            {
                texcoord.x = mirrorX ? 0.0f : 1.0f;
            }

            if (y == 1)
            {
                pos = (x == sampleWidth - 1) ? tr : tl + (firstTexWidth + (x - 1) * texWidth) * right;
                temp = texcoord.y;
                texcoord.y = texOffY;
                vertices[x] = {pos, normal, tangent, texcoord};
                texcoord.y = temp;
            }

            if (x == sampleWidth - 1)
            {
                pos = tr + (firstTexHeight + (y - 1) * texHeight) * down;
            }
            else
            {
                pos = tl + (firstTexWidth + (x - 1) * texWidth) * right + (firstTexHeight + (y - 1) * texHeight) * down;
            }

            vertices[y * sampleWidth + x] = {pos, normal, tangent, texcoord};

            indices[6 * ((y - 1) * (sampleWidth - 1) + (x - 1))] = (y - 1) * sampleWidth + (x - 1);
            indices[6 * ((y - 1) * (sampleWidth - 1) + (x - 1)) + 1] = y * sampleWidth + (x - 1);
            indices[6 * ((y - 1) * (sampleWidth - 1) + (x - 1)) + 2] = y * sampleWidth + x;

            indices[6 * ((y - 1) * (sampleWidth - 1) + (x - 1)) + 3] = y * sampleWidth + x;
            indices[6 * ((y - 1) * (sampleWidth - 1) + (x - 1)) + 4] = (y - 1) * sampleWidth + x;
            indices[6 * ((y - 1) * (sampleWidth - 1) + (x - 1)) + 5] = (y - 1) * sampleWidth + (x - 1);

            mirrorX = !mirrorX;
        }

        mirrorY = !mirrorY;
    }

    add(&vertices, &indices);

    // collision = false;

    if (!collision)
        return;

    // collision = false;

    glm::vec3 translation = tl + 0.5f * width * right + 0.5f * height * down;
    glm::vec3 xAxis = right;
    glm::vec3 yAxis = -down;
    glm::vec3 zAxis = normal;

    glm::mat4 model(xAxis.x, xAxis.y, xAxis.z, 0.0f, yAxis.x, yAxis.y, yAxis.z, 0.0f, zAxis.x, zAxis.y, zAxis.z, 0.0f, translation.x, translation.y,
                    translation.z, 1.0f);

    SharedCollisionObject collision = std::make_shared<CollisionObject>();
    collision->initStatic(new btBoxShape(btVector3(width / 2.0f, height / 2.0f, 0.1f)), model);

    CollisionWorld::getInstance()->addObject(collision);

    if (bodies && halfedge >= 0)
    {
        (*bodies)[halfedge] = collision.get();
    }

    /*btCollisionShape* collisionShape = new btBoxShape(btVector3(width / 2.0f, height / 2.0f, 0.1f));
    btDefaultMotionState* motionState = new btDefaultMotionState(to_bullet(model));
    btRigidBody* rigidBodyRaw = new btRigidBody(0.0f, motionState, collisionShape);
    std::shared_ptr<btRigidBody> rigidBody(rigidBodyRaw);
    // std::shared_ptr<btRigidBody> rigidBody = std::make_shared<btRigidBody>(0.0f, motionState, collisionShape);
    // create collision info
    // mRigidBody->setUserPointer(&mCollisionInfo);
    // setCollisionObjType(objTag);

    SharedBulletAdapter adapter = std::make_shared<BulletAdapter>(collisionShape, motionState, rigidBody);
    rigidBody->setUserPointer(adapter->getInfo());
    adapter->getInfo()->mObjTag = zgl::CollisionInfo::ObjTag::Wall;
    BulletAdapter::objects.push_back(adapter);

    world->addToPhysicalWorld(rigidBody);

    if (bodies && halfedge >= 0)
    {
        (*bodies)[halfedge] = rigidBodyRaw;
    }*/

    /*ab->bind().setData(vertices);
    eab->bind().setIndices(indices);

    meshes->push_back(glow::VertexArray::create(ab, eab, GL_TRIANGLES));*/
}

void MazeConverter2D::rasterizeFace(std::vector<glm::vec2>* face, float y, glm::vec3 normal, glm::vec3 tangent, float texWidth, float texHeight)
{
    uint32_t n = face->size();

    glm::vec2 min = (*face)[0];
    glm::vec2 max = (*face)[0];

    for (uint32_t i = 1; i < n; i++)
    {
        min = glm::min(min, (*face)[i]);
        max = glm::max(max, (*face)[i]);
    }

    float width = max.x - min.x;
    float height = max.y - min.y;

    glm::vec3 pos;
    glm::vec2 texcoord;

    std::vector<MazeVertex> vertices(n);
    std::vector<uint32_t> indices((n - 2) * 3);

    for (uint32_t i = 0; i < n; i++)
    {
        pos.x = (*face)[i].x;
        pos.y = y;
        pos.z = (*face)[i].y;

        texcoord.x = ((*face)[i].x - min.x) / width;
        texcoord.y = ((*face)[i].y - min.y) / height;

        vertices[i] = {pos, normal, tangent, texcoord};
        if (i > 1)
        {
            indices[3 * (i - 2)] = 0;
            indices[3 * (i - 2) + 1] = (i - 1);
            indices[3 * (i - 2) + 2] = i;
        }
    }

    add(&vertices, &indices);
}
