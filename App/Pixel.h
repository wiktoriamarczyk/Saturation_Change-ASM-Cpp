#pragma once
#include <iostream>

/*
 Simple 4 byte pixel class in rgb model.
*/
struct rgb
{
    // Red channel
    uint8_t r;
    // Green channel
    uint8_t g;
    // Blue channel
    uint8_t b;
    // Alpha channel
    uint8_t a;
};

/*
 Simple 4 byte pixel class in hsv model.
*/
struct hsv
{
    // Hue channel
    uint8_t h;
    // Saturation channel
    uint8_t s;
    // Value channel
    uint8_t v;
    // Alpha channel
    uint8_t a;
};

/*
 Pixel class.
*/
class Pixel
{
public:
    // Pixel class constructor in rgb model with parameters as uint8.
    Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    // Pixel class constructor from rgb object.
    Pixel(rgb inputRGB) : Pixel(inputRGB.r, inputRGB.g, inputRGB.b, inputRGB.a) {};
    // Pixel class constructor from hsv object.
    Pixel(hsv inputHSV);

    operator rgb();
    operator hsv();

    // getters for RGBA channels
    uint8_t getR() { return RGB.r; };
    uint8_t getG() { return RGB.g; };
    uint8_t getB() { return RGB.b; };
    uint8_t getA() { return RGB.a; };
    // getters for HSV channels
    uint8_t getH() { return HSV.h; };
    uint8_t getS() { return HSV.s; };
    uint8_t getV() { return HSV.v; };
    // setters for RGBA channels
    void setR(uint8_t r) { this->RGB.r = r; calcHSV();       };
    void setG(uint8_t g) { this->RGB.g = g; calcHSV();       };
    void setB(uint8_t b) { this->RGB.b = b; calcHSV();       };
    void setA(uint8_t a) { this->RGB.a = a; this->HSV.a = a; };
    // setters for HSV channels
    void setH(uint8_t h) { this->HSV.h = h; calcRGB(); };
    void setS(uint8_t s) { this->HSV.s = s; calcRGB(); };
    void setV(uint8_t v) { this->HSV.v = v; calcRGB(); };


private:
    // rgb object.
    rgb RGB;
    // hsv object.
    hsv HSV;
    // Calculates HSV channels from RGB.
    void calcHSV();
    // Calculates RGB channels from HSV.
    void calcRGB();
};
