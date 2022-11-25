#pragma once
#include <iostream>

struct rgb
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

struct hsv
{
    unsigned char h;
    unsigned char s;
    unsigned char v;
    unsigned char a;
};

class Pixel
{
public:
    Pixel(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    Pixel(rgb inputRGB) : Pixel(inputRGB.r, inputRGB.g, inputRGB.b, inputRGB.a) {};
    Pixel(hsv inputHSV);

    operator rgb();
    operator hsv();

    unsigned char getR() { return RGB.r; };
    unsigned char getG() { return RGB.g; };
    unsigned char getB() { return RGB.b; };
    unsigned char getA() { return RGB.a; };

    unsigned char getH() { return HSV.h; };
    unsigned char getS() { return HSV.s; };
    unsigned char getV() { return HSV.v; };

    void setR(unsigned char r) { this->RGB.r = r; calcHSV();       };
    void setG(unsigned char g) { this->RGB.g = g; calcHSV();       };
    void setB(unsigned char b) { this->RGB.b = b; calcHSV();       };
    void setA(unsigned char a) { this->RGB.a = a; this->HSV.a = a; };

    void setH(unsigned char h) { this->HSV.h = h; calcRGB(); };
    void setS(unsigned char s) { this->HSV.s = s; calcRGB(); };
    void setV(unsigned char v) { this->HSV.v = v; calcRGB(); };


private:
    rgb RGB;
    hsv HSV;

    void calcHSV();
    void calcRGB();
};
