#include "image.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "../../include/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../include/stb_image_write.h"

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

Pixel Image::getMean(int fromX, int fromY, int toX, int toY) const {
    double sumR = 0, sumG = 0, sumB = 0, sumA = 0;
    int count = (toX - fromX) * (toY - fromY);

    if (count == 0)
        return Pixel(0, 0, 0, 0);

    for (int y = fromY; y < toY; ++y) {
        for (int x = fromX; x < toX; ++x) {
            const Pixel &p = pixels[y][x];
            sumR += p.r / count;
            sumG += p.g / count;
            sumB += p.b / count;
            sumA += p.a / count;
        }
    }

    return Pixel(
        static_cast<unsigned char>(min(255.0, sumR)),
        static_cast<unsigned char>(min(255.0, sumG)),
        static_cast<unsigned char>(min(255.0, sumB)),
        static_cast<unsigned char>(min(255.0, sumA)));
}

bool Image::saveFromMatrix(const vector<vector<Pixel>> &imageMatrix, int width, int height, const string &filePath) {
    vector<unsigned char> outputData;
    outputData.reserve(width * height * 4);

    // Convert the matrix to a 1D vector in RGBA format
    for (const auto &row : imageMatrix) {
        for (const auto &pixel : row) {
            outputData.push_back(pixel.r);
            outputData.push_back(pixel.g);
            outputData.push_back(pixel.b);
            outputData.push_back(pixel.a);
        }
    }

    // Save the image using stb_image_write
    if (stbi_write_png(filePath.c_str(), width, height, 4, outputData.data(), width * 4)) {
        cout << "Image successfully saved to: " << filePath << endl;
        return true;
    } else {
        cerr << "Error saving image to: " << filePath << endl;
        return false;
    }
}