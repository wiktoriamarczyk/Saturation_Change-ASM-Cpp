#pragma once
#include <iostream>

struct rgb
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

struct hsv
{
    uint8_t h;
    uint8_t s;
    uint8_t v;
    uint8_t a;
};

class Pixel
{
public:
    Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    Pixel(rgb inputRGB) : Pixel(inputRGB.r, inputRGB.g, inputRGB.b, inputRGB.a) {};
    Pixel(hsv inputHSV);

    operator rgb();
    operator hsv();

    uint8_t getR() { return RGB.r; };
    uint8_t getG() { return RGB.g; };
    uint8_t getB() { return RGB.b; };
    uint8_t getA() { return RGB.a; };

    uint8_t getH() { return HSV.h; };
    uint8_t getS() { return HSV.s; };
    uint8_t getV() { return HSV.v; };

    void setR(uint8_t r) { this->RGB.r = r; calcHSV();       };
    void setG(uint8_t g) { this->RGB.g = g; calcHSV();       };
    void setB(uint8_t b) { this->RGB.b = b; calcHSV();       };
    void setA(uint8_t a) { this->RGB.a = a; this->HSV.a = a; };

    void setH(uint8_t h) { this->HSV.h = h; calcRGB(); };
    void setS(uint8_t s) { this->HSV.s = s; calcRGB(); };
    void setV(uint8_t v) { this->HSV.v = v; calcRGB(); };


private:
    rgb RGB;
    hsv HSV;

    void calcHSV();
    void calcRGB();
};
