#ifndef IMAGE_WRITER_H
#define IMAGE_WRITER_H

#include <string>
#include <vector>
#include <cstdint>

bool write_png_image(const std::string& filename, int width, int height, const std::vector<uint8_t>& rgb_data);

#endif // IMAGE_WRITER_H
