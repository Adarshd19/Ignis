#include "RayGenerationShader.h"
#include "Logger.h"
#include "loader/Loader.h"
#include "loader/LoaderCamera.h"
#include "loader/LoaderTechnique.h"
#include "loader/ShaderUtils.h"

#include <sstream>

namespace IG {
using namespace Parser;

std::string RayGenerationShader::setup(LoaderContext& ctx)
{
    std::stringstream stream;

    stream << LoaderTechnique::generateHeader(ctx, true) << std::endl;

    stream << "#[export] fn ig_ray_generation_shader(settings: &Settings, iter: i32, id: &mut i32, size: i32, xmin: i32, ymin: i32, xmax: i32, ymax: i32) -> i32 {" << std::endl
           << "  maybe_unused(settings);" << std::endl
           << "  " << ShaderUtils::constructDevice(ctx.Target) << std::endl
           << std::endl
           << "  let spp = " << ctx.SamplesPerIteration << " : i32;" << std::endl;

    if (ctx.IsTracer) {
        stream << "  let emitter = make_list_emitter(device.load_rays(), iter, init_raypayload);" << std::endl;
    } else {
        stream << LoaderCamera::generate(ctx) << std::endl // Will set `camera`
               << "  let emitter = make_camera_emitter(camera, iter, spp, make_uniform_pixel_sampler()/*make_mjitt_pixel_sampler(4,4)*/, init_raypayload);" << std::endl;
    }

    stream << "  device.generate_rays(emitter, id, size, xmin, ymin, xmax, ymax, spp)" << std::endl
           << "}" << std::endl;

    return stream.str();
}

} // namespace IG