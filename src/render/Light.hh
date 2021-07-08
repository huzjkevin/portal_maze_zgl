#pragma once

#include "../forward.hh"

class Light
{
public:
    Light() {}
    virtual ~Light() {}
};

class GlobalLight : public Light
{
private:
    glm::vec3 direction;
    glm::vec3 color;

    float ambient;
    float intensity;

public:
    GlobalLight(glm::vec3 d = glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3 c = glm::vec3(1.0f, 1.0f, 1.0f), float a = LIGHT_DEFAULT_AMBIENT, float i = LIGHT_DEFAULT_DIRECTIONAL);
    virtual ~GlobalLight();

    glm::vec3 getDirection() { return direction; }
    void setDirection(glm::vec3 d) { direction = d; }

    glm::vec3 getColor() { return color; }
    void setColor(glm::vec3 c) { color = c; }

    float getAmbient() { return ambient; }
    void setAmbient(float a) { ambient = a; }

    float getIntensity() { return intensity; }
    void setIntensity(float i) { intensity = i; }
};

class PointLight : public Light
{
private:
    glm::vec3 location;
    glm::vec3 color;

    float intensity;
    float cutoff;

    float constFactor;
    float linFactor;
    float quadFactor;

    glow::SharedVertexArray mesh;
    glm::mat4 model;

public:
    PointLight(glm::vec3 loc = glm::vec3(0.0f, 0.0f, 0.0f),
               glm::vec3 col = glm::vec3(1.0f, 1.0f, 1.0f),
               float in = LIGHT_DEFAULT_INTENSITY,
               float cut = LIGHT_DEFAULT_CUTOFF,
               float c = LIGHT_DEFAULT_CONST,
               float l = LIGHT_DEFAULT_LIN,
               float q = LIGHT_DEFAULT_QUAD);
    virtual ~PointLight();

    glm::vec3 getLocation() { return location; }
    void setLocation(glm::vec3 l) { location = l; }

    glm::vec3 getColor() { return color; }
    void setColor(glm::vec3 c) { color = c; }

    float getIntensity() { return intensity; }
    void setIntensity(float i) { intensity = i; }

    float getCutoff() { return cutoff; }
    void setCutoff(float c) { cutoff = c; }

    float getConstFactor() { return constFactor; }
    float getLinFactor() { return linFactor; }
    float getQuadFactor() { return quadFactor; }

    glow::SharedVertexArray getMesh() { return mesh; }
    glm::mat4 getModel() { return model; }

    void setAttenuation(float c, float l, float q);
    void setAttenuation(float radius);

    float getRadius();
    void initObjects(int32_t segments, int32_t rings);
};

class SpotLight : public Light
{
};
