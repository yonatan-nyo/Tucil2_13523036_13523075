#include "QuadTreeNode.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

const double C1 = (0.01 * 255) * (0.01 * 255);
const double C2 = (0.03 * 255) * (0.03 * 255);
using namespace std;

double QuadTreeNode::computeError(int pickMethod, const Image &image, int fromX, int fromY, int toX, int toY) const {
    Pixel errorPixel;
    double errorValue = 0.0;

    if (pickMethod == 1) {
        return getVarianceError(image, fromX, fromY, toX, toY);
    } else if (pickMethod == 2) {
        return getMeanAbsoluteDeviationError(image, fromX, fromY, toX, toY);
    } else if (pickMethod == 3) {
        errorPixel = getMaxPixelDifference(image, fromX, fromY, toX, toY);
        double maxErrorPixel = errorPixel.r;
        if (image.channels > 1) {
            maxErrorPixel = max(maxErrorPixel, static_cast<double>(errorPixel.g));
        }
        if (image.channels > 2) {
            maxErrorPixel = max(maxErrorPixel, static_cast<double>(errorPixel.b));
        }
        if (image.channels > 3) {
            maxErrorPixel = max(maxErrorPixel, static_cast<double>(errorPixel.a));
        }
        return maxErrorPixel;
    } else if (pickMethod == 4) {
        return getEntropyError(image, fromX, fromY, toX, toY);
    } else if (pickMethod == 5) {
        // For SSIM, we want error = 1 - SSIM since SSIM of 1 means perfect similarity
        // Create a temporary image with just the mean pixel value for comparison
        Image meanImage = createMeanImage(image, fromX, fromY, toX, toY);
        errorValue = 1.0 - getSSIM(image, meanImage, fromX, fromY, toX, toY);
        return errorValue;
    } else {
        throw invalid_argument("Invalid pick method.");
    }
}

Image QuadTreeNode::createMeanImage(const Image &sourceImage, int fromX, int fromY, int toX, int toY) const {
    Image meanImage;
    meanImage.width = toX - fromX;
    meanImage.height = toY - fromY;
    meanImage.channels = sourceImage.channels;

    // Initialize the pixel data
    meanImage.pixels.resize(meanImage.height);
    for (int y = 0; y < meanImage.height; y++) {
        meanImage.pixels[y].resize(meanImage.width, meanPixel);
    }

    return meanImage;
}

double QuadTreeNode::calculateError(const double r, const double g, const double b, const double a, const int channels) const {
    double error = 0.0;
    if (channels >= 1)
        error += r / channels;
    if (channels >= 2)
        error += g / channels;
    if (channels >= 3)
        error += b / channels;
    if (channels >= 4)
        error += a / channels;
    return error;
}

double QuadTreeNode::getVarianceError(const Image &image, int fromX, int fromY, int toX, int toY) const {
    double retR = 0, retG = 0, retB = 0, retA = 0;
    int count = (toX - fromX) * (toY - fromY);

    if (count == 0)
        return 0.0;

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

    return calculateError(retR, retG, retB, retA, image.channels);
}

double QuadTreeNode::getMeanAbsoluteDeviationError(const Image &image, int fromX, int fromY, int toX, int toY) const {
    double retR = 0, retG = 0, retB = 0, retA = 0;
    int count = (toX - fromX) * (toY - fromY);

    if (count == 0)
        return 0.0;

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
    return calculateError(retR, retG, retB, retA, image.channels);
}

Pixel QuadTreeNode::getMaxPixelDifference(const Image &image, int fromX, int fromY, int toX, int toY) const {
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

double QuadTreeNode::getEntropyError(const Image &image, int fromX, int fromY, int toX, int toY) const {
    unordered_map<int, int> freqR, freqG, freqB, freqA;
    int totalPixels = (toX - fromX) * (toY - fromY);

    if (totalPixels == 0)
        return 0.0;

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

    return calculateError(entropyR, entropyG, entropyB, entropyA, image.channels);
}

double QuadTreeNode::SSIMmean(const Image &img, int fx, int fy, int tx, int ty, int channel) const {
    double sum = 0.0;
    int count = (tx - fx) * (ty - fy);
    if (count == 0)
        return 0.0;

    for (int y = fy; y < ty; ++y) {
        for (int x = fx; x < tx; ++x) {
            const Pixel &p = img.pixels[y - fy][x - fx];
            if (channel == 0)
                sum += p.r;
            else if (channel == 1)
                sum += p.g;
            else if (channel == 2)
                sum += p.b;
            else if (channel == 3)
                sum += p.a;
        }
    }
    return sum / count;
}

double QuadTreeNode::SSIMvariance(const Image &img, int fx, int fy, int tx, int ty, int channel, double mu) const {
    double sumSq = 0.0;
    int count = (tx - fx) * (ty - fy);
    if (count == 0)
        return 0.0;

    for (int y = fy; y < ty; ++y) {
        for (int x = fx; x < tx; ++x) {
            const Pixel &p = img.pixels[y - fy][x - fx];
            double val = 0.0;

            if (channel == 0)
                val = p.r;
            else if (channel == 1)
                val = p.g;
            else if (channel == 2)
                val = p.b;
            else if (channel == 3)
                val = p.a;

            sumSq += (val - mu) * (val - mu);
        }
    }
    return sumSq / count;
}

double QuadTreeNode::SSIMcovariance(const Image &img1, const Image &img2, int fx, int fy, int tx, int ty, int channel, double mu1, double mu2) const {
    double sumCov = 0.0;
    int count = (tx - fx) * (ty - fy);
    if (count == 0)
        return 0.0;

    for (int y = fy; y < ty; ++y) {
        for (int x = fx; x < tx; ++x) {
            const Pixel &p1 = img1.pixels[y][x];
            const Pixel &p2 = img2.pixels[y - fy][x - fx];
            double val1 = 0.0, val2 = 0.0;

            if (channel == 0) {
                val1 = p1.r;
                val2 = p2.r;
            } else if (channel == 1) {
                val1 = p1.g;
                val2 = p2.g;
            } else if (channel == 2) {
                val1 = p1.b;
                val2 = p2.b;
            } else if (channel == 3) {
                val1 = p1.a;
                val2 = p2.a;
            }

            sumCov += (val1 - mu1) * (val2 - mu2);
        }
    }
    return sumCov / count;
}

double QuadTreeNode::getSSIM(const Image &original, const Image &compressed, int fromX, int fromY, int toX, int toY) const {
    int channels = original.channels;

    if (channels <= 0 || channels > 4) {
        throw std::invalid_argument("Unsupported number of channels.");
    }

    vector<double> weights(channels, 1.0);
    if (channels == 3) {
        weights = {0.2125, 0.7154, 0.0721};
    } else if (channels == 4) {
        weights = {0.2125, 0.7154, 0.0721, 0.1};
    }

    double totalWeight = 0.0;
    for (double w : weights)
        totalWeight += w;

    double totalSSIM = 0.0;

    for (int c = 0; c < channels; ++c) {
        double mu1 = SSIMmean(original, fromX, fromY, toX, toY, c);
        double mu2 = SSIMmean(compressed, fromX, fromY, toX, toY, c);

        double var1 = SSIMvariance(original, fromX, fromY, toX, toY, c, mu1);
        double var2 = SSIMvariance(compressed, fromX, fromY, toX, toY, c, mu2);

        double cov = SSIMcovariance(original, compressed, fromX, fromY, toX, toY, c, mu1, mu2);

        double numerator = (2 * mu1 * mu2 + C1) * (2 * cov + C2);
        double denominator = (mu1 * mu1 + mu2 * mu2 + C1) * (var1 + var2 + C2);

        double ssim = (denominator == 0.0) ? 1.0 : numerator / denominator;
        totalSSIM += ssim * weights[c];
    }

    return totalSSIM / totalWeight;
}