#ifndef UTILS_IMAGE_IMAGE_HPP
#define UTILS_IMAGE_IMAGE_HPP

#include "Pixel.hpp"
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Image {
public:
    int width, height, channels;
    vector<vector<Pixel>> pixels;

    Image() : width(0), height(0) {}
    Image(int width, int height) : width(width), height(height) {
        pixels.resize(height, vector<Pixel>(width));
    }
    Image(const string &filename);

    int getPixelCount() const;
    Pixel getMean(int fromX, int fromY, int toX, int toY) const;

    static bool saveFromMatrix(const vector<vector<Pixel>> &imageMatrix, int width, int height, const string &filePath);

    friend ostream &operator<<(ostream &os, const Image &image);
};

#endif
