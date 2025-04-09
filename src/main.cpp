#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>

#include "utils/image/File.hpp"
#include "utils/image/Image.hpp"
#include "utils/quadtree/QuadTreeNode.hpp"

namespace fs = std::filesystem;
using namespace std;

fs::path getDefaultPath(const fs::path &inputFilePath, int pickMethod, double threshold, int minBlockSize) {
    fs::path parentPath = inputFilePath.parent_path();
    string inputBase = inputFilePath.stem().string();
    string inputExtension = inputFilePath.extension().string();

    stringstream ss;
    ss << inputBase << "_" << pickMethod << "_" << threshold << "_" << minBlockSize << inputExtension;

    return parentPath / ss.str();
}

void initialize(fs::path &inputFilePath, fs::path &savePath, int &pickMethod, double &treshold, int &minBlockSize, Image &inputImage) {
    // Input image filename
    cout << "Enter image file name: ";
    string inputStr;
    getline(cin, inputStr);
    inputFilePath = fs::absolute(fs::path(inputStr));

    // Check if file exists
    cout << "Checking file at: " << inputFilePath << endl;

    if (!fs::exists(inputFilePath)) {
        throw runtime_error("Error: File " + inputFilePath.string() + " does not exist.");
    }

    cout << "File exists, initializing image." << endl;
    inputImage = Image(inputFilePath.string()); // Initialize image
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

    double minThreshold, maxThreshold;
    if (pickMethod == 1) {
        minThreshold = 0;
        maxThreshold = 65025.0;
    } else if (pickMethod == 2 || pickMethod == 3) {
        minThreshold = 0;
        maxThreshold = 255.0;
    } else if (pickMethod == 4) {
        minThreshold = 0;
        maxThreshold = 8.0;
    } else if (pickMethod == 5) {
        minThreshold = -1.0;
        maxThreshold = 1.0;
    } else {
        throw invalid_argument("Invalid method selected.");
    }

    cout << "Masukkan Treshold (ambang batas): ";
    cin >> treshold;
    if (treshold < minThreshold) {
        cout << "Treshold tidak boleh kurang dari " << minThreshold << ", di set ke " << minThreshold << endl;
        treshold = minThreshold;
    } else if (treshold > maxThreshold) {
        cout << "Treshold tidak boleh lebih dari " << maxThreshold << ", di set ke " << maxThreshold << endl;
        treshold = maxThreshold;
    }

    cout << "Masukkan ukuran blok minimum: ";
    cin >> minBlockSize;
    if (minBlockSize < 1) {
        cout << "Ukuran blok minimum tidak boleh kurang dari 1!" << endl;
        minBlockSize = 1;
        cout << "Ukuran blok minimum di set ke 1!" << endl;
    }

    fs::path defaultSavePath = getDefaultPath(inputFilePath, pickMethod, treshold, minBlockSize);
    cout << "Masukkan alamat file hasil (tekan Enter untuk default):" << endl;
    cout << "[Default: " << defaultSavePath << "]" << endl;

    cin.ignore();
    string savePathStr;
    getline(cin, savePathStr);

    if (savePathStr.empty()) {
        savePath = defaultSavePath;
        cout << "Tidak ada path diberikan. Menggunakan default: " << savePath << endl;
    } else {
        savePath = fs::absolute(fs::path(savePathStr));
    }

    cout << "File hasil akan disimpan di: " << savePath << endl;
}

int main() {
    try {
        fs::path inputFilePath, savePath;
        int pickMethod, minBlockSize;
        double treshold;
        Image inputImage;
        initialize(inputFilePath, savePath, pickMethod, treshold, minBlockSize, inputImage);

        cout << "Processing image with method " << pickMethod << " and threshold " << treshold << endl;
        cout << "Minimum block size: " << minBlockSize << endl;

        auto start = chrono::high_resolution_clock::now();

        QuadTreeNode quadRoot(inputImage, pickMethod, treshold, minBlockSize);
        cout << "QuadTreeNode initialized." << endl;

        cout << "Building matrix..." << endl;
        vector<vector<Pixel>> imageMatrix(inputImage.height, vector<Pixel>(inputImage.width));
        quadRoot.buildMatrix(imageMatrix);
        cout << "Matrix built." << endl;

        cout << "Saving image..." << endl;
        Image::saveFromMatrix(imageMatrix, inputImage.width, inputImage.height, inputImage.channels, savePath);
        cout << "Image saved to: " << savePath << endl;
        cout << endl;

        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end - start;
        cout << "====== stats =======" << endl;
        cout << "Waktu Eksekusi: " << duration.count() << " seconds" << endl;

        long long inputSize = File::getFileSize(inputFilePath.string());
        long long outputSize = File::getFileSize(savePath.string());
        cout << "Ukuran Gambar sebelum: " << inputSize << " bit" << endl;
        cout << "Ukuran Gambar setelah: " << outputSize << " bit" << endl;
        cout << "Persentase kompresi: " << (1 - (static_cast<double>(outputSize) / inputSize)) * 100 << "%" << endl;

        auto stat = quadRoot.getStat();
        cout << "Kedalaman QuadTree: " << stat.first << endl;
        cout << "Jumlah Simpul: " << stat.second << endl;

    } catch (const exception &e) {
        cerr << "Exception: " << e.what() << endl;
        return 1;
    }

    return 0;
}
