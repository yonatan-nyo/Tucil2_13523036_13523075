#include <filesystem>
#include <iostream>
#include <string>

#include "utils/image/Image.hpp"
#include "utils/quadtree/QuadTreeNode.hpp"

namespace fs = std::filesystem;
using namespace std;

void initialize(string &inputFilename, string &resultPath, int &pickMethod, int &treshold, int &minBlockSize, Image &inputImage) {
    // Input image filename
    cout << "Enter image file name: ";
    getline(cin, inputFilename);

    // Get absolute file path
    fs::path inputPath(inputFilename);
    fs::path absolutePath = fs::absolute(inputPath);

    // Check if file exists
    cout << "Checking file at: " << absolutePath << endl;

    if (!fs::exists(absolutePath)) {
        throw runtime_error("Error: File " + absolutePath.string() + " does not exist.");
    }

    cout << "File exists, initializing image." << endl;
    inputImage = Image(absolutePath.string()); // Initialize image
    cout << "Image initialized." << endl;
    cout << inputImage << endl;

    // Choose error calculation method
    cout << "Pilih Metode Perhitungan Error" << endl;
    cout << "1. Variance" << endl;
    cout << "2. Mean Absolute Deviation" << endl;
    cout << "3. Max Pixel Difference" << endl;
    cout << "4. Entropy" << endl;
    cout << "5. Structural Similarity Index" << endl;
    cout << "Masukkan metode (1-5): ";
    cin >> pickMethod;
    cout << endl;

    cout << "Masukkan Treshold (ambang batas): ";
    cin >> treshold;
    cout << "Masukkan ukuran blok minimum: ";
    cin >> minBlockSize;
    cout << "Masukkan alamat file hasil: ";
    cin >> resultPath;
    cout << endl;

    fs::path resultPathObj(resultPath);
    fs::path resultAbsolutePath = fs::absolute(resultPathObj);
    cout << "File hasil akan disimpan di: " << resultAbsolutePath << endl;
}

int main() {
    try {
        string inputFilename, resultPath;
        int pickMethod, treshold, minBlockSize;
        Image inputImage;
        initialize(inputFilename, resultPath, pickMethod, treshold, minBlockSize, inputImage);

        cout << "Processing image with method " << pickMethod << " and threshold " << treshold << endl;
        cout << "Minimum block size: " << minBlockSize << endl;
        QuadTreeNode quadRoot(inputImage, pickMethod, treshold, minBlockSize);
        cout << "QuadTreeNode initialized." << endl;
        // quadRoot.debugTree();

        cout << "Building matrix..." << endl;
        vector<vector<Pixel>> imageMatrix(inputImage.height, vector<Pixel>(inputImage.width));
        quadRoot.buildMatrix(imageMatrix);
        cout << "Matrix built." << endl;

        cout << "Saving image..." << endl;
        Image::saveFromMatrix(imageMatrix, inputImage.width, inputImage.height, resultPath);
        cout << "Image saved." << endl;

    } catch (const exception &e) {
        cerr << "Exception: " << e.what() << endl;
        return 1;
    }

    return 0;
}

/*
my absolute test path
/mnt/c/Users/Pongo/Documents/Yonatan/University/ITB/0.Teknik-Informatika/Semester-4/Strategi-dan-Algoritma/tucil2/Tucil2_13523036_13523075/test/a.png
*/
