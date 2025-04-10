#include "GIFMaker.hpp"
#include "../../include/gif.h"
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;
using namespace std;

void GIFMaker::generateAndSave(int maxDepth, QuadTreeNode *quadRoot, int width, int height, fs::path &savePath) {
    cout << "Saving GIF to: " << savePath << endl;
    GifWriter writer;

    vector<vector<Pixel>> imageMatrix;
    imageMatrix.resize(height, vector<Pixel>(width));

    if (!GifBegin(&writer, savePath.string().c_str(), width, height, 10)) {
        cerr << "Gagal membuat GIF\n";
        return;
    }

    for (int currentDepth = 1; currentDepth <= maxDepth; currentDepth++) {
        quadRoot->reconstructImageMatrix(imageMatrix, currentDepth);

        vector<uint8_t> frameData(width * height * 4);
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int idx = (y * width + x) * 4;
                const Pixel &p = imageMatrix[y][x];
                frameData[idx + 0] = p.r;
                frameData[idx + 1] = p.g;
                frameData[idx + 2] = p.b;
                frameData[idx + 3] = 255;
            }
        }

        GifWriteFrame(&writer, frameData.data(), width, height, 50);
    }

    GifEnd(&writer);

    cout << "GIF saved successfully to: " << savePath << endl;
}