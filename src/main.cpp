#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>

#include "utils/image/File.hpp"
#include "utils/image/Image.hpp"
#include "utils/quadtree/QuadTreeNode.hpp"

namespace fs = std::filesystem;
using namespace std;

void initialize(string &inputFilename, string &resultPath, int &pickMethod, double &treshold, int &minBlockSize, Image &inputImage) {
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
    double minThreshold, maxThreshold;

    if (pickMethod == 1) {
        minThreshold = 0;
        maxThreshold = 65025.0; // 255^2 for maximum variance
    } else if (pickMethod == 2 || pickMethod == 3) {
        minThreshold = 0;
        maxThreshold = 255.0;
    } else if (pickMethod == 4) {
        minThreshold = 0;
        maxThreshold = 8.0; // Entropy (log2(256)) for 8-bit color values
    } else if (pickMethod == 5) {
        minThreshold = -1.0;
        maxThreshold = 1.0;
    } else {
        throw invalid_argument("Invalid method selected.");
    }

    cout << "Masukkan Treshold (ambang batas): ";
    cin >> treshold;
    // check treshold according to min max method treshold, if <min use min, if >max use max
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
    cout << "Masukkan alamat file hasil (test/[nama_file].png)" << endl;
    cout << "contoh (test/a1.png): ";
    cin >> resultPath;
    cout << endl;

    fs::path resultPathObj(resultPath);
    fs::path resultAbsolutePath = fs::absolute(resultPathObj);
    cout << "File hasil akan disimpan di: " << resultAbsolutePath << endl;
}

int main() {
    try {
        string inputFilename, resultPath;
        int pickMethod, minBlockSize;
        double treshold;
        Image inputImage;
        initialize(inputFilename, resultPath, pickMethod, treshold, minBlockSize, inputImage);

        cout << "Processing image with method " << pickMethod << " and threshold " << treshold << endl;
        cout << "Minimum block size: " << minBlockSize << endl;

        auto start = chrono::high_resolution_clock::now();

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
        cout << endl;

        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end - start;
        cout << "====== stats =======" << endl;
        cout << "Waktu Eksekusi: " << duration.count() << " seconds" << endl;
        long long inputSize = File::getFileSize(inputFilename);
        long long outputSize = File::getFileSize(resultPath);
        cout << "Ukuran Gambar sebelum: " << inputSize << " bit" << endl;
        cout << "Ukuran Gambar setelah: " << File::getFileSize(resultPath) << " bit" << endl;
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

/*
my absolute test path
/mnt/c/Users/Pongo/Documents/Yonatan/University/ITB/0.Teknik-Informatika/Semester-4/Strategi-dan-Algoritma/tucil2/Tucil2_13523036_13523075/test/a.png
*/
