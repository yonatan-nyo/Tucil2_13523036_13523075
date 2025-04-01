#include "Pixel.hpp"
#include <iostream>
#include <stdexcept>

Pixel::Pixel() : r(0), g(0), b(0), a(255) {}

Pixel::Pixel(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    if (r > 255 || g > 255 || b > 255 || a > 255) {
        throw std::invalid_argument("Pixel values must be between 0 and 255.");
    }
    this->r = r;
    this->g = g;
    this->b = b;
    this->a = a;
}
