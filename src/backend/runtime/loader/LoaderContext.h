#pragma once

#include "LoaderEnvironment.h"
#include "Target.h"
#include "TechniqueInfo.h"

#include <filesystem>
#include <variant>

namespace IG {

struct SceneDatabase;

using TextureColorVariant = std::variant<uint32, Vector3f>;
inline bool isTexture(const TextureColorVariant& var)
{
    return std::holds_alternative<uint32>(var);
}

inline uint32 extractTexture(const TextureColorVariant& var)
{
    return std::get<uint32>(var);
}

inline Vector3f extractColor(const TextureColorVariant& var)
{
    return std::get<Vector3f>(var);
}

struct LoaderContext {
    Parser::Scene Scene;

    std::filesystem::path FilePath;
    IG::Target Target;
    bool EnablePadding;
    size_t SamplesPerIteration;
    std::unordered_map<std::string, uint32> Images; // Image to Buffer

    std::string CameraType;
    std::string TechniqueType;
    IG::TechniqueInfo TechniqueInfo;

    bool IsTracer = false;

    size_t CurrentTechniqueVariant;
    inline const IG::TechniqueVariantInfo CurrentTechniqueVariantInfo() const { return TechniqueInfo.Variants[CurrentTechniqueVariant]; }

    std::unordered_map<std::string, uint32> TextureBuffer; // Texture to Buffer/Image, used only in workaround
    std::vector<Vector3f> TextureAverages;                 // Workaround for now

    LoaderEnvironment Environment;
    SceneDatabase* Database = nullptr;

    size_t EntityCount;

    // The width & height while loading. This might change in the actual rendering
    size_t FilmWidth  = 800;
    size_t FilmHeight = 600;

    std::filesystem::path handlePath(const std::filesystem::path& path, const Parser::Object& obj) const;

    Vector3f extractColor(const Parser::Object& obj, const std::string& propname, const Vector3f& def = Vector3f::Ones()) const;
    float extractIOR(const Parser::Object& obj, const std::string& propname, float def = 1.55f) const;
};

} // namespace IG