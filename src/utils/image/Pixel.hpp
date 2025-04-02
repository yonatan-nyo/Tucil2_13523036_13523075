#ifndef PIXEL_HPP
#define PIXEL_HPP

#include <iostream>
using namespace std;

class Pixel {
public:
    unsigned char r, g, b, a;
    Pixel();
    Pixel(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

    friend ostream &operator<<(ostream &os, const Pixel &pixel);
};

#endif