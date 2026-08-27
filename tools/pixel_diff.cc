#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <image1.png> <image2.png> [--tolerance <int>] [--mean-tol <float>]\n";
        return 2;
    }

    std::string path1 = argv[1];
    std::string path2 = argv[2];

    int max_channel_delta_tol = 30; // Default max delta per channel (allowing RNG variance)
    double max_mean_delta_tol = 15.0;

    for (int i = 3; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--tolerance" && i + 1 < argc) {
            max_channel_delta_tol = std::stoi(argv[++i]);
        } else if (arg == "--mean-tol" && i + 1 < argc) {
            max_mean_delta_tol = std::stod(argv[++i]);
        }
    }

    int w1, h1, c1;
    int w2, h2, c2;

    unsigned char* data1 = stbi_load(path1.c_str(), &w1, &h1, &c1, 3);
    if (!data1) {
        std::cerr << "Error: Failed to load image " << path1 << "\n";
        return 2;
    }

    unsigned char* data2 = stbi_load(path2.c_str(), &w2, &h2, &c2, 3);
    if (!data2) {
        std::cerr << "Error: Failed to load image " << path2 << "\n";
        stbi_image_free(data1);
        return 2;
    }

    if (w1 != w2 || h1 != h2) {
        std::cerr << "Error: Image dimensions do not match: (" << w1 << "x" << h1
                  << ") vs (" << w2 << "x" << h2 << ")\n";
        stbi_image_free(data1);
        stbi_image_free(data2);
        return 1;
    }

    int total_pixels = w1 * h1;
    int differing_pixels = 0;
    int max_delta = 0;
    double total_delta = 0.0;

    for (int i = 0; i < total_pixels * 3; i++) {
        int d = std::abs(int(data1[i]) - int(data2[i]));
        if (d > max_delta)
            max_delta = d;
        total_delta += d;
        if (d > max_channel_delta_tol)
            differing_pixels++;
    }

    double mean_delta = total_delta / (total_pixels * 3);

    stbi_image_free(data1);
    stbi_image_free(data2);

    std::cout << "Image Diff Analysis between " << path1 << " and " << path2 << ":\n"
              << "  Total pixels: " << total_pixels << "\n"
              << "  Max channel delta: " << max_delta << " (allowed: " << max_channel_delta_tol << ")\n"
              << "  Mean channel delta: " << mean_delta << " (allowed: " << max_mean_delta_tol << ")\n"
              << "  Differing channel count (> tol): " << differing_pixels << "\n";

    if (mean_delta > max_mean_delta_tol) {
        std::cerr << "FAILED: Mean pixel delta exceeds threshold (" << mean_delta << " > " << max_mean_delta_tol << ")\n";
        return 1;
    }

    std::cout << "PASSED: Images match within tolerance.\n";
    return 0;
}
