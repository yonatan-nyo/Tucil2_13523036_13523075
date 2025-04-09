#ifndef UTILS_GIF_GIF_HPP
#define UTILS_GIF_GIF_HPP

#include "../image/Pixel.hpp"
#include "../quadtree/QuadTreeNode.hpp"
#include <vector>

namespace fs = std::filesystem;
using namespace std;

class GIFMaker {
public:
    static void generateAndSave(int maxDepth, QuadTreeNode *quadRoot, int width, int height, fs::path &savePath);
    static void generateAndSaveHelper(int maxDepth, QuadTreeNode *quadRoot, int width, int height, vector<vector<Pixel>> &imageMatrix, fs::path &savePath);
};

#endif
