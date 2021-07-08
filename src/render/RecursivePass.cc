#include "RecursivePass.hh"

#include "Drawable.hh"
#include "Light.hh"
#include "Portal.hh"
#include "QueryTree.hh"
#include "RenderWorld.hh"
#include "Renderer.hh"

#include "../Stats.hh"

#include <glow/objects/ArrayBuffer.hh>
#include <glow/objects/Framebuffer.hh>
#include <glow/objects/Program.hh>
#include <glow/objects/TextureRectangle.hh>
#include <glow/objects/VertexArray.hh>

void RecursivePass::initRoot(glow::camera::Camera* camera)
{
    view = camera->getViewMatrix();
    proj = camera->getProjectionMatrix();
}

void RecursivePass::initChild(Portal* portal)
{
    this->portal = portal;
    this->invertView = parent->invertsView() ? !portal->isMirror() : portal->isMirror();

    clipPlane = portal->calculateClipPlane(renderer->getPortalClipGuard());
    view = portal->transformView(parent->getView());
    proj = parent->getProj();
}

RecursivePass* RecursivePass::step()
{
    if (progress < 0)
    {
        renderWorld();
        renderLight();

        progress = 0;
        return this;
    }
    else if (((uint32_t)progress) < RenderWorld::getInstance()->getPortalCount())
    {
        RecursivePass* next;

        SharedPortal current = RenderWorld::getInstance()->getPortal(progress);
        SharedQueryTree query = queries->prepareChild(progress);

        renderPortal(current, query);

        if (query->getResult())
        {
            if (depth + 1 < renderer->getRecursionDepth())
            {
                next = launchChild(current.get(), query);
            }
            else
            {
                clearPortalDepth(renderer->getFadeColor());
                renderBackground();
                resetPortalDepth(current.get());
                next = this;
            }
        }
        else
        {
            resetPortalDepth(current.get());
            next = this;
        }

        progress++;
        return next;
    }
    else
    {
        done();
        return nullptr;
    }
}

void RecursivePass::done()
{
    if (isChild())
    {
        parent->resetPortalDepth(portal);
    }
}

RecursivePass* RecursivePass::launchChild(Portal* portal, SharedQueryTree query)
{
    RecursivePass* child = new RecursivePass(renderer, query, this, depth + 1);
    Stats::getInstance()->countPass();

    clearPortalDepth(renderer->getBackgroundColor());

    child->initChild(portal);
    return child;
}

void RecursivePass::prepareFlags(GLboolean color, bool depth, GLenum depthFunc, GLint stencilRef, GLenum stencilOp, bool culling, bool clip, bool wireframe)
{
    glColorMask(color, color, color, color);

    if (depth)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }

    glDepthFunc(depthFunc);

    glStencilFunc(GL_EQUAL, stencilRef, 0xFF);

    glStencilOp(GL_KEEP, GL_KEEP, stencilOp);

    if (culling)
    {
        glEnable(GL_CULL_FACE);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }

    if (invertView)
    {
        glFrontFace(GL_CW);
    }
    else
    {
        glFrontFace(GL_CCW);
    }

    if (clip)
    {
        if (isRoot())
        {
            glDisable(GL_CLIP_DISTANCE0);
        }
        else
        {
            glEnable(GL_CLIP_DISTANCE0);
        }
    }
    else
    {
        glDisable(GL_CLIP_DISTANCE0);
    }

    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? (renderer->showsWireframe() ? GL_LINE : GL_FILL) : GL_FILL);
}

void RecursivePass::renderWorld()
{
    prepareFlags(GL_TRUE, true, GL_LESS, depth, GL_KEEP, true, true, true);

    auto framebuffer = renderer->getGeometryBuffer()->bind();
    auto shader = renderer->getWorldShader()->use();

    shader.setUniform("uView", view);
    shader.setUniform("uProj", proj);
    shader.setUniform("uClip", clipPlane);

    RenderWorld::getInstance()->getRoot()->draw(&shader);
}

void RecursivePass::renderLight()
{
    auto framebuffer = renderer->getLightBuffer()->bind();
    size_t n;
    
    std::vector<SharedGlobalLight>* globalLights = RenderWorld::getInstance()->getGlobalLights();
    n = globalLights->size();

    if (n > 0)
    {
        prepareFlags(GL_TRUE, false, GL_LESS, depth, GL_KEEP, false, false, false);

        auto shader = renderer->getGlobalLightShader()->use();

        shader.setTexture("uPosition", renderer->getPositionBuffer());
        shader.setTexture("uNormal", renderer->getNormalBuffer());
        shader.setTexture("uAlbedo", renderer->getAlbedoBuffer());

        for (size_t i = 0; i < n; i++)
        {
            renderGlobalLight(globalLights->at(i), &shader);
        }
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDepthMask(GL_FALSE);

    std::vector<SharedPointLight>* pointLights = RenderWorld::getInstance()->getPointLights();
    n = pointLights->size();

    if (n > 0)
    {
        glm::vec3 cameraPosition = glm::vec3(glm::inverse(view)[3]);

        for (size_t i = 0; i < n; i++)
        {
            renderPointLight(pointLights->at(i), cameraPosition);
        }
    }

    // spotlights not supported

    if (depth >= renderer->getFadeDepth())
    {
        fade();
    }

    glDepthMask(GL_TRUE);
    glBlendFunc(GL_ONE, GL_ZERO);
    glDisable(GL_BLEND);
}

void RecursivePass::renderGlobalLight(SharedGlobalLight globalLight, glow::UsedProgram* shader) {

    shader->setUniform("uLightDir", globalLight->getDirection());
    shader->setUniform("uLightColor", globalLight->getColor());

    shader->setUniform("uAmbient", globalLight->getAmbient());
    shader->setUniform("uIntensity", globalLight->getIntensity());

    glEnable(GL_CULL_FACE);

    if (invertView)
    {
        glCullFace(GL_BACK);
    }
    else
    {
        glCullFace(GL_FRONT);
    }

    renderer->getQuad()->bind().draw();

    glCullFace(GL_BACK);
}

void RecursivePass::renderPointLight(SharedPointLight pointLight, glm::vec3 cameraPosition) {

    if (glm::distance(cameraPosition, pointLight->getLocation()) < pointLight->getRadius() + LIGHT_DEFAULT_GUARD)
    {
        prepareFlags(GL_FALSE, false, GL_LESS, depth, GL_INCR, false, false, false);

        {
            auto shader = renderer->getConstLightShader()->use();

            renderer->getQuad()->bind().draw();
        }
    }
    else
    {
        prepareFlags(GL_FALSE, true, GL_LESS, depth, GL_INCR, true, false, false);

        {
            auto shader = renderer->getPointLightShader()->use();

            shader.setUniform("uView", view);
            shader.setUniform("uProj", proj);
            shader.setUniform("uModel", pointLight->getModel());

            pointLight->getMesh()->bind().draw();
        }
    }

    prepareFlags(GL_FALSE, true, GL_LESS, depth + 1, GL_DECR, true, false, false);

    if (invertView)
    {
        glFrontFace(GL_CCW);
    }
    else
    {
        glFrontFace(GL_CW);
    }

    {
        auto shader = renderer->getPointLightShader()->use();

        shader.setUniform("uView", view);
        shader.setUniform("uProj", proj);
        shader.setUniform("uModel", pointLight->getModel());

        pointLight->getMesh()->bind().draw();
    }

    prepareFlags(GL_TRUE, false, GL_LESS, depth + 1, GL_DECR, false, false, false);

    {
        auto shader = renderer->getCalcLightShader()->use();

        shader.setTexture("uPosition", renderer->getPositionBuffer());
        shader.setTexture("uNormal", renderer->getNormalBuffer());
        shader.setTexture("uAlbedo", renderer->getAlbedoBuffer());

        shader.setUniform("uCameraPos", cameraPosition);
        shader.setUniform("uLightPos", pointLight->getLocation());
        shader.setUniform("uLightColor", pointLight->getColor());
        shader.setUniform("uLightIntensity", pointLight->getIntensity());
        shader.setUniform("uLightConst", pointLight->getConstFactor());
        shader.setUniform("uLightLin", pointLight->getLinFactor());
        shader.setUniform("uLightQuad", pointLight->getQuadFactor());

        glEnable(GL_CULL_FACE);

        if (invertView)
        {
            glCullFace(GL_BACK);
        }
        else
        {
            glCullFace(GL_FRONT);
        }

        renderer->getQuad()->bind().draw();

        glCullFace(GL_BACK);
    }
}

void RecursivePass::renderSpotLight(SharedSpotLight spotLight) {
    
}

void RecursivePass::fade() {
    uint32_t diff = depth - renderer->getFadeDepth() + 1;
    uint32_t max = renderer->getRecursionDepth() - renderer->getFadeDepth() + 1;

    float factor = ((float)diff) / ((float)max);
    factor = glm::pow(factor, 1.0f / 2.224f);

    prepareFlags(GL_TRUE, false, GL_LESS, depth, GL_KEEP, false, false, false);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_CONSTANT_COLOR);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glBlendColor(1.0f - factor, 1.0f - factor, 1.0f - factor, 1.0f);

    {
        auto shader = renderer->getFadeLightShader()->use();

        shader.setUniform("uColor", renderer->getFadeColor() * factor);

        glEnable(GL_CULL_FACE);

        if (invertView)
        {
            glCullFace(GL_BACK);
        }
        else
        {
            glCullFace(GL_FRONT);
        }

        renderer->getQuad()->bind().draw();

        glCullFace(GL_BACK);
    }

    glDepthMask(GL_TRUE);
    glBlendFunc(GL_ONE, GL_ZERO);
    glDisable(GL_BLEND);
}

void RecursivePass::renderPortal(SharedPortal child, SharedQueryTree query)
{
    prepareFlags(GL_FALSE, true, GL_LESS, depth, GL_INCR, true, true);

    auto framebuffer = renderer->getGeometryBuffer()->bind();
    auto shader = renderer->getConstShader()->use();

    shader.setUniform("uView", view);
    shader.setUniform("uProj", proj);
    shader.setUniform("uClip", clipPlane);
    shader.setUniform("uModel", child->getModel());
    shader.setUniform("uColor", renderer->getFadeColor());

    query->startQuery();
    child->getMesh()->bind().draw();
    query->endQuery();
}

void RecursivePass::clearPortalDepth(glm::vec3 color)
{
    prepareFlags(GL_TRUE, true, GL_ALWAYS, depth + 1, GL_KEEP, false, false);

    auto framebuffer = renderer->getGeometryBuffer()->bind();
    auto shader = renderer->getDepthShader()->use();

    shader.setUniform("uDepth", 1.0f);
    shader.setUniform("uColor", color);

    renderer->getQuad()->bind().draw();
}

void RecursivePass::resetPortalDepth(Portal* child)
{
    prepareFlags(GL_FALSE, true, GL_ALWAYS, depth + 1, GL_DECR, true, true);

    auto framebuffer = renderer->getGeometryBuffer()->bind();
    auto shader = renderer->getConstShader()->use();

    shader.setUniform("uView", view);
    shader.setUniform("uProj", proj);
    shader.setUniform("uClip", clipPlane);
    shader.setUniform("uModel", child->getModel());
    shader.setUniform("uColor", renderer->getFadeColor());

    child->getMesh()->bind().draw();
}

void RecursivePass::renderBackground()
{
    prepareFlags(GL_TRUE, false, GL_LESS, depth + 1, GL_KEEP, false, false, false);

    auto framebuffer = renderer->getLightBuffer()->bind();
    auto shader = renderer->getLightShader()->use();

    shader.setTexture("uPosition", renderer->getPositionBuffer());
    shader.setTexture("uNormal", renderer->getNormalBuffer());
    shader.setTexture("uAlbedo", renderer->getAlbedoBuffer());
    shader.setUniform("uLightDir", glm::vec3());

    renderer->getQuad()->bind().draw();
}
