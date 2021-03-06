#include "IBL.hh"

#include <glow/common/str_utils.hh>
#include <glow/objects/Program.hh>
#include <glow/objects/Texture2D.hh>
#include <glow/objects/TextureCubeMap.hh>
#include <glow/util/DefaultShaderParser.hh>

#if GLOW_EXTRAS_EMBED_SHADERS
#include "../../embed_shader.hh"
#endif

glow::SharedTexture2D glow::material::IBL::sEnvLutGGX = nullptr;

glow::SharedTexture2D glow::material::IBL::createEnvLutGGX(int width, int height)
{
    // only one mip-map level
    auto tex = Texture2D::createStorageImmutable(width, height, GL_RG16F, 1);

    // setup
    {
        auto t = tex->bind();
        t.setMinFilter(GL_LINEAR);
        t.setMagFilter(GL_LINEAR);
        t.setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
    }

    // compute
    const int localSize = 4;
    auto shader = Program::createFromFile("glow-material/precalc-env-brdf-lut.csh");
    {
        auto s = shader->use();
        s.setImage(0, tex, GL_WRITE_ONLY);
        s.compute((width - 1) / localSize + 1, (height - 1) / localSize + 1);
    }

    return tex;
}

glow::SharedTextureCubeMap glow::material::IBL::createEnvMapGGX(const glow::SharedTextureCubeMap& envMap, int size)
{
    // GL_RGB8 and GL_RGB16F are not supported! see https://www.opengl.org/sdk/docs/man/html/glBindImageTexture.xhtml
    auto tex = TextureCubeMap::createStorageImmutable(size, size, GL_RGBA16F);

    // setup
    {
        auto t = tex->bind();
        t.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
        t.setMagFilter(GL_LINEAR);
    }

    // compute
    const int localSize = 4;
    auto shader = Program::createFromFile("glow-material/precalc-env-map.csh");
    {
        auto s = shader->use();
        s.setTexture("uEnvMap", envMap);
        auto miplevel = 0;
        auto maxLevel = (int)glm::floor(glm::log2((float)size));
        for (auto tsize = size; tsize > 0; tsize /= 2)
        {
            auto roughness = miplevel / (float)maxLevel;
            s.setUniform("uRoughness", roughness);
            s.setImage(0, tex, GL_WRITE_ONLY, miplevel);
            s.compute((tsize - 1) / localSize + 1, (tsize - 1) / localSize + 1, 6);
            ++miplevel;
        }
    }

    // we manually calculated all mipmaps
    tex->setMipmapsGenerated(true);

    return tex;
}

void glow::material::IBL::initShaderGGX(UsedProgram& shader, SharedTexture2D const& customLUT)
{
    if (!customLUT && !sEnvLutGGX)
        sEnvLutGGX = createEnvLutGGX();

    shader.setTexture("uGlowMaterialEnvLutGGX", customLUT ? customLUT : sEnvLutGGX);
}

void glow::material::IBL::prepareShaderGGX(UsedProgram& shader, SharedTextureCubeMap const& envMapGGX)
{
    shader.setTexture("uGlowMaterialEnvMapGGX", envMapGGX);
}

void glow::material::IBL::GlobalInit()
{
#ifdef GLOW_EXTRAS_EMBED_SHADERS

    for (auto& virtualPair : internal_embedded_files::material_embed_shaders)
        DefaultShaderParser::addVirtualFile(virtualPair.first, virtualPair.second);

#else

    DefaultShaderParser::addIncludePath(util::pathOf(__FILE__) + "/../../shader");

#endif
}
