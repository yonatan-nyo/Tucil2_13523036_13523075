#ifndef UTILS_IMAGE_IMAGE_HPP
#define UTILS_IMAGE_IMAGE_HPP

#include "Pixel.hpp"
#include <string>
#include <vector>

using namespace std;

class Image {
public:
    int width, height, channels;
    std::vector<std::vector<Pixel>> pixels;

    Image() : width(0), height(0) {}
    Image(int width, int height) : width(width), height(height) {
        pixels.resize(height, std::vector<Pixel>(width));
    }
    Image(const string &filename);

    int getPixelCount() const;

    // toX and toY not included (exclusive)
    Pixel getMean(int fromX, int fromY, int toX, int toY) const;
    Pixel getVariance(int fromX, int fromY, int toX, int toY) const;
    // Pixel getMeanAbsoluteDeviation(int fromX, int fromY, int toX, int toY) const;
    // Pixel getMaxPixelDifference(int fromX, int fromY, int toX, int toY) const;
    // Pixel getEntropy(int fromX, int fromY, int toX, int toY) const;
    // Pixel getStructuralSimilarityIndex(int fromX, int fromY, int toX, int toY) const;

    friend ostream &operator<<(ostream &os, const Image &image);
};

#endif