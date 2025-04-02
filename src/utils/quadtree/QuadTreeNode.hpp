#ifndef UTILS_QUADTREE_QUADTREE_NODE_HPP
#define UTILS_QUADTREE_QUADTREE_NODE_HPP

#include "../image/Image.hpp"
#include <unordered_map>
#include <vector>

using namespace std;

class QuadTreeNode {
private:
    static constexpr int CAPACITY = 4;
    int fromX, fromY;
    int width, height;
    bool divided = false;
    Pixel meanPixel;

    QuadTreeNode *northwest = nullptr;
    QuadTreeNode *northeast = nullptr;
    QuadTreeNode *southwest = nullptr;
    QuadTreeNode *southeast = nullptr;

    void showTree(int startIndentation) const;
    double computeEntropy(unordered_map<int, int> &frequencyMap, int totalPixels) const;
    Pixel computeAveragePixel(vector<Pixel> &pixels) const;
    double computeError(int pickMethod, vector<Pixel> &pixels, Pixel &meanPixel) const;
    void buildTree(Image &image, int fromX, int fromY, int toX, int toY, int pickMethod, int threshold, int minBlockSize);
    void deleteChildren();

public:
    QuadTreeNode() = default;
    QuadTreeNode(Image &image, int pickMethod, int treshold, int minBlockSize);
    QuadTreeNode(Image &image, int fromX, int fromY, int toX, int toY, int pickMethod, int treshold, int minBlockSize);
    ~QuadTreeNode();

    void debugTree() const;
    Pixel getVariance(vector<Pixel> &pixels, Pixel &meanPixel) const;
    Pixel getMeanAbsoluteDeviation(vector<Pixel> &pixels, Pixel &meanPixel) const;
    Pixel getMaxPixelDifference(vector<Pixel> &pixels, Pixel &meanPixel) const;
    Pixel getEntropy(vector<Pixel> &pixels) const;
    // Pixel getStructuralSimilarityIndex(vector<Pixel> &pixels, Pixel &meanPixel) const;

    void buildMatrix(vector<vector<Pixel>> &imageMatrix) const;
};

#endif
