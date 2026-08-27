#include "image_writer.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

bool write_png_image(const std::string& filename, int width, int height, const std::vector<uint8_t>& rgb_data) {
    int result = stbi_write_png(filename.c_str(), width, height, 3, rgb_data.data(), width * 3);
    return result != 0;
}
