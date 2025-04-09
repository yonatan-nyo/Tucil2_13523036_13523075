#include "image.hpp"
#include <algorithm>
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

namespace fs = std::filesystem;
using namespace std;

Image::Image(const string &filename) {
    unsigned char *data = stbi_load(filename.c_str(), &this->width, &this->height, &this->channels, 0);
    cout << "CHANNELS: " << this->channels << endl;

    if (!data) {
        throw runtime_error("Error: Tidak dapat memuat gambar " + filename + ". Error: " + stbi_failure_reason());
    }

    this->pixels.resize(this->height, vector<Pixel>(this->width));

    for (int y = 0; y < this->height; ++y) {
        for (int x = 0; x < this->width; ++x) {
            int index = (y * this->width + x) * this->channels;

            if (channels == 1) {
                this->pixels[y][x] = {data[index], data[index], data[index], 255};
            } else if (channels == 3) {
                this->pixels[y][x] = {data[index], data[index + 1], data[index + 2], 255};
            } else if (channels == 4) {
                this->pixels[y][x] = {data[index], data[index + 1], data[index + 2], data[index + 3]};
            } else {
                throw runtime_error("Error: Unsupported number of channels: " + to_string(this->channels));
            }
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
    double retR = 0, retG = 0, retB = 0, retA = 0;
    int count = (toX - fromX) * (toY - fromY);

    if (count == 0)
        return Pixel(0, 0, 0, 0);

    for (int y = fromY; y < toY; ++y) {
        for (int x = fromX; x < toX; ++x) {
            const Pixel &p = pixels[y][x];
            retR += static_cast<double>(p.r) / count;
            retG += static_cast<double>(p.g) / count;
            retB += static_cast<double>(p.b) / count;
            retA += static_cast<double>(p.a) / count;
        }
    }

    return Pixel(retR, retG, retB, retA);
}

void Image::saveFromMatrix(const vector<vector<Pixel>> &imageMatrix, int width, int height, int channels, fs::path &filePath) {
    vector<unsigned char> outputData;
    outputData.reserve(width * height * channels);

    for (const auto &row : imageMatrix) {
        for (const auto &pixel : row) {
            if (channels == 1) {
                unsigned char gray = static_cast<unsigned char>(0.299 * pixel.r + 0.587 * pixel.g + 0.114 * pixel.b);
                outputData.push_back(gray);
            } else if (channels == 3) {
                outputData.push_back(pixel.r);
                outputData.push_back(pixel.g);
                outputData.push_back(pixel.b);
            } else if (channels == 4) {
                outputData.push_back(pixel.r);
                outputData.push_back(pixel.g);
                outputData.push_back(pixel.b);
                outputData.push_back(pixel.a);
            } else {
                throw std::invalid_argument("Unsupported channel count: " + std::to_string(channels));
            }
        }
    }

    string ext = filePath.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    fs::path actualPath = filePath;
    bool success = false;

    // Force PNG if grayscale
    if (channels == 1 && ext != ".png") {
        cout << "Warning: Grayscale images are only supported in PNG format. Forcing .png extension.\n";
        actualPath.replace_extension(".png");
        filePath = actualPath; // 🔁 Update the original path
    }

    cout << "Saving image...\nchannels: " << channels << endl;

    if (actualPath.extension() == ".png") {
        success = stbi_write_png(actualPath.string().c_str(), width, height, channels, outputData.data(), width * channels);
    } else if (actualPath.extension() == ".jpg" || actualPath.extension() == ".jpeg") {
        success = stbi_write_jpg(actualPath.string().c_str(), width, height, channels, outputData.data(), 100);
    } else if (actualPath.extension() == ".bmp") {
        success = stbi_write_bmp(actualPath.string().c_str(), width, height, channels, outputData.data());
    } else if (actualPath.extension() == ".tga") {
        success = stbi_write_tga(actualPath.string().c_str(), width, height, channels, outputData.data());
    } else {
        throw std::invalid_argument("Unsupported file extension: " + actualPath.extension().string());
    }

    if (!success) {
        throw std::runtime_error("Failed to save image to: " + actualPath.string());
    }

    cout << "Image successfully saved to: " << actualPath << endl;
}
