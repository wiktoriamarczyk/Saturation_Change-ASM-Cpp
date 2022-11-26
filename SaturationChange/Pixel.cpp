#include "Pixel.h"

Pixel::Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    this->RGB.r = r;
    this->RGB.g = g;
    this->RGB.b = b;
    this->RGB.a = a;

    calcHSV();
}

Pixel::Pixel(hsv inputHSV)
{
    this->HSV.h = inputHSV.h;
    this->HSV.s = inputHSV.s;
    this->HSV.v = inputHSV.v;
    this->HSV.a = inputHSV.a;

    calcRGB();
}

Pixel::operator rgb() 
{
    rgb result;
    result.r = this->RGB.r;
    result.g = this->RGB.g;
    result.b = this->RGB.b;
    result.a = this->RGB.a;
    return result;
}

Pixel::operator hsv()
{
    hsv result;
    result.h = this->HSV.h;
    result.s = this->HSV.s;
    result.v = this->HSV.v;
    result.a = this->HSV.a;
    return result;
}

void Pixel::calcHSV()
{
    uint8_t rgbmin, rgbmax;
    HSV.a = RGB.a;
    
    rgbmin = RGB.r < RGB.g ? (RGB.r < RGB.b ? RGB.r : RGB.b) : (RGB.g < RGB.b ? RGB.g : RGB.b);
    rgbmax = RGB.r > RGB.g ? (RGB.r > RGB.b ? RGB.r : RGB.b) : (RGB.g > RGB.b ? RGB.g : RGB.b);
    
    HSV.v = rgbmax;
    if (HSV.v == 0)
    {
        HSV.h = 0;
        HSV.s = 0;
        return;
    }
    
    HSV.s = 255 * long(rgbmax - rgbmin) / HSV.v;
    if (HSV.s == 0)
    {
        HSV.h = 0;
        return;
    }
    
    if (rgbmax == RGB.r)
        HSV.h = 0 + 43 * (RGB.g - RGB.b) / (rgbmax - rgbmin);
    else if (rgbmax == RGB.g)
        HSV.h = 85 + 43 * (RGB.b - RGB.r) / (rgbmax - rgbmin);
    else
        HSV.h = 171 + 43 * (RGB.r - RGB.g) / (rgbmax - rgbmin);
}

void Pixel::calcRGB()
{
    uint8_t region, remainder, p, q, t;
    RGB.a = HSV.a;

    if (HSV.s == 0)
    {
        RGB.r = HSV.v;
        RGB.g = HSV.v;
        RGB.b = HSV.v;
        return;
    }

    region = HSV.h / 43;
    remainder = (HSV.h - (region * 43)) * 6;

    p = (HSV.v * (255 - HSV.s)) >> 8;
    q = (HSV.v * (255 - ((HSV.s * remainder) >> 8))) >> 8;
    t = (HSV.v * (255 - ((HSV.s * (255 - remainder)) >> 8))) >> 8;

    switch (region)
    {
    case 0:
        RGB.r = HSV.v; RGB.g = t; RGB.b = p;
        break;
    case 1:
        RGB.r = q; RGB.g = HSV.v; RGB.b = p;
        break;
    case 2:
        RGB.r = p; RGB.g = HSV.v; RGB.b = t;
        break;
    case 3:
        RGB.r = p; RGB.g = q; RGB.b = HSV.v;
        break;
    case 4:
        RGB.r = t; RGB.g = p; RGB.b = HSV.v;
        break;
    default:
        RGB.r = HSV.v; RGB.g = p; RGB.b = q;
        break;
    }
}