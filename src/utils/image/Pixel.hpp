#ifndef PIXEL_HPP
#define PIXEL_HPP

#include <iostream>
using namespace std;

class Pixel {
private:
    static constexpr unsigned char clampToUChar(int value) {
        return static_cast<unsigned char>(max(0, min(255, value)));
    }

public:
    unsigned char r, g, b, a;

    Pixel();
    Pixel(int r, int g, int b, int a = 255);

    friend ostream &operator<<(ostream &os, const Pixel &pixel);
};

#endif
