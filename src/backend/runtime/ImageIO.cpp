#include "ImageIO.h"
#include "Image.h"

IG_BEGIN_IGNORE_WARNINGS
// We already make use of zlib, so use it here aswell
#include <zlib.h>
#define TINYEXR_USE_THREAD (1)
#define TINYEXR_USE_MINIZ (0)
#define TINYEXR_IMPLEMENTATION
#include <tinyexr.h>
IG_END_IGNORE_WARNINGS

namespace IG {

bool ImageIO::save(const std::filesystem::path& path, size_t width, size_t height,
                   const std::vector<const float*>& layer_ptrs, const std::vector<std::string>& layer_names)
{
    IG_ASSERT(layer_ptrs.size() == layer_names.size(), "Expected layer pointers and layer names of the same size");

    // Make sure the directory containing the new file exists
    if (!path.parent_path().empty())
        std::filesystem::create_directories(path.parent_path());

    EXRHeader header;
    InitEXRHeader(&header);
    // header.compression_type = TINYEXR_COMPRESSIONTYPE_ZIP;
    header.compression_type = TINYEXR_COMPRESSIONTYPE_PIZ;

    EXRImage image;
    InitEXRImage(&image);

    image.num_channels = (int)layer_ptrs.size();
    image.images       = (unsigned char**)layer_ptrs.data();
    image.width        = (int)width;
    image.height       = (int)height;

    header.num_channels = image.num_channels;
    header.channels     = new EXRChannelInfo[header.num_channels];

    constexpr size_t BUFFER_MAX = 255;
    for (int i = 0; i < image.num_channels; ++i) {
        const auto& name = layer_names[i];
        strncpy(header.channels[i].name, name.c_str(), BUFFER_MAX);
        if (name.length() >= BUFFER_MAX)
            header.channels[i].name[BUFFER_MAX - 1] = '\0';
    }

    header.pixel_types           = new int[header.num_channels];
    header.requested_pixel_types = new int[header.num_channels];
    for (int i = 0; i < header.num_channels; ++i) {
        header.pixel_types[i]           = TINYEXR_PIXELTYPE_FLOAT; // pixel type of input image
        header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // pixel type of output image to be stored in .EXR
    }

    const char* err = nullptr;
    // std::cout<< "debug " << path.generic_u8string().c_str() + layer_names[0] << std::endl;
    auto ret_name = path.generic_u8string().c_str();
    int ret         = SaveEXRImageToFile(&image, &header, ret_name, &err);

    delete[] header.channels;
    delete[] header.pixel_types;
    delete[] header.requested_pixel_types;

    if (ret != TINYEXR_SUCCESS) {
        std::string _err = err;
        FreeEXRErrorMessage(err); // free's buffer for an error message
        throw ImageSaveException(_err, path);
        return false;
    }

    return true;
}
} // namespace IG

/*
#include "ImageIO.h"
#include "Image.h"

IG_BEGIN_IGNORE_WARNINGS
// We already make use of zlib, so use it here aswell
#include <zlib.h>
#define TINYEXR_USE_THREAD (1)
#define TINYEXR_USE_MINIZ (0)
#define TINYEXR_IMPLEMENTATION
#include <tinyexr.h>
IG_END_IGNORE_WARNINGS

namespace IG {

bool ImageIO::save(const std::filesystem::path& path, size_t width, size_t height,
                   const std::vector<const float*>& layer_ptrs, const std::vector<std::string>& layer_names)
{
    IG_ASSERT(layer_ptrs.size() == layer_names.size(), "Expected layer pointers and layer names of the same size");

    std::cout<< "path: " << path << std::endl;
    std::cout<< "width: " << width << std::endl;
    std::cout<< "height: " << height << std::endl;
    std::cout<< "layer_ptrs: " << layer_ptrs.size() << std::endl;
    std::cout<< "layer_names: " << layer_names[0] << ", " << layer_names[1] << ", " << layer_names[2] << ", " << layer_names[3] << ", " << layer_names[4] << ", " << layer_names[5] << ", " << layer_names[6] << ", " << layer_names[7] << ", " << layer_names[8] << std::endl;
    // Make sure the directory containing the new file exists
    if (!path.parent_path().empty())
        std::filesystem::create_directories(path.parent_path());

    EXRHeader header;
    InitEXRHeader(&header);
    // header.compression_type = TINYEXR_COMPRESSIONTYPE_ZIP;
    header.compression_type = TINYEXR_COMPRESSIONTYPE_PIZ;

    EXRImage image;
    InitEXRImage(&image);

    image.num_channels = (int)layer_ptrs.size();
    image.images       = (unsigned char**)layer_ptrs.data();
    image.width        = (int)width;
    image.height       = (int)height;

    header.num_channels = image.num_channels;
    header.channels     = new EXRChannelInfo[header.num_channels];

    constexpr size_t BUFFER_MAX = 255;
    for (int i = 0; i < image.num_channels; ++i) {
        const auto& name = layer_names[i];
        strncpy(header.channels[i].name, name.c_str(), BUFFER_MAX);
        if (name.length() >= BUFFER_MAX)
            header.channels[i].name[BUFFER_MAX - 1] = '\0';
    }// contains aov_names: Default.R,G,B, Normals.R,G,B, Depth.R,G,B

    header.pixel_types           = new int[header.num_channels]; // array of size 9
    header.requested_pixel_types = new int[header.num_channels];
    
    // std::cout<< "debug " << TINYEXR_PIXELTYPE_FLOAT << std::endl;
    for (int i = 0; i < header.num_channels; ++i) {
        header.pixel_types[i]           = TINYEXR_PIXELTYPE_FLOAT; // pixel type of input image
        header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // pixel type of output image to be stored in .EXR
    }
    // std::cout<< "debug " << path.generic_u8string().c_str() <<", " << << std::endl;

    const char* err = nullptr;
    int ret         = SaveEXRImageToFile(&image, &header, path.generic_u8string().c_str(), &err);

    delete[] header.channels;
    delete[] header.pixel_types;
    delete[] header.requested_pixel_types;

    if (ret != TINYEXR_SUCCESS) {
        std::string _err = err;
        std::cout<< "save error " << std::endl;
        FreeEXRErrorMessage(err); // free's buffer for an error message
        throw ImageSaveException(_err, path);
        return false;
    }

    return true;
}
} // namespace IG

*/