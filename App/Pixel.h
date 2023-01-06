#pragma once
#include <iostream>

struct hsv;

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
    // Calculates HSV channels from RGB.
    hsv calcHSV()const;
    operator hsv();
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
    // Calculates RGB channels from HSV.
    rgb calcRGB()const;
    operator rgb();
};