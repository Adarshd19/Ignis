#include "AdvancedShadowShader.h"
#include "Logger.h"
#include "loader/Loader.h"
#include "loader/LoaderLight.h"
#include "loader/LoaderTechnique.h"
#include "loader/ShaderUtils.h"
#include "loader/ShadingTree.h"

#include <sstream>

namespace IG {
using namespace Parser;

std::string AdvancedShadowShader::setup(bool is_hit, LoaderContext& ctx)
{
    std::stringstream stream;

    ShadingTree tree(ctx);

    stream << LoaderTechnique::generateHeader(ctx) << std::endl;

    stream << "#[export] fn ig_advanced_shadow_shader(settings: &Settings, first: i32, last: i32) -> () {" << std::endl
           << "  maybe_unused(settings);" << std::endl
           << "  " << ShaderUtils::constructDevice(ctx.Target) << std::endl
           << std::endl;

    if (ctx.CurrentTechniqueVariantInfo().UsesLights) {
        bool requireAreaLight = is_hit || ctx.CurrentTechniqueVariantInfo().UsesAllLightsInMiss;
        if (requireAreaLight)
            stream << ShaderUtils::generateDatabase() << std::endl;

        stream << LoaderLight::generate(tree, !requireAreaLight)
               << std::endl;
    }

    stream << "  let spp = " << ctx.SamplesPerIteration << " : i32;" << std::endl;

    // Will define technique
    stream << LoaderTechnique::generate(ctx) << std::endl
           << std::endl;

    stream << "  let is_hit = " << (is_hit ? "true" : "false") << ";" << std::endl
           << "  let use_framebuffer = " << (!ctx.CurrentTechniqueVariantInfo().LockFramebuffer ? "true" : "false") << ";" << std::endl
           << "  device.handle_advanced_shadow_shader(technique, first, last, spp, use_framebuffer, is_hit)" << std::endl
           << "}" << std::endl;

    return stream.str();
}

} // namespace IG