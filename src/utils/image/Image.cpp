#include "image.hpp"
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "../../include/stb_image.h"

using namespace std;

Image::Image(const string &filename) {
    unsigned char *data = stbi_load(filename.c_str(), &this->width, &this->height, &this->channels, 4);

    if (!data) {
        throw runtime_error("Error: Tidak dapat memuat gambar " + filename + ". Error: " + stbi_failure_reason());
    }

    this->channels = 4;
    this->pixels.resize(this->height, vector<Pixel>(this->width));

    for (int y = 0; y < this->height; ++y) {
        for (int x = 0; x < this->width; ++x) {
            int index = (y * this->width + x) * this->channels;
            this->pixels[y][x] = {data[index], data[index + 1], data[index + 2], data[index + 3]};
        }
    }

    stbi_image_free(data);
}

int Image::getPixelCount() const {
    return width * height;
}

ostream &operator<<(ostream &os, const Image &image) {
    os << "Image Info:\n";
    os << "Width: " << image.width << "\n";
    os << "Height: " << image.height << "\n";
    os << "Channels: " << image.channels << "\n";
    os << "Total Pixels: " << image.getPixelCount() << "\n";
    return os;
}