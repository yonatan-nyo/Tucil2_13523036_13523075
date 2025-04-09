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
    double computeError(int pickMethod, Image &image, int fromX, int fromY, int toX, int toY) const;
    void buildTree(Image &image, int fromX, int fromY, int toX, int toY, int pickMethod, double threshold, int minBlockSize);
    bool isCanDivide(int width, int height, int minBlockSize) const;
    void deleteChildren();
    Image createMeanImage(const Image &sourceImage, int fromX, int fromY, int toX, int toY) const;
    double SSIMmean(const Image &img, int fx, int fy, int tx, int ty, int channel) const;
    double SSIMvariance(const Image &img, int fx, int fy, int tx, int ty, int channel, double mu) const;
    double SSIMcovariance(const Image &img1, const Image &img2, int fx, int fy, int tx, int ty, int channel, double mu1, double mu2) const;

public:
    QuadTreeNode() = default;
    QuadTreeNode(Image &image, int pickMethod, double treshold, int minBlockSize);
    QuadTreeNode(Image &image, int fromX, int fromY, int toX, int toY, int pickMethod, double treshold, int minBlockSize);
    ~QuadTreeNode();

    void debugTree() const;
    Pixel getVariance(Image &image, int fromX, int fromY, int toX, int toY) const;
    Pixel getMeanAbsoluteDeviation(Image &image, int fromX, int fromY, int toX, int toY) const;
    Pixel getMaxPixelDifference(Image &image, int fromX, int fromY, int toX, int toY) const;
    Pixel getEntropy(Image &image, int fromX, int fromY, int toX, int toY) const;
    double getSSIM(Image &original, Image &compressed, int fromX, int fromY, int toX, int toY) const;

    pair<int, int> getStat() const;
    void buildMatrix(vector<vector<Pixel>> &imageMatrix) const;
};

#endif
