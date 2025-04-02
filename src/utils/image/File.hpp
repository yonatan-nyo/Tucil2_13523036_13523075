#ifndef UTILS_IMAGE_FILE_HPP
#define UTILS_IMAGE_FILE_HPP

#include <filesystem>

using namespace std;

class File {
public:
    static long long getFileSize(const filesystem::path &filePath);
};

#endif