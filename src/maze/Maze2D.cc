#include "Maze2D.hh"

#include "MazeFace2D.hh"
#include "MazeHalfedge2D.hh"
#include "MazeVertex2D.hh"
#include "WallState.hh"

Maze2D::Maze2D() : vertices(), faces(), halfedges(), walls(), initial(NO_FACE), corridor(new WallStateChooser()), barrier(new WallStateChooser()) {
    
}

Maze2D::Maze2D(size_t v) : vertices(), faces(), halfedges(), walls(), initial(NO_FACE), corridor(new WallStateChooser()), barrier(new WallStateChooser()) {
    vertices.reserve(v);
}

Maze2D::Maze2D(size_t v, size_t f) : vertices(), faces(), halfedges(), walls(), initial(NO_FACE), corridor(new WallStateChooser()), barrier(new WallStateChooser()) {
    vertices.reserve(v);
    faces.reserve(f);
}

Maze2D::Maze2D(size_t v, size_t f, size_t h) : vertices(), faces(), halfedges(), walls(), initial(NO_FACE), corridor(new WallStateChooser()), barrier(new WallStateChooser()) {
    vertices.reserve(v);
    faces.reserve(f);
    halfedges.reserve(h);
    walls.reserve(h);
}

Maze2D::~Maze2D() {

}

size_t Maze2D::getVertexCount() const {
    return vertices.size();
}

SharedMazeVertex2D Maze2D::getVertex(size_t index) const {
    return vertices[index];
}

int32_t Maze2D::addVertex(SharedMazeVertex2D vertex) {
    vertices.push_back(vertex);
    return ((int32_t)(vertices.size() - 1));
}

int32_t Maze2D::insertVertex(size_t index, SharedMazeVertex2D vertex) {
    vertices.insert(vertices.begin() + index, vertex);
    return ((int32_t)index);
}

void Maze2D::removeVertex(size_t index) {
    vertices.erase(vertices.begin() + index);
}

void Maze2D::clearVertices() {
    vertices.clear();
}

size_t Maze2D::getFaceCount() const {
    return faces.size();
}

SharedMazeFace2D Maze2D::getFace(size_t index) const {
    return faces[index];
}

int32_t Maze2D::addFace(SharedMazeFace2D face) {
    faces.push_back(face);
    return ((int32_t)(faces.size() - 1));
}

int32_t Maze2D::insertFace(size_t index, SharedMazeFace2D face) {
    faces.insert(faces.begin() + index, face);
    return ((int32_t)index);
}

void Maze2D::removeFace(size_t index) {
    faces.erase(faces.begin() + index);
}

void Maze2D::clearFaces() {
    faces.clear();
}

size_t Maze2D::getHalfedgeCount() const {
    return halfedges.size();
}

SharedMazeHalfedge2D Maze2D::getHalfedge(size_t index) const {
    return halfedges[index];
}

int32_t Maze2D::addHalfedge(SharedMazeHalfedge2D edge) {
    halfedges.push_back(edge);
    walls.push_back(std::make_shared<WallState>());
    return ((int32_t)(halfedges.size() - 1));
}

int32_t Maze2D::insertHalfedge(size_t index, SharedMazeHalfedge2D edge) {
    halfedges.insert(halfedges.begin() + index, edge);
    walls.insert(walls.begin() + index, std::make_shared<WallState>());
    return ((int32_t)index);
}

void Maze2D::removeHalfedge(size_t index) {
    halfedges.erase(halfedges.begin() + index);
    walls.erase(walls.begin() + index);
}

void Maze2D::clearHalfedges() {
    halfedges.clear();
    walls.clear();
}

SharedWallState Maze2D::getWallState(size_t index) const {
    return walls[index];
}

void Maze2D::setWallState(size_t index, SharedWallState state, bool invert) {
    walls[index] = state;
    int32_t opposite = halfedges[index]->getOpposite();
    if (invert && opposite != NO_HALFEDGE)
    {
        walls[opposite] = state->invert();
    }
}

int32_t Maze2D::getInitial() const {
    return initial;
}

void Maze2D::setInitial(int32_t init) {
    initial = init;
}

SharedWallStateChooser Maze2D::getBarrierChooser() {
    return barrier;
}

SharedWallStateChooser Maze2D::getCorridorChooser() {
    return corridor;
}

void Maze2D::setBarrierChooser(SharedWallStateChooser bar) {
    barrier = bar;
}

void Maze2D::setCorridorChooser(SharedWallStateChooser cor) {
    corridor = cor;
}

int32_t Maze2D::createVertex(glm::vec2 location) {
    return addVertex(std::make_shared<MazeVertex2D>(location));
}

int32_t Maze2D::createFace(std::vector<int32_t> points) {
    SharedMazeFace2D face = std::make_shared<MazeFace2D>();
    int32_t index = addFace(face);

    int32_t halfedge;
    size_t j;

    for (size_t i = 0; i < points.size(); i++)
    {
        j = (i + 1) % points.size();
        halfedge = createHalfedge(points[i], points[j], index);

        face->addVertex(points[i]);
        face->addEdge(halfedge);

        vertices[points[i]]->addFace(index);
        vertices[points[j]]->addFace(index);

        vertices[points[i]]->addOutgoingEdge(halfedge);
        vertices[points[j]]->addIncomingEdge(halfedge);
    }

    return index;
}

int32_t Maze2D::createHalfedge(int32_t start, int32_t end, int32_t face, int32_t opposite) {
    return addHalfedge(std::make_shared<MazeHalfedge2D>(start, end, face, opposite));
}

void Maze2D::connectHalfedge(int32_t halfedge, int32_t opposite) {
    halfedges[halfedge]->setOpposite(opposite);
    halfedges[opposite]->setOpposite(halfedge);
}

void Maze2D::connect() {
    int32_t in;
    int32_t out;

    for (size_t i = 0; i < vertices.size(); i++)
    {
        for (size_t k = 0; k < vertices[i]->getIncomingCount(); k++)
        {
            for (size_t l = 0; l < vertices[i]->getOutgoingCount(); l++)
            {
                in = vertices[i]->getIncomingEdge(k);
                out = vertices[i]->getOutgoingEdge(l);

                if ((halfedges[in]->getStartVertex()) == (halfedges[out]->getEndVertex()))
                {
                    connectHalfedge(in, out);
                }
            }
        }
    }
}

glm::vec2 Maze2D::getDirection(size_t halfedge) const {
    return (vertices[halfedges[halfedge]->getEndVertex()]->getLocation()) - (vertices[halfedges[halfedge]->getStartVertex()]->getLocation());
}

glm::vec2 Maze2D::inverseDirection(size_t halfedge) const {
    return (vertices[halfedges[halfedge]->getStartVertex()]->getLocation()) - (vertices[halfedges[halfedge]->getEndVertex()]->getLocation());
}

float Maze2D::getCosAngle(size_t first, size_t second) const {
    glm::vec2 dir1 = glm::normalize(-getDirection(first));
    glm::vec2 dir2 = glm::normalize(getDirection(second));
    return glm::dot(dir1, dir2);
}

float Maze2D::getSinAngle(size_t first, size_t second) const {
    float cos = getCosAngle(first, second);
    return glm::sqrt(1 - cos * cos);
}

float Maze2D::getAngle(size_t first, size_t second) const {
    return glm::acos(getCosAngle(first, second));
}

glm::vec2 Maze2D::getInnerStart(size_t halfedge, size_t prev) const
{
    glm::vec2 start = vertices[halfedges[halfedge]->getStartVertex()]->getLocation();
    return start + ((walls[halfedge]->getThickness()) / glm::sin(getAngle(prev, halfedge) / 2)) * glm::normalize(getDirection(halfedge) - getDirection(prev));
}

glm::vec2 Maze2D::getInnerEnd(size_t halfedge, size_t next) const
{
    glm::vec2 end = vertices[halfedges[halfedge]->getEndVertex()]->getLocation();
    return end + ((walls[halfedge]->getThickness()) / glm::sin(getAngle(halfedge, next) / 2)) * glm::normalize(getDirection(next) - getDirection(halfedge));
}

glm::vec2 Maze2D::getInnerStart(size_t halfedge) const
{
    size_t face = (size_t)halfedges[halfedge]->getFace();

    uint32_t n = faces[face]->getEdgeCount();
    size_t prev = NO_HALFEDGE;

    for (size_t i = 0; i < n; i++)
    {
        if (faces[face]->getEdge(i) == halfedge)
        {
            prev = faces[face]->getEdge((i + n - 1) % n);
        }
    }

    return getInnerStart(halfedge, prev);
}

glm::vec2 Maze2D::getInnerEnd(size_t halfedge) const
{
    size_t face = (size_t)halfedges[halfedge]->getFace();

    uint32_t n = faces[face]->getEdgeCount();
    size_t next = NO_HALFEDGE;

    for (size_t i = 0; i < n; i++)
    {
        if (faces[face]->getEdge(i) == halfedge)
        {
            next = faces[face]->getEdge((i + 1) % n);
        }
    }

    return getInnerEnd(halfedge, next);
}

float Maze2D::projectInnerStart(size_t halfedge) const
{
    glm::vec2 start = vertices[halfedges[halfedge]->getStartVertex()]->getLocation();
    glm::vec2 inner = getInnerStart(halfedge);

    return glm::dot(inner - start, glm::normalize(getDirection(halfedge)));
}

float Maze2D::projectInnerEnd(size_t halfedge) const
{
    glm::vec2 end = vertices[halfedges[halfedge]->getEndVertex()]->getLocation();
    glm::vec2 inner = getInnerEnd(halfedge);

    return glm::dot(inner - end, glm::normalize(inverseDirection(halfedge)));
}

float Maze2D::getLeftmost(size_t halfedge) const
{
    float own = projectInnerEnd(halfedge);
    float opposite = halfedges[halfedge]->getOpposite() == NO_HALFEDGE ? 0.0f : projectInnerStart(halfedges[halfedge]->getOpposite());

    return glm::length(getDirection(halfedge)) - (own < opposite ? opposite : own);
}

float Maze2D::getRightmost(size_t halfedge) const
{
    float own = projectInnerStart(halfedge);
    float opposite = halfedges[halfedge]->getOpposite() == NO_HALFEDGE ? 0.0f : projectInnerEnd(halfedges[halfedge]->getOpposite());

    return (own < opposite ? opposite : own);
}

bool Maze2D::isCorridor(size_t halfedge, bool opposite) const
{
    int32_t own = walls[halfedge]->getState();
    if (own == WALL_NONE || own == WALL_DOOR)
    {
        int32_t reverse = halfedges[halfedge]->getOpposite();
        if (opposite && reverse != NO_HALFEDGE)
        {
            int32_t other = walls[reverse]->getState();
            return (other == WALL_NONE || other == WALL_DOOR);
        }
        else
        {
            return true;
        }
    }
    else
    {
        return false;
    }
}

bool Maze2D::isBarrier(size_t halfedge, bool opposite) const
{
    int32_t own = walls[halfedge]->getState();
    if (own == WALL_SOLID)
    {
        return true;
    }
    else
    {
        int32_t reverse = halfedges[halfedge]->getOpposite();
        if (opposite && reverse != NO_HALFEDGE)
        {
            int32_t other = walls[reverse]->getState();
            return (other == WALL_SOLID);
        }
        else
        {
            return false;
        }
    }
}

void Maze2D::makeBarrier(size_t halfedge, bool opposite, bool force)
{
    SharedWallState state = barrier->choose();
    walls[halfedge] = state;
    int32_t reverse = halfedges[halfedge]->getOpposite();
    if (opposite && reverse != NO_HALFEDGE)
    {
        if (force || walls[reverse]->getState() == WALL_UNDETERMINED)
            walls[reverse] = state->invert();
    }
}

void Maze2D::makeCorridor(size_t halfedge, bool opposite, bool force)
{
    SharedWallState state = corridor->choose();
    walls[halfedge] = state;
    int32_t reverse = halfedges[halfedge]->getOpposite();
    if (opposite && reverse != NO_HALFEDGE)
    {
        if (force || walls[reverse]->getState() == WALL_UNDETERMINED)
            walls[reverse] = state->invert();
    }
}

void Maze2D::applyWallState(SharedWallStateChooser chooser, int mode)
{
    size_t n = getHalfedgeCount();
    for (size_t i = 0; i < n; i++)
    {
        bool boundary = halfedges[i]->isBoundary();
        if ((boundary && mode <= 0) || (!boundary && mode >= 0))
        {
            walls[i] = chooser->choose();
        }
    }
}

void Maze2D::resolveUndeterminedWalls(SharedWallStateChooser chooser, bool twoside)
{
    size_t n = getHalfedgeCount();
    SharedWallState state;
    int32_t opposite;
    for (size_t i = 0; i < n; i++)
    {
        if (walls[i]->getState() == WALL_UNDETERMINED)
        {
            state = chooser->choose();
            walls[i] = state;
            opposite = halfedges[i]->getOpposite();
            if (twoside && opposite != NO_HALFEDGE && walls[opposite]->getState() == WALL_UNDETERMINED)
            {
                walls[opposite] = state->invert();
            }
        }
    }
}

void Maze2D::setWallBase(float base)
{
    size_t n = getHalfedgeCount();
    for (size_t i = 0; i < n; i++)
    {
        walls[i]->setBase(base);
    }
}

void Maze2D::setWallHeight(float height)
{
    size_t n = getHalfedgeCount();
    for (size_t i = 0; i < n; i++)
    {
        walls[i]->setHeight(height);
    }
}

void Maze2D::setWallThickness(float thickness)
{
    size_t n = getHalfedgeCount();
    for (size_t i = 0; i < n; i++)
    {
        walls[i]->setThickness(thickness);
    }
}

glm::vec3 Maze2D::getLeftDirection(size_t halfedge) const
{
    glm::vec2 dir = getDirection(halfedge);
    glm::vec3 left;
    left.x = dir.x;
    left.y = 0.0f;
    left.z = dir.y;
    return glm::normalize(left);
}

glm::vec3 Maze2D::getRightDirection(size_t halfedge) const
{
    glm::vec2 dir = inverseDirection(halfedge);
    glm::vec3 right;
    right.x = dir.x;
    right.y = 0.0f;
    right.z = dir.y;
    return glm::normalize(right);
}

glm::vec3 Maze2D::getUpDirection(size_t halfedge) const
{
    return glm::vec3(0.0f, 1.0f, 0.0f);
}

glm::vec3 Maze2D::getDownDirection(size_t halfedge) const
{
    return glm::vec3(0.0f, -1.0f, 0.0f);
}

glm::vec3 Maze2D::getInDirection(size_t halfedge) const
{
    return glm::cross(getUpDirection(halfedge), getLeftDirection(halfedge));
}

glm::vec3 Maze2D::getOutDirection(size_t halfedge) const
{
    return glm::cross(getUpDirection(halfedge), getRightDirection(halfedge));
}

glm::mat4 Maze2D::attachRectangle(size_t halfedge, float width, float height, float sgn, int anchor, float offset, int vAnchor, float vOffset, float guard, float mirror, int zAnchor, float zOffset) const
{
    glm::vec2 direction = glm::normalize(getDirection(halfedge));

    glm::vec2 ref;

    if (anchor < 0)
    {
        ref = getInnerStart(halfedge) + (width / 2) * direction;
    }
    else if (anchor > 0)
    {
        ref = getInnerEnd(halfedge) - (width / 2) * direction;
    }
    else
    {
        ref = 0.5f * (getInnerStart(halfedge) + getInnerEnd(halfedge));
    }

    ref += offset * direction;

    float y;

    if (vAnchor < 0)
    {
        y = walls[halfedge]->getBase() + (height / 2);
    }
    else if (vAnchor > 0)
    {
        y = walls[halfedge]->getBase() + walls[halfedge]->getHeight() - (height / 2);
    }
    else
    {
        y = walls[halfedge]->getBase() + (walls[halfedge]->getHeight() / 2);
    }

    y += vOffset;

    glm::vec3 pos(ref.x, y, ref.y);
    glm::vec3 right = sgn * mirror * getRightDirection(halfedge);
    glm::vec3 up = getUpDirection(halfedge);
    glm::vec3 target = sgn * getOutDirection(halfedge);

    //pos -= guard * sgn * target;
    pos -= guard * target;

    if (zAnchor < 0)
    {
        pos += walls[halfedge]->getThickness() * getOutDirection(halfedge);
    }
    pos -= zOffset * getOutDirection(halfedge);

    glm::mat4 model(right.x, right.y, right.z, 0.0f, up.x, up.y, up.z, 0.0f, target.x, target.y, target.z, 0.0f, pos.x, pos.y, pos.z, 1.0f);

    return model;
}

int32_t Maze2D::findVertex(glm::vec2 location, float tolerance) const
{
    for (size_t i = 0; i < vertices.size(); i++)
    {
        if (glm::distance(location, vertices[i]->getLocation()) < tolerance)
        {
            return (int32_t)i;
        }
    }
    return NO_VERTEX;
}

int32_t Maze2D::findHalfedge(int32_t start, int32_t end) const
{
    for (size_t i = 0; i < halfedges.size(); i++)
    {
        if (halfedges[i]->getStartVertex() == start && halfedges[i]->getEndVertex() == end)
        {
            return (int32_t)i;
        }
    }
    return NO_HALFEDGE;
}

int32_t Maze2D::findFace(glm::vec2 location) const
{
    size_t n;
    bool match;
    for (size_t i = 0; i < faces.size(); i++)
    {
        n = faces[i]->getEdgeCount();
        match = true;
        for (size_t j = 0; j < n; j++)
        {
            glm::vec2 from = vertices[halfedges[faces[i]->getEdge(j)]->getStartVertex()]->getLocation();
            glm::vec2 to = vertices[halfedges[faces[i]->getEdge(j)]->getEndVertex()]->getLocation();
            if ((to.x - from.x) * (location.y - from.y) >= (to.y - from.y) * (location.x - from.x))
            {
                match = false;
                break;
            }
        }
        if (match)
        {
            return (int32_t)i;
        }
    }
    return NO_FACE;
}

glm::vec3 Maze2D::getCenter(size_t face) const
{
    size_t n = 0;

    float base = 0.0f;
    glm::vec2 sum(0.0f, 0.0f);

    for (; n < faces[face]->getEdgeCount(); n++)
    {
        sum += vertices[halfedges[faces[face]->getEdge(n)]->getStartVertex()]->getLocation();
        base += walls[faces[face]->getEdge(n)]->getBase();
    }

    sum = (1.0f / (float)n) * sum;
    base = (1.0f / (float)n) * base;

    return glm::vec3(sum.x, base, sum.y);
}

uint32_t Maze2D::lookAtHalfedge(size_t face, glm::vec2 location, glm::vec2 direction) const
{
    const SharedMazeFace2D handle = faces[face];
    size_t n = handle->getEdgeCount();
    for (size_t i = 0; i < n; i++)
    {
        glm::vec2 start = getInnerStart(handle->getEdge(i), handle->getEdge((n + i - 1) % n));
        glm::vec2 segment = getInnerEnd(handle->getEdge(i), handle->getEdge((n + i + 1) % n)) - start;
        float det = segment.x * direction.y - segment.y * direction.x;
        if (det != 0.0f)
        {
            float param = (direction.x * (start.y - location.y) + direction.y * (location.x - start.x)) / det;
            float lambda = (segment.x * (location.y - start.y) + segment.y * (start.x - location.x)) / (-det);
            if (0.0f <= param && param <= 1.0f && lambda >= 0.0f)
            {
                return i;
            }
        }
    }
    return n;
}

glm::vec3 Maze2D::directionToHalfedge(size_t face, size_t halfedge) const
{
    const SharedMazeFace2D handle = faces[face];
    size_t n = handle->getEdgeCount();
    glm::vec2 start = getInnerStart(handle->getEdge(halfedge), handle->getEdge((n + halfedge - 1) % n));
    glm::vec2 end = getInnerEnd(handle->getEdge(halfedge), handle->getEdge((n + halfedge + 1) % n));
    glm::vec2 middle = 0.5f * start + 0.5f * end;
    
    glm::vec3 from = getCenter(face);
    glm::vec3 to(middle.x, walls[handle->getEdge(halfedge)]->getBase(), middle.y);
    return glm::normalize(to - from);
}

SharedMaze2D Maze2D::createGrid(uint32_t xTiles, uint32_t zTiles, float xTileWidth, float zTileWidth, float xCenter, float zCenter)
{
    SharedMaze2D maze = std::make_shared<Maze2D>((xTiles + 1) * (zTiles + 1), xTiles * zTiles, 4 * xTiles * zTiles);

    glm::vec2 location;
    float xLeft = xCenter - (xTiles * xTileWidth) / 2.0f;
    float zBehind = zCenter - (zTiles * zTileWidth) / 2.0f;

    std::vector<int32_t> points;

    for (uint32_t j = 0; j <= zTiles; j++)
    {
        for (uint32_t i = 0; i <= xTiles; i++)
        {
            location.x = xLeft + i * xTileWidth;
            location.y = zBehind + j * zTileWidth;
            maze->createVertex(location);
        }
    }

    for (uint32_t j = 0; j < zTiles; j++)
    {
        for (uint32_t i = 0; i < xTiles; i++)
        {
            points.clear();
            points.push_back(j * (xTiles + 1) + i);
            points.push_back((j + 1) * (xTiles + 1) + i);
            points.push_back((j + 1) * (xTiles + 1) + i + 1);
            points.push_back(j * (xTiles + 1) + i + 1);
            maze->createFace(points);
        }
    }

    maze->connect();
    return maze;
}

SharedMaze2D Maze2D::createHexMaze(uint32_t xTiles, uint32_t zTiles, float xWidth, float zWidth, float xCenter, float zCenter)
{
    SharedMaze2D maze = std::make_shared<Maze2D>(4 * xTiles * zTiles + 2 * xTiles, xTiles * zTiles + (xTiles - 1) * (zTiles - 1),
                                                 6 * xTiles * zTiles + (xTiles - 1) * (zTiles - 1));

    float sideWidth = zWidth / 2;
    float fullWidth = 2 * sideWidth + xWidth;
    float halfWidth = glm::sqrt(3.0f) * zWidth / 2.0f;

    glm::vec2 location;
    float xLeft = xCenter - (xTiles * fullWidth + (xTiles - 1) * xWidth) / 2.0f;
    float zBehind = zCenter - (zTiles * halfWidth);

    std::vector<int32_t> points;

    for (uint32_t j = 0; j <= zTiles; j++)
    {
        for (uint32_t i = 0; i < xTiles; i++)
        {
            if (j < zTiles)
            {
                location.x = xLeft + i * (fullWidth + xWidth);
                location.y = zBehind + (2 * j + 1) * halfWidth;
                maze->createVertex(location);
                location.x = xLeft + i * (fullWidth + xWidth) + fullWidth;
                location.y = zBehind + (2 * j + 1) * halfWidth;
                maze->createVertex(location);
            }
            location.x = xLeft + i * (fullWidth + xWidth) + sideWidth;
            location.y = zBehind + (2 * j) * halfWidth;
            maze->createVertex(location);
            location.x = xLeft + i * (fullWidth + xWidth) + sideWidth + xWidth;
            location.y = zBehind + (2 * j) * halfWidth;
            maze->createVertex(location);
        }
    }

    for (uint32_t j = 0; j < zTiles; j++)
    {
        for (uint32_t i = 0; i < xTiles; i++)
        {
            points.clear();
            points.push_back(j * 4 * xTiles + i * 4);
            points.push_back((j + 1) * 4 * xTiles + i * (j + 1 < zTiles ? 4 : 2) + (j + 1 < zTiles ? 2 : 0));
            points.push_back((j + 1) * 4 * xTiles + i * (j + 1 < zTiles ? 4 : 2) + (j + 1 < zTiles ? 3 : 1));
            points.push_back(j * 4 * xTiles + i * 4 + 1);
            points.push_back(j * 4 * xTiles + i * 4 + 3);
            points.push_back(j * 4 * xTiles + i * 4 + 2);
            maze->createFace(points);
            if (j < zTiles - 1 && i < xTiles - 1)
            {
                points.clear();
                points.push_back((j + 1) * 4 * xTiles + i * 4 + 3);
                points.push_back((j + 1) * 4 * xTiles + i * 4 + 1);
                points.push_back((j + 1) * 4 * xTiles + (i + 1) * 4);
                points.push_back((j + 1) * 4 * xTiles + (i + 1) * 4 + 2);
                points.push_back(j * 4 * xTiles + (i + 1) * 4);
                points.push_back(j * 4 * xTiles + i * 4 + 1);
                maze->createFace(points);
            }
        }
    }

    maze->connect();
    return maze;
}
