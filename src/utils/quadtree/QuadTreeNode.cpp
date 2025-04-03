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
            double diffR = p.r - meanPixel.r;
            double diffG = p.g - meanPixel.g;
            double diffB = p.b - meanPixel.b;
            double diffA = p.a - meanPixel.a;

            retR += (diffR * diffR) / count;
            retG += (diffG * diffG) / count;
            retB += (diffB * diffB) / count;
            retA += (diffA * diffA) / count;
        }
    }

    return Pixel(retR, retG, retB, retA);
}

Pixel QuadTreeNode::getMeanAbsoluteDeviation(Image &image, int fromX, int fromY, int toX, int toY) const {
    double retR = 0, retG = 0, retB = 0, retA = 0;
    int count = (toX - fromX) * (toY - fromY);

    if (count == 0)
        return Pixel(0, 0, 0, 0);

    for (int y = fromY; y < toY; ++y) {
        for (int x = fromX; x < toX; ++x) {
            Pixel p = image.pixels[y][x];
            retR += abs(p.r - meanPixel.r) / count;
            retG += abs(p.g - meanPixel.g) / count;
            retB += abs(p.b - meanPixel.b) / count;
            retA += abs(p.a - meanPixel.a) / count;
        }
    }

    return Pixel(retR, retG, retB, retA);
}

Pixel QuadTreeNode::getMaxPixelDifference(Image &image, int fromX, int fromY, int toX, int toY) const {
    int maxDiffR = 0, maxDiffG = 0, maxDiffB = 0, maxDiffA = 0;

    for (int y = fromY; y < toY; ++y) {
        for (int x = fromX; x < toX; ++x) {
            Pixel p = image.pixels[y][x];
            maxDiffR = max(maxDiffR, abs(p.r - meanPixel.r));
            maxDiffG = max(maxDiffG, abs(p.g - meanPixel.g));
            maxDiffB = max(maxDiffB, abs(p.b - meanPixel.b));
            maxDiffA = max(maxDiffA, abs(p.a - meanPixel.a));
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

    int entropyRGB = static_cast<int>((entropyR + entropyG + entropyB + entropyA) / 4.0);

    entropyPixel.r = entropyRGB;
    entropyPixel.g = entropyRGB;
    entropyPixel.b = entropyRGB;
    entropyPixel.a = entropyRGB;

    return entropyPixel;
}

QuadTreeNode::QuadTreeNode(Image &image, int pickMethod, double threshold, int minBlockSize)
    : width(image.width), height(image.height), divided(false), northwest(nullptr), northeast(nullptr), southwest(nullptr), southeast(nullptr) {
    if (image.getPixelCount() == 0) {
        cout << "[QuadTree] Image is empty." << endl;
        return;
    }
    fromX = 0;
    fromY = 0;

    buildTree(image, 0, 0, width, height, pickMethod, threshold, minBlockSize);
}

void QuadTreeNode::buildTree(Image &image, int fromX, int fromY, int toX, int toY, int pickMethod, double threshold, int minBlockSize) {
    width = toX - fromX;
    height = toY - fromY;

    if (width <= 0 || height <= 0) {
        throw invalid_argument("Invalid dimensions for QuadTreeNode.");
    }

    if ((width * height) > minBlockSize && width > 1 && height > 1) {
        int midX = (fromX + toX) / 2;
        int midY = (fromY + toY) / 2;
        divided = true;

        // Create subtrees with correct boundaries
        northwest = new QuadTreeNode(image, fromX, fromY, midX, midY, pickMethod, threshold, minBlockSize);
        northeast = new QuadTreeNode(image, midX, fromY, toX, midY, pickMethod, threshold, minBlockSize);
        southwest = new QuadTreeNode(image, fromX, midY, midX, toY, pickMethod, threshold, minBlockSize);
        southeast = new QuadTreeNode(image, midX, midY, toX, toY, pickMethod, threshold, minBlockSize);

        vector<Pixel> pixelGroup = {northwest->meanPixel, northeast->meanPixel, southwest->meanPixel, southeast->meanPixel};
        meanPixel = computeAveragePixel(pixelGroup);

        double error = computeError(pickMethod, image, fromX, fromY, toX, toY);
        if (error < threshold) {
            deleteChildren();
            divided = false;
        }
    } else {
        meanPixel = image.getMean(fromX, fromY, toX, toY);
    }
}

QuadTreeNode::QuadTreeNode(Image &image, int fromX, int fromY, int toX, int toY, int pickMethod, double threshold, int minBlockSize) {
    this->fromX = fromX;
    this->fromY = fromY;
    width = toX - fromX;
    height = toY - fromY;
    if (width <= 0 || height <= 0) {
        throw invalid_argument("Invalid dimensions for QuadTreeNode.");
    }

    if ((width * height) > minBlockSize && width > 1 && height > 1) {
        divided = true;
        int midX = (fromX + toX) / 2;
        int midY = (fromY + toY) / 2;

        northwest = new QuadTreeNode(image, fromX, fromY, midX, midY, pickMethod, threshold, minBlockSize);
        northeast = new QuadTreeNode(image, midX, fromY, toX, midY, pickMethod, threshold, minBlockSize);
        southwest = new QuadTreeNode(image, fromX, midY, midX, toY, pickMethod, threshold, minBlockSize);
        southeast = new QuadTreeNode(image, midX, midY, toX, toY, pickMethod, threshold, minBlockSize);

        vector<Pixel> pixelGroup = {northwest->meanPixel, northeast->meanPixel, southwest->meanPixel, southeast->meanPixel};
        meanPixel = computeAveragePixel(pixelGroup);

        double error = computeError(pickMethod, image, fromX, fromY, toX, toY);
        if (error < threshold) {
            deleteChildren();
            divided = false;
        }
    } else {
        divided = false;
        meanPixel = image.getMean(fromX, fromY, toX, toY);
    }
}

void QuadTreeNode::deleteChildren() {
    delete northwest;
    northwest = nullptr;
    delete northeast;
    northeast = nullptr;
    delete southwest;
    southwest = nullptr;
    delete southeast;
    southeast = nullptr;
}

QuadTreeNode::~QuadTreeNode() {
    deleteChildren();
}

Pixel QuadTreeNode::computeAveragePixel(vector<Pixel> &pixels) const {
    int retR = 0, retG = 0, retB = 0, retA = 0;
    int count = pixels.size();

    if (count == 0)
        return Pixel(0, 0, 0, 0);

    for (const auto &p : pixels) {
        retR += p.r;
        retG += p.g;
        retB += p.b;
        retA += p.a;
    }

    return Pixel(retR / count, retG / count, retB / count, retA / count);
}

void QuadTreeNode::showTree(int depth) const {
    for (int i = 0; i < depth; ++i)
        cout << "  ";
    cout << "[" << width << "x" << height << (divided ? "] - Subdivided" : "] - Leaf") << " " << meanPixel << endl;
    for (int i = 0; i < depth; ++i)
        cout << "  ";
    cout << "\\from (" << fromX << ", " << fromY << ") to (" << fromX + width << ", " << fromY + height << ")" << endl;
    if (divided) {
        northwest->showTree(depth + 1);
        northeast->showTree(depth + 1);
        southwest->showTree(depth + 1);
        southeast->showTree(depth + 1);
    }
}

void QuadTreeNode::debugTree() const {
    cout << "\n[QuadTree Structure]" << endl;
    showTree(0);
}

void QuadTreeNode::buildMatrix(vector<vector<Pixel>> &imageMatrix) const {
    if (divided) {
        northwest->buildMatrix(imageMatrix);
        northeast->buildMatrix(imageMatrix);
        southwest->buildMatrix(imageMatrix);
        southeast->buildMatrix(imageMatrix);
    } else {
        for (int y = fromY; y < fromY + height; ++y) {
            for (int x = fromX; x < fromX + width; ++x) {
                imageMatrix[y][x] = meanPixel;
            }
        }
    }
}

pair<int, int> QuadTreeNode::getStat() const {
    if (!divided) {
        return {1, 1}; // Depth = 1, Node count = 1 (leaf node)
    } else {
        pair<int, int> northwestStat = northwest ? northwest->getStat() : make_pair(0, 0);
        pair<int, int> northeastStat = northeast ? northeast->getStat() : make_pair(0, 0);
        pair<int, int> southwestStat = southwest ? southwest->getStat() : make_pair(0, 0);
        pair<int, int> southeastStat = southeast ? southeast->getStat() : make_pair(0, 0);

        int depth = 1 + max({northwestStat.first, northeastStat.first, southwestStat.first, southeastStat.first});
        int nodeCount = 1 + northwestStat.second + northeastStat.second + southwestStat.second + southeastStat.second;

        return {depth, nodeCount};
    }
}
