#ifndef PNG_H_INCLUDED
#define PNG_H_INCLUDED

#include <iostream>
#include <string>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

struct PngImage {
    int width = 0;
    int height = 0;
    int channels = 0;
    unsigned char* data = nullptr;
};

PngImage load_png(const std::string& filename) {
    PngImage img;
    img.data = stbi_load(filename.c_str(), &img.width, &img.height, &img.channels, 0);
    if (!img.data) {
        std::cerr << "Failed to load PNG: " << stbi_failure_reason() << "\n";

        img.data = nullptr;
        img.height = img.width = img.channels = 0;
    }
    return img;
}

bool save_png(const std::string& filename, const PngImage& img) {
    int success = stbi_write_png(filename.c_str(), img.width, img.height, img.channels, img.data, img.width * img.channels);
    return success != 0;
}

#endif // PNG_H_INCLUDED
