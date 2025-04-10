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

void startCommandColor() {
    cout << "\033[1;37m";
}

void startInputColor() {
    cout << "\033[1;33m";
}

void startInfoColor() {
    cout << "\033[0m";
}

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

void initialize(fs::path &inputFilePath, fs::path &savePath, fs::path &gifSavePath,
                int &pickMethod, double &threshold, int &minBlockSize,
                Image &inputImage, double &percentageCompression) {
    // Input image filename
    startCommandColor();
    cout << "Enter image file name: ";
    startInputColor();
    string inputStr;
    getline(cin, inputStr);
    startInfoColor();
    inputFilePath = fs::absolute(fs::path(inputStr));

    // Check if file exists
    cout << "Checking file at: " << inputFilePath << endl;

    if (!fs::exists(inputFilePath)) {
        throw runtime_error("Error: File " + inputFilePath.string() + " does not exist.");
    }

    cout << "File exists, initializing image." << endl;
    inputImage = Image(inputFilePath.string());
    cout << "Image initialized." << endl
         << endl;
    cout << inputImage << endl;

    startCommandColor();
    cout << "Do you want to use percentage compression? (0.0 - 1.0) (input 0 to use threshold and minimum block size): ";
    startInputColor();
    cin >> percentageCompression;
    startInfoColor();
    if (percentageCompression != 0) {
        if (percentageCompression < 0 || percentageCompression > 1) {
            throw invalid_argument("Invalid percentage compression value. Must be between 0 and 1.");
        }
        cout << "Using percentage compression: " << percentageCompression << endl;
        pickMethod = 1;
        threshold = 0.0;
        minBlockSize = 1;
    } else {
        cout << endl;
        cout << "Pilih Metode Perhitungan Error" << endl;
        cout << "1. Variance" << endl;
        cout << "2. Mean Absolute Deviation" << endl;
        cout << "3. Max Pixel Difference" << endl;
        cout << "4. Entropy" << endl;
        cout << "5. Structural Similarity Index" << endl;
        startCommandColor();
        cout << "Masukkan metode (1-5): ";
        startInputColor();
        cin >> pickMethod;
        startInfoColor();
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
            minThreshold = 0.0;
            maxThreshold = 1.0;
        } else {
            throw invalid_argument("Invalid method selected.");
        }

        startCommandColor();
        cout << "Masukkan Threshold atau ambang batas ( " << minThreshold << " ≤ x ≤ " << maxThreshold << " ): ";
        startInputColor();
        cin >> threshold;
        startInfoColor();
        if (threshold < minThreshold) {
            cout << "Threshold tidak boleh kurang dari " << minThreshold << ", di set ke " << minThreshold << endl;
            threshold = minThreshold;
        } else if (threshold > maxThreshold) {
            cout << "Threshold tidak boleh lebih dari " << maxThreshold << ", di set ke " << maxThreshold << endl;
            threshold = maxThreshold;
        }

        startCommandColor();
        cout << "Masukkan ukuran blok minimum: ";
        startInputColor();
        cin >> minBlockSize;
        startInfoColor();
        if (minBlockSize < 1) {
            cout << "Ukuran blok minimum tidak boleh kurang dari 1!" << endl;
            minBlockSize = 1;
            cout << "Ukuran blok minimum di set ke 1!" << endl;
        }
    }

    cin.ignore();
    fs::path defaultSavePath = getDefaultPath(inputFilePath, pickMethod, threshold, minBlockSize, percentageCompression);
    startCommandColor();
    cout << endl
         << "Masukkan alamat file hasil" << endl;
    startInfoColor();
    cout << "[Default: " << defaultSavePath << "]" << endl;
    startCommandColor();
    cout << "(tekan Enter untuk default): ";
    string savePathStr;
    startInputColor();
    getline(cin, savePathStr);
    startInfoColor();

    if (savePathStr.empty()) {
        savePath = defaultSavePath;
        cout << "Tidak ada path diberikan. Menggunakan default: " << savePath << endl;
    } else {
        savePath = fs::absolute(fs::path(savePathStr));
    }

    fs::path defaultGifPath = getDefaultGifPath(inputFilePath, pickMethod, threshold, minBlockSize, percentageCompression);
    startCommandColor();
    cout << endl
         << "Masukkan alamat file hasil GIF" << endl;
    startInfoColor();
    cout << "[Default: " << defaultGifPath << "]" << endl;
    startCommandColor();
    cout << "(tekan Enter untuk default): ";
    string gifPathStr;
    startInputColor();
    getline(cin, gifPathStr);
    startInfoColor();

    if (gifPathStr.empty()) {
        gifSavePath = defaultGifPath;
        cout << "Tidak ada path diberikan. Menggunakan default: " << gifSavePath << endl;
    } else {
        gifSavePath = fs::absolute(fs::path(gifPathStr));
        if (gifSavePath.extension() != ".gif") {
            gifSavePath.replace_extension(".gif");
            cout << "Ekstensi file GIF tidak valid. Menggunakan ekstensi .gif: " << gifSavePath << endl;
        }
    }
    cout << endl;
}

double findBestThreshold(const Image &inputImage, double percentageCompression, const fs::path &inputFilePath) {
    cout << "Melakukan pencarian threshold otomatis dengan target kompresi: " << percentageCompression * 100 << "%" << endl;

    int pickMethod = 1;
    int minBlockSize = 1;
    double low = 0.0, high = 65025.0, bestThreshold = 0.0, binarySearchStep = 1e-8;

    fs::path tempPath = inputFilePath.parent_path() / ("temp_output" + inputFilePath.extension().string());

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

    fs::remove(tempPath);
    cout << "Threshold terbaik ditemukan: " << bestThreshold << endl;

    return bestThreshold;
}

void printStats(double executionTime, const fs::path &inputFilePath, const fs::path &savePath, const QuadTreeNode &quadRoot) {
    cout << "\033[1;37m====== stats =======\033[0m" << endl;
    cout << "Waktu Eksekusi: " << executionTime << " seconds" << endl;

    long long inputSize = File::getFileSize(inputFilePath.string());
    long long outputSize = File::getFileSize(savePath.string());
    cout << "Ukuran Gambar sebelum: " << inputSize << " bit" << endl;
    cout << "Ukuran Gambar setelah: " << outputSize << " bit" << endl;
    cout << "Persentase kompresi: " << (1 - (static_cast<double>(outputSize) / inputSize)) * 100 << "%" << endl;

    auto stat = quadRoot.getStat();
    cout << "Kedalaman QuadTree: " << stat.first << endl;
    cout << "Jumlah Simpul: " << stat.second << endl;
}

void processAndSaveImage(const Image &inputImage, int pickMethod, double threshold, int minBlockSize,
                         fs::path &savePath, const fs::path &inputFilePath, double percentageCompression,
                         fs::path &gifSavePath) {
    auto start = chrono::high_resolution_clock::now();

    cout << "Processing image with method " << pickMethod << " and threshold " << threshold << endl;
    cout << "Minimum block size: " << minBlockSize << endl;

    QuadTreeNode quadRoot(inputImage, pickMethod, threshold, minBlockSize);
    cout << "QuadTreeNode initialized." << endl;

    cout << "Building matrix..." << endl;
    vector<vector<Pixel>> imageMatrix(inputImage.height, vector<Pixel>(inputImage.width));
    quadRoot.buildMatrix(imageMatrix);
    cout << "Matrix built." << endl;

    Image::saveFromMatrix(imageMatrix, inputImage.width, inputImage.height, inputImage.channels, savePath);
    cout << "Image saved to: " << savePath << endl;
    cout << endl;

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    printStats(duration.count(), inputFilePath, savePath, quadRoot);
    auto stat = quadRoot.getStat();

    cout << endl;
    GIFMaker::generateAndSave(stat.first, &quadRoot, inputImage.width, inputImage.height, gifSavePath);
}

int main() {
    try {
        cout << "\033[1;33m";
        cout << "=========================\n";
        cout << "              .." << endl;
        cout << "             ( '`< \033[1;34mITB\033[1;33m" << endl;
        cout << "              )(" << endl;
        cout << "       ( ----'  '." << endl;
        cout << "       (         ;" << endl;
        cout << "        (_______,'" << endl;
        cout << "\033[1;34m";
        cout << "~^~^~^~^~^~^~^~^~^~^~^~^~" << endl;
        cout << "\033[1;37m";
        cout << "QuadTree Image Compressor\n";
        cout << "\033[1;34m";
        cout << "=========================\n";
        cout << "\033[0m" << endl;

        fs::path inputFilePath, savePath, gifSavePath;
        int pickMethod, minBlockSize;
        double threshold;
        Image inputImage;
        double percentageCompression;

        initialize(inputFilePath, savePath, gifSavePath, pickMethod, threshold, minBlockSize, inputImage, percentageCompression);

        if (percentageCompression != 0) {
            threshold = findBestThreshold(inputImage, percentageCompression, inputFilePath);
        }

        processAndSaveImage(inputImage, pickMethod, threshold, minBlockSize, savePath, inputFilePath, percentageCompression, gifSavePath);

    } catch (const exception &e) {
        cerr << "Exception: " << e.what() << endl;
        return 1;
    }

    return 0;
}
