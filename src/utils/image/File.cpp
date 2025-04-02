#include "File.hpp"

long long File::getFileSize(const filesystem::path &filePath) {
    if (!filesystem::exists(filePath)) {
        throw runtime_error("File does not exist: " + filePath.string());
    }
    return filesystem::file_size(filePath);
}