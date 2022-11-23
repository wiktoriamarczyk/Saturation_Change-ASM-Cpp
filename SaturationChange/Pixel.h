#pragma once
#include <iostream>
using namespace std;

struct rgb
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

class Pixel
{
public:
    Pixel(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    {
        this->r = r;
        this->g = g;
        this->b = b;
        this->a = a;

        calcHSV();
    }

    Pixel(rgb RGB) : Pixel(RGB.r, RGB.g, RGB.b, RGB.a) {};

    operator rgb();

    unsigned char getR() { return r; };
    unsigned char getG() { return g; };
    unsigned char getB() { return b; };
    unsigned char getA() { return a; };

    unsigned char getH() { return h; };
    unsigned char getS() { return s; };
    unsigned char getV() { return v; };

    void setR(unsigned char r) { this->r = r; calcHSV(); };
    void setG(unsigned char g) { this->g = g; calcHSV(); };
    void setB(unsigned char b) { this->b = b; calcHSV(); };
    void setA(unsigned char a) { this->a = a; };

    void setH(unsigned char h) { this->h = h; calcRGB(); };
    void setS(unsigned char s) { this->s = s; calcRGB(); };
    void setV(unsigned char v) { this->v = v; calcRGB(); };


private:
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    unsigned char a = 0;

    unsigned char h = 0;
    unsigned char s = 0;
    unsigned char v = 0;

    void calcHSV();
    void calcRGB();
};
