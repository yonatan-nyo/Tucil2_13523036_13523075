#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>

#include "utils/gif/GIFMaker.hpp"
#include "utils/image/File.hpp"
#include "utils/image/Image.hpp"
#include "utils/quadtree/QuadTreeNode.hpp"

namespace fs = std::filesystem;
using namespace std;

fs::path getDefaultPath(const fs::path &inputFilePath, int pickMethod, double threshold, int minBlockSize, double percentageCompression = 0.0) {
    fs::path parentPath = inputFilePath.parent_path();
    string inputBase = inputFilePath.stem().string();
    string inputExtension = inputFilePath.extension().string();

    stringstream ss;
    if (percentageCompression != 0) {
        ss << inputBase << "_percentage_" << percentageCompression << inputExtension;
    } else {
        ss << inputBase << "_" << pickMethod << "_" << threshold << "_" << minBlockSize << inputExtension;
    }

    return parentPath / ss.str();
}

fs::path getDefaultGifPath(const fs::path &inputFilePath, int pickMethod, double threshold, int minBlockSize, double percentageCompression = 0.0) {
    fs::path parentPath = inputFilePath.parent_path();
    string inputBase = inputFilePath.stem().string();
    string inputExtension = ".gif";

    stringstream ss;
    if (percentageCompression != 0) {
        ss << inputBase << "_percentage_" << percentageCompression << inputExtension;
    } else {
        ss << inputBase << "_" << pickMethod << "_" << threshold << "_" << minBlockSize << inputExtension;
    }

    return parentPath / ss.str();
}

void initialize(fs::path &inputFilePath, fs::path &savePath, int &pickMethod, double &threshold, int &minBlockSize, Image &inputImage, double &percentageCompression) {
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

    // check if user wants to user percentage compression
    cout << "Do you want to use percentage compression? (0.0 - 1.0) (default: 0): ";
    cin >> percentageCompression;
    if (percentageCompression != 0) {
        cout << "Using percentage compression: " << percentageCompression << endl;
        pickMethod = 1; // set to variance, using 0 to 65025.0 as threshold using binary search to get the right threshold
        threshold = 0.0;
        minBlockSize = 1;
    } else {
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

        cout << "Masukkan Threshold (ambang batas): ";
        cin >> threshold;
        if (threshold < minThreshold) {
            cout << "Threshold tidak boleh kurang dari " << minThreshold << ", di set ke " << minThreshold << endl;
            threshold = minThreshold;
        } else if (threshold > maxThreshold) {
            cout << "Threshold tidak boleh lebih dari " << maxThreshold << ", di set ke " << maxThreshold << endl;
            threshold = maxThreshold;
        }

        cout << "Masukkan ukuran blok minimum: ";
        cin >> minBlockSize;
        if (minBlockSize < 1) {
            cout << "Ukuran blok minimum tidak boleh kurang dari 1!" << endl;
            minBlockSize = 1;
            cout << "Ukuran blok minimum di set ke 1!" << endl;
        }
    }

    fs::path defaultSavePath = getDefaultPath(inputFilePath, pickMethod, threshold, minBlockSize, percentageCompression);
    cout << "Masukkan alamat file hasil" << endl;
    cout << "[Default: " << defaultSavePath << "]" << endl;
    cout << "(tekan Enter untuk default): ";

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
        double threshold;
        Image inputImage;
        double percentageCompression;
        initialize(inputFilePath, savePath, pickMethod, threshold, minBlockSize, inputImage, percentageCompression);

        if (percentageCompression != 0) {
            cout << "Melakukan pencarian threshold otomatis dengan target kompresi: " << percentageCompression * 100 << "%" << endl;

            auto start = chrono::high_resolution_clock::now();

            // Setup untuk binary search
            pickMethod = 1; // Variance
            minBlockSize = 1;
            double low = 0.0, high = 65025.0, bestThreshold = 0.0, binarySearchStep = 0.000001;

            fs::path tempPath = inputFilePath.parent_path() / ("temp_output" + savePath.extension().string());

            while (low <= high) {
                double mid = (low + high) / 2.0;

                QuadTreeNode tempNode(inputImage, pickMethod, mid, minBlockSize);
                vector<vector<Pixel>> tempMatrix(inputImage.height, vector<Pixel>(inputImage.width));
                tempNode.buildMatrix(tempMatrix);

                Image::saveFromMatrix(tempMatrix, inputImage.width, inputImage.height, inputImage.channels, tempPath);

                long long inputSize = File::getFileSize(inputFilePath.string());
                long long outputSize = File::getFileSize(tempPath.string());
                double compressionAchieved = 1.0 - static_cast<double>(outputSize) / inputSize;

                cout << "[Debug] threshold: " << mid << ", compression: " << compressionAchieved * 100 << "%" << endl;

                if (compressionAchieved < percentageCompression) {
                    low = mid + binarySearchStep;
                } else {
                    bestThreshold = mid;
                    high = mid - binarySearchStep;
                }
            }

            fs::remove(tempPath); // Bersihkan file sementara
            threshold = bestThreshold;

            cout << "Threshold terbaik ditemukan: " << threshold << endl;

            // Build tree dengan threshold terbaik
            QuadTreeNode quadRoot(inputImage, pickMethod, threshold, minBlockSize);
            cout << "QuadTreeNode initialized." << endl;

            vector<vector<Pixel>> imageMatrix(inputImage.height, vector<Pixel>(inputImage.width));
            quadRoot.buildMatrix(imageMatrix);
            cout << "Matrix built." << endl;

            Image::saveFromMatrix(imageMatrix, inputImage.width, inputImage.height, inputImage.channels, savePath);
            cout << "Image saved to: " << savePath << endl;

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
            int maxDepth = stat.first;
            int nodeCount = stat.second;
            cout << "Kedalaman QuadTree: " << maxDepth << endl;
            cout << "Jumlah Simpul: " << nodeCount << endl;

            fs::path defaultGifSavePath = getDefaultGifPath(inputFilePath, pickMethod, threshold, minBlockSize, percentageCompression);
            GIFMaker::generateAndSave(maxDepth, &quadRoot, inputImage.width, inputImage.height, defaultGifSavePath);
        } else {
            cout << "Processing image with method " << pickMethod << " and threshold " << threshold << endl;
            cout << "Minimum block size: " << minBlockSize << endl;

            auto start = chrono::high_resolution_clock::now();

            QuadTreeNode quadRoot(inputImage, pickMethod, threshold, minBlockSize);
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
            int maxDepth = stat.first;
            int nodeCount = stat.second;
            cout << "Kedalaman QuadTree: " << maxDepth << endl;
            cout << "Jumlah Simpul: " << nodeCount << endl;

            fs::path defaultGifSavePath = getDefaultGifPath(inputFilePath, pickMethod, threshold, minBlockSize, percentageCompression);
            GIFMaker::generateAndSave(maxDepth, &quadRoot, inputImage.width, inputImage.height, defaultGifSavePath);
        }
    } catch (const exception &e) {
        cerr << "Exception: " << e.what() << endl;
        return 1;
    }

    return 0;
}
