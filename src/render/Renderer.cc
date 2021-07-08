#include "Renderer.hh"

#include "Portal.hh"
#include "QueryTree.hh"
#include "RecursivePass.hh"
#include "RenderWorld.hh"

#include "../logic/CameraObject.hh"
#include "../logic/LogicWorld.hh"

#include "../Stats.hh"

#include <glow/objects/ArrayBuffer.hh>
#include <glow/objects/Framebuffer.hh>
#include <glow/objects/Program.hh>
#include <glow/objects/TextureRectangle.hh>
#include <glow/objects/VertexArray.hh>

#include <stack>

SharedRenderer Renderer::instance;

void Renderer::initInstance()
{
    instance = std::make_shared<Renderer>();
    instance->init();
}

void Renderer::init()
{
    targets.push_back(positionBuffer = glow::TextureRectangle::create(1, 1, GL_RGB16F));
    targets.push_back(normalBuffer = glow::TextureRectangle::create(1, 1, GL_RGB16F));
    targets.push_back(albedoBuffer = glow::TextureRectangle::create(1, 1, GL_RGB16F));
    targets.push_back(depthBuffer = glow::TextureRectangle::create(1, 1, GL_DEPTH32F_STENCIL8));
    targets.push_back(colorBuffer = glow::TextureRectangle::create(1, 1, GL_RGB16F));

    {
        geometryBuffer = glow::Framebuffer::create();
        auto fb = geometryBuffer->bind();
        fb.attachColor("fPosition", positionBuffer);
        fb.attachColor("fNormal", normalBuffer);
        fb.attachColor("fAlbedo", albedoBuffer);
        fb.attachDepthStencil(depthBuffer);
    }

    {
        lightBuffer = glow::Framebuffer::create();
        auto fb = lightBuffer->bind();
        fb.attachColor("fColor", colorBuffer);
        fb.attachDepthStencil(depthBuffer);
    }

    worldShader = glow::Program::createFromFile("../data/shaders/world");
    constShader = glow::Program::createFromFile("../data/shaders/const");
    depthShader = glow::Program::createFromFile("../data/shaders/depth");
    postShader = glow::Program::createFromFile("../data/shaders/post");
    fxaaShader = glow::Program::createFromFile("../data/shaders/fxaa");
    lightShader = glow::Program::createFromFile("../data/shaders/light");
    globalLightShader = glow::Program::createFromFile("../data/shaders/light_global");
    pointLightShader = glow::Program::createFromFile("../data/shaders/light_point");
    calcLightShader = glow::Program::createFromFile("../data/shaders/light_calc");
    constLightShader = glow::Program::createFromFile("../data/shaders/light_const");
    fadeLightShader = glow::Program::createFromFile("../data/shaders/light_fade");

    auto ab = glow::ArrayBuffer::create();
    ab->defineAttributes({glow::ArrayBufferAttribute("aPosition", GL_FLOAT, 2, 0)});

    std::vector<glm::vec2> points(4);
    points.push_back({-1, -1});
    points.push_back({-1, 1});
    points.push_back({1, -1});
    points.push_back({1, 1});

    ab->bind().setData(points);
    quad = glow::VertexArray::create(ab, NULL, GL_TRIANGLE_STRIP);

    queries = std::make_shared<QueryTree>();
    glGenQueries(1, &query);
}

void Renderer::reset()
{
    queries = std::make_shared<QueryTree>();
}

void Renderer::resize(int width, int height)
{
    for (auto const& target : targets)
    {
        target->bind().resize(width, height);
    }

    // auto bound = frameBuffer->bind();
}

void Renderer::render()
{
    Stats::getInstance()->onFrameStart();
    Stats::getInstance()->countPass();

    mainPass();
    postPass();

    Stats::getInstance()->onFrameEnd();
}

void Renderer::mainPass()
{
    {
        auto bound = geometryBuffer->bind();

        glEnable(GL_CULL_FACE);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearStencil(0);

        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glStencilMask(0xFF);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glDisable(GL_CULL_FACE);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);

        glDisable(GL_CLIP_DISTANCE0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        auto shader = depthShader->use();
        shader.setUniform("uDepth", 1.0f);
        shader.setUniform("uColor", backgroundColor);
        quad->bind().draw();

        glEnable(GL_STENCIL_TEST);
    }

    {
        auto bound = lightBuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT);
    }

    RecursivePass* initial = new RecursivePass(this, queries);
    initial->initRoot(LogicWorld::getInstance()->getCamera()->getCamera().get());

    std::stack<RecursivePass*> passes;
    passes.push(initial);

    RecursivePass* current;
    RecursivePass* next;

    while (!passes.empty())
    {
        current = passes.top();
        next = current->step();

        if (next != current)
        {
            if (next == nullptr)
            {
                passes.pop();
                delete current;
            }
            else
            {
                passes.push(next);
            }
        }
    }
}

void Renderer::postPass()
{
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    glDisable(GL_CULL_FACE);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);

    glDisable(GL_CLIP_DISTANCE0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if (enableFXAA)
    {
        auto shader = fxaaShader->use();
        shader.setTexture("uTexColor", colorBuffer);
        quad->bind().draw();
    }
    else
    {
        auto shader = postShader->use();
        shader.setTexture("uTexColor", colorBuffer);
        quad->bind().draw();
    }
}
