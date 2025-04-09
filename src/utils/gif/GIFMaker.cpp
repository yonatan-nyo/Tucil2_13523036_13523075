#include "GIFMaker.hpp"
#include "../../include/gif.h"
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;

void GIFMaker::generateAndSave(int maxDepth, QuadTreeNode *quadRoot, int width, int height, fs::path &savePath) {
    GifWriter writer;

    if (!GifBegin(&writer, savePath.string().c_str(), width, height, 10)) {
        std::cerr << "Gagal membuat GIF\n";
        return;
    }

    for (int i = 1; i <= maxDepth; i++) {
    }
}