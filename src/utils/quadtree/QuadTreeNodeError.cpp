#include "QuadTreeNode.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

double QuadTreeNode::computeError(int pickMethod, Image &image, int fromX, int fromY, int toX, int toY) const {
    double error = 0.0;
    if (pickMethod == 1) {
        Pixel variancePixel = getVariance(image, fromX, fromY, toX, toY);
        error = (variancePixel.r + variancePixel.g + variancePixel.b + variancePixel.a) / 4.0;
    } else if (pickMethod == 2) {
        Pixel madPixel = getMeanAbsoluteDeviation(image, fromX, fromY, toX, toY);
        error = (madPixel.r + madPixel.g + madPixel.b + madPixel.a) / 4.0;
    } else if (pickMethod == 3) {
        Pixel maxDiffPixel = getMaxPixelDifference(image, fromX, fromY, toX, toY);
        error = (maxDiffPixel.r + maxDiffPixel.g + maxDiffPixel.b + maxDiffPixel.a) / 4.0;
    } else if (pickMethod == 4) {
        Pixel entropyPixel = getEntropy(image, fromX, fromY, toX, toY);
        error = (entropyPixel.r + entropyPixel.g + entropyPixel.b + entropyPixel.a) / 4.0;
    } else {
        throw invalid_argument("Invalid pick method.");
    }
    return error;
}

Pixel QuadTreeNode::getVariance(Image &image, int fromX, int fromY, int toX, int toY) const {
    double retR = 0, retG = 0, retB = 0, retA = 0;
    int count = (toX - fromX) * (toY - fromY);

    if (count == 0)
        return Pixel(0, 0, 0, 0);

    for (int y = fromY; y < toY; ++y) {
        for (int x = fromX; x < toX; ++x) {
            Pixel p = image.pixels[y][x];
            double diffR = static_cast<double>(p.r) - static_cast<double>(meanPixel.r);
            double diffG = static_cast<double>(p.g) - static_cast<double>(meanPixel.g);
            double diffB = static_cast<double>(p.b) - static_cast<double>(meanPixel.b);
            double diffA = static_cast<double>(p.a) - static_cast<double>(meanPixel.a);

            retR += (diffR * diffR) / count;
            retG += (diffG * diffG) / count;
            retB += (diffB * diffB) / count;
            retA += (diffA * diffA) / count;
        }
    }

    // Clamp values to valid unsigned char range
    return Pixel(
        min(255.0, max(0.0, retR)),
        min(255.0, max(0.0, retG)),
        min(255.0, max(0.0, retB)),
        min(255.0, max(0.0, retA)));
}

Pixel QuadTreeNode::getMeanAbsoluteDeviation(Image &image, int fromX, int fromY, int toX, int toY) const {
    double retR = 0, retG = 0, retB = 0, retA = 0;
    int count = (toX - fromX) * (toY - fromY);

    if (count == 0)
        return Pixel(0, 0, 0, 0);

    for (int y = fromY; y < toY; ++y) {
        for (int x = fromX; x < toX; ++x) {
            Pixel p = image.pixels[y][x];
            retR += abs(static_cast<double>(p.r) - static_cast<double>(meanPixel.r)) / count;
            retG += abs(static_cast<double>(p.g) - static_cast<double>(meanPixel.g)) / count;
            retB += abs(static_cast<double>(p.b) - static_cast<double>(meanPixel.b)) / count;
            retA += abs(static_cast<double>(p.a) - static_cast<double>(meanPixel.a)) / count;
        }
    }

    // Clamp values to valid unsigned char range
    return Pixel(
        min(255.0, max(0.0, retR)),
        min(255.0, max(0.0, retG)),
        min(255.0, max(0.0, retB)),
        min(255.0, max(0.0, retA)));
}

Pixel QuadTreeNode::getMaxPixelDifference(Image &image, int fromX, int fromY, int toX, int toY) const {
    int maxDiffR = 0, maxDiffG = 0, maxDiffB = 0, maxDiffA = 0;

    for (int y = fromY; y < toY; ++y) {
        for (int x = fromX; x < toX; ++x) {
            Pixel p = image.pixels[y][x];
            maxDiffR = max(maxDiffR, abs(static_cast<int>(p.r) - static_cast<int>(meanPixel.r)));
            maxDiffG = max(maxDiffG, abs(static_cast<int>(p.g) - static_cast<int>(meanPixel.g)));
            maxDiffB = max(maxDiffB, abs(static_cast<int>(p.b) - static_cast<int>(meanPixel.b)));
            maxDiffA = max(maxDiffA, abs(static_cast<int>(p.a) - static_cast<int>(meanPixel.a)));
        }
    }

    return Pixel(maxDiffR, maxDiffG, maxDiffB, maxDiffA);
}

double QuadTreeNode::computeEntropy(unordered_map<int, int> &frequencyMap, int totalPixels) const {
    double entropy = 0.0;

    for (const auto &[value, count] : frequencyMap) {
        double p = static_cast<double>(count) / totalPixels;
        if (p > 0)
            entropy -= p * log2(p);
    }

    return entropy;
}

Pixel QuadTreeNode::getEntropy(Image &image, int fromX, int fromY, int toX, int toY) const {
    Pixel entropyPixel = {0, 0, 0, 0};
    unordered_map<int, int> freqR, freqG, freqB, freqA;
    int totalPixels = (toX - fromX) * (toY - fromY);

    if (totalPixels == 0)
        return entropyPixel;

    for (int y = fromY; y < toY; ++y) {
        for (int x = fromX; x < toX; ++x) {
            Pixel p = image.pixels[y][x];
            freqR[p.r]++;
            freqG[p.g]++;
            freqB[p.b]++;
            freqA[p.a]++;
        }
    }

    double entropyR = computeEntropy(freqR, totalPixels);
    double entropyG = computeEntropy(freqG, totalPixels);
    double entropyB = computeEntropy(freqB, totalPixels);
    double entropyA = computeEntropy(freqA, totalPixels);

    // Clamp entropy values to valid unsigned char range
    double entropyRGB = (entropyR + entropyG + entropyB + entropyA) / 4.0;
    unsigned char clampedEntropy = static_cast<unsigned char>(min(255.0, max(0.0, entropyRGB)));

    entropyPixel.r = clampedEntropy;
    entropyPixel.g = clampedEntropy;
    entropyPixel.b = clampedEntropy;
    entropyPixel.a = clampedEntropy;

    return entropyPixel;
}
