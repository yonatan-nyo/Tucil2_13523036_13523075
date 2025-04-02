#include "image.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
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

Pixel Image::getVariance(int fromX, int fromY, int toX, int toY) const {
    Pixel mean = getMean(fromX, fromY, toX, toY);
    double sumR = 0, sumG = 0, sumB = 0, sumA = 0;
    int count = (toX - fromX) * (toY - fromY);

    if (count == 0)
        return Pixel(0, 0, 0, 0);

    for (int y = fromY; y < toY; ++y) {
        for (int x = fromX; x < toX; ++x) {
            const Pixel &p = pixels[y][x];
            sumR += pow(p.r - mean.r, 2) / count;
            sumG += pow(p.g - mean.g, 2) / count;
            sumB += pow(p.b - mean.b, 2) / count;
            sumA += pow(p.a - mean.a, 2) / count;
        }
    }

    return Pixel(
        static_cast<unsigned char>(min(255.0, sumR)),
        static_cast<unsigned char>(min(255.0, sumG)),
        static_cast<unsigned char>(min(255.0, sumB)),
        static_cast<unsigned char>(min(255.0, sumA)));
}

Pixel Image::getMeanAbsoluteDeviation(int fromX, int fromY, int toX, int toY) const {
    Pixel mean = getMean(fromX, fromY, toX, toY);
    double sumR = 0, sumG = 0, sumB = 0, sumA = 0;
    int count = (toX - fromX) * (toY - fromY);

    if (count == 0)
        return Pixel(0, 0, 0, 0);

    for (int y = fromY; y < toY; ++y) {
        for (int x = fromX; x < toX; ++x) {
            const Pixel &p = pixels[y][x];
            sumR += abs(p.r - mean.r) / count;
            sumG += abs(p.g - mean.g) / count;
            sumB += abs(p.b - mean.b) / count;
            sumA += abs(p.a - mean.a) / count;
        }
    }

    return Pixel(
        static_cast<unsigned char>(min(255.0, sumR)),
        static_cast<unsigned char>(min(255.0, sumG)),
        static_cast<unsigned char>(min(255.0, sumB)),
        static_cast<unsigned char>(min(255.0, sumA)));
}

Pixel Image::getMaxPixelDifference(int fromX, int fromY, int toX, int toY) const {
    int maxDiffR = 0, maxDiffG = 0, maxDiffB = 0, maxDiffA = 0;

    for (int y = fromY; y < toY; ++y) {
        for (int x = fromX; x < toX; ++x) {
            const Pixel &p = pixels[y][x];
            maxDiffR = max(maxDiffR, abs(p.r - 0));
            maxDiffG = max(maxDiffG, abs(p.g - 0));
            maxDiffB = max(maxDiffB, abs(p.b - 0));
            maxDiffA = max(maxDiffA, abs(p.a - 0));
        }
    }

    return Pixel(
        static_cast<unsigned char>(min(255, maxDiffR)),
        static_cast<unsigned char>(min(255, maxDiffG)),
        static_cast<unsigned char>(min(255, maxDiffB)),
        static_cast<unsigned char>(min(255, maxDiffA)));
}

Pixel Image::getEntropy(int fromX, int fromY, int toX, int toY) const {
    unordered_map<unsigned char, int> freqR, freqG, freqB, freqA;

    int count = (toX - fromX) * (toY - fromY);

    if (count == 0)
        return Pixel(0, 0, 0, 0);

    // Count the frequency of each color channel
    for (int y = fromY; y < toY; ++y) {
        for (int x = fromX; x < toX; ++x) {
            const Pixel &p = pixels[y][x];
            freqR[p.r]++;
            freqG[p.g]++;
            freqB[p.b]++;
            freqA[p.a]++;
        }
    }

    // Calculate entropy for each channel
    double entropyR = 0, entropyG = 0, entropyB = 0, entropyA = 0;

    for (const auto &entry : freqR) {
        double p = static_cast<double>(entry.second) / count;
        entropyR -= p * log2(p);
    }
    for (const auto &entry : freqG) {
        double p = static_cast<double>(entry.second) / count;
        entropyG -= p * log2(p);
    }
    for (const auto &entry : freqB) {
        double p = static_cast<double>(entry.second) / count;
        entropyB -= p * log2(p);
    }
    for (const auto &entry : freqA) {
        double p = static_cast<double>(entry.second) / count;
        entropyA -= p * log2(p);
    }

    // Normalize entropy and return as a Pixel (clamp to 255)
    return Pixel(
        static_cast<unsigned char>(min(255.0, entropyR)),
        static_cast<unsigned char>(min(255.0, entropyG)),
        static_cast<unsigned char>(min(255.0, entropyB)),
        static_cast<unsigned char>(min(255.0, entropyA)));
}