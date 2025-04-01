#include <filesystem>
#include <iostream>
#include <string>

#include "utils/image/Image.hpp"

namespace fs = std::filesystem;
using namespace std;

int main() {
    try {
        string inputFilename;
        cout << "Enter image file name: ";
        getline(cin, inputFilename);

        fs::path inputPath(inputFilename);
        fs::path absolutePath = fs::absolute(inputPath);

        cout << "Checking file at: " << absolutePath << endl;

        if (!fs::exists(absolutePath)) {
            throw runtime_error("Error: File " + absolutePath.string() + " does not exist.");
        }

        cout << "File exist, initializing image." << endl;

        cout << endl;
        Image image(absolutePath.string());
        cout << "Image initialized." << endl;
        cout << image << endl;

    } catch (const exception &e) {
        cerr << "Exception: " << e.what() << endl;
        return 1;
    }

    return 0;
}
