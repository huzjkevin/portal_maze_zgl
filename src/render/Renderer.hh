#pragma once

#include "../forward.hh"
#include <glow-extras/camera/Camera.hh>

class Renderer
{
private:
    static SharedRenderer instance;

    // framebuffers
    std::vector<glow::SharedTextureRectangle> targets;
    glow::SharedTextureRectangle depthBuffer;

    glow::SharedFramebuffer geometryBuffer;
    glow::SharedTextureRectangle positionBuffer;
    glow::SharedTextureRectangle normalBuffer;
    glow::SharedTextureRectangle albedoBuffer;

    glow::SharedFramebuffer lightBuffer;
    glow::SharedTextureRectangle colorBuffer;

    // shaders
    glow::SharedProgram worldShader;
    glow::SharedProgram constShader;
    glow::SharedProgram depthShader;
    glow::SharedProgram postShader;
    glow::SharedProgram fxaaShader;
    glow::SharedProgram lightShader;
    glow::SharedProgram globalLightShader;
    glow::SharedProgram pointLightShader;
    glow::SharedProgram calcLightShader;
    glow::SharedProgram constLightShader;
    glow::SharedProgram fadeLightShader;

    // other objects
    glow::SharedVertexArray quad;

    SharedQueryTree queries;
    GLuint query;

    // parameters
    uint32_t recursionDepth;
    uint32_t fadeDepth;
    float portalClipGuard;
    bool showWireframe;
    bool enableFXAA;
    glm::vec3 backgroundColor;
    glm::vec3 fadeColor;

public:
    Renderer()
      : recursionDepth(DEFAULT_DEPTH),
        fadeDepth(DEFAULT_FADE_DEPTH),
        portalClipGuard(DEFAULT_CLIPDIST),
        showWireframe(DEFAULT_WIREFRAME),
        enableFXAA(DEFAULT_FXAA),
        backgroundColor(DEFAULT_BACKGROUND_R, DEFAULT_BACKGROUND_G, DEFAULT_BACKGROUND_B),
        fadeColor(DEFAULT_FADE_R, DEFAULT_FADE_G, DEFAULT_FADE_B)
    {
    }

    virtual ~Renderer() {}

    static SharedRenderer getInstance() { return instance; }

    std::vector<glow::SharedTextureRectangle>* getTargets() { return &targets; }
    glow::SharedTextureRectangle getDepthBuffer() { return depthBuffer; }

    glow::SharedFramebuffer getGeometryBuffer() { return geometryBuffer; }
    glow::SharedTextureRectangle getPositionBuffer() { return positionBuffer; }
    glow::SharedTextureRectangle getNormalBuffer() { return normalBuffer; }
    glow::SharedTextureRectangle getAlbedoBuffer() { return albedoBuffer; }

    glow::SharedFramebuffer getLightBuffer() { return lightBuffer; }
    glow::SharedTextureRectangle getColorBuffer() { return colorBuffer; }

    glow::SharedProgram getWorldShader() { return worldShader; }
    glow::SharedProgram getConstShader() { return constShader; }
    glow::SharedProgram getDepthShader() { return depthShader; }
    glow::SharedProgram getPostShader() { return depthShader; }
    glow::SharedProgram getFXAAShader() { return fxaaShader; }
    glow::SharedProgram getLightShader() { return lightShader; }
    glow::SharedProgram getGlobalLightShader() { return globalLightShader; }
    glow::SharedProgram getPointLightShader() { return pointLightShader; }
    glow::SharedProgram getCalcLightShader() { return calcLightShader; }
    glow::SharedProgram getConstLightShader() { return constLightShader; }
    glow::SharedProgram getFadeLightShader() { return fadeLightShader; }

    glow::SharedVertexArray getQuad() { return quad; }
    void setQuad(glow::SharedVertexArray q) { quad = q; }

    SharedQueryTree getQueryTree() { return queries; }

    GLuint getQuery() { return query; }
    void setQuery(GLuint q) { query = q; }

    uint32_t getRecursionDepth() const { return recursionDepth; }
    void setRecursionDepth(uint32_t d) { recursionDepth = d; }

    uint32_t getFadeDepth() const { return fadeDepth; }
    void setFadeDepth(uint32_t f) { fadeDepth = f; }

    float getPortalClipGuard() const { return portalClipGuard; }
    void setPortalClipGuard(float g) { portalClipGuard = g; }

    glm::vec3 getBackgroundColor() const { return backgroundColor; }
    void setBackgroundColor(glm::vec3 b) { backgroundColor = b; }
    float* backgroundColorPtr() { return &backgroundColor.r; }

    glm::vec3 getFadeColor() const { return fadeColor; }
    void setFadeColor(glm::vec3 f) { fadeColor = f; }
    float* fadeColorPtr() { return &fadeColor.r; }

    bool showsWireframe() const { return showWireframe; }
    void setWireframe(bool w) { showWireframe = w; }
    bool* showWireframePtr() { return &showWireframe; }

    bool isFXAAEnabled() const { return enableFXAA; }
    void setFXAAEnabled(bool f) { enableFXAA = f; }
    bool* enableFXAAPtr() { return &enableFXAA; }

    static void initInstance();

    void init();
    void reset();

    void resize(int width, int height);

    void render();

    void mainPass();
    void postPass();
};
