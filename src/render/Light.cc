#include "Light.hh"

#include <glow/objects/VertexArray.hh>
#include <glow-extras/geometry/UVSphere.hh>

PointLight::PointLight(glm::vec3 loc, glm::vec3 col, float in, float cut, float c, float l, float q)
  : location(loc), color(col), intensity(in), cutoff(cut), constFactor(c), linFactor(l), quadFactor(q)
{

}

PointLight::~PointLight() {

}

void PointLight::setAttenuation(float c, float l, float q) {
    constFactor = c;
    linFactor = l;
    quadFactor = q;
}

void PointLight::setAttenuation(float radius) {
    constFactor = 0.1f;
    linFactor = 0.0f;
    quadFactor = (intensity/cutoff - 0.1f) / (radius * radius);
}

float PointLight::getRadius() {
    return (glm::sqrt(linFactor * linFactor - 4 * quadFactor * (constFactor - intensity/cutoff)) - linFactor) / (2 * quadFactor);
}

void PointLight::initObjects(int32_t segments, int32_t rings) {
    mesh = glow::geometry::UVSphere<>(glow::geometry::UVSphere<>::attributesOf(nullptr), segments, rings, getRadius()).generate();
    model = glm::translate(location);
}

GlobalLight::GlobalLight(glm::vec3 d, glm::vec3 c, float a, float i) : direction(d), color(c), ambient(a), intensity(i) {

}

GlobalLight::~GlobalLight() {

}
