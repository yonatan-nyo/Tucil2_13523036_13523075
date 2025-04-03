#include "Pixel.hpp"
#include <iostream>
#include <stdexcept>
using namespace std;

Pixel::Pixel() : r(0), g(0), b(0), a(255) {}

Pixel::Pixel(int r, int g, int b, int a)
    : r(clampToUChar(r)), g(clampToUChar(g)), b(clampToUChar(b)), a(clampToUChar(a)) {}

ostream &operator<<(ostream &os, const Pixel &pixel) {
    os << "rgba(" << static_cast<int>(pixel.r) << ", "
       << static_cast<int>(pixel.g) << ", "
       << static_cast<int>(pixel.b) << ", "
       << static_cast<int>(pixel.a) << ")";
    return os;
}
