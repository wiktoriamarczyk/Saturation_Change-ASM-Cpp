#include "Pixel.h"


Pixel::operator rgb() 
{
    rgb result;
    result.r = this->r;
    result.g = this->g;
    result.b = this->b;
    result.a = this->a;
    return result;
}

void Pixel::calcHSV()
{
    unsigned char rgbmin, rgbmax;
    
    rgbmin = r < g ? (r < b ? r : b) : (g < b ? g : b);
    rgbmax = r > g ? (r > b ? r : b) : (g > b ? g : b);
    
    v = rgbmax;
    if (v == 0)
    {
        h = 0;
        s = 0;
        return;
    }
    
    s = 255 * long(rgbmax - rgbmin) / v;
    if (s == 0)
    {
        h = 0;
        return;
    }
    
    if (rgbmax == r)
        h = 0 + 43 * (g - b) / (rgbmax - rgbmin);
    else if (rgbmax == g)
        h = 85 + 43 * (b - r) / (rgbmax - rgbmin);
    else
        h = 171 + 43 * (r - g) / (rgbmax - rgbmin);
}

void Pixel::calcRGB()
{
    unsigned char region, remainder, p, q, t;

    if (s == 0)
    {
        r = v;
        g = v;
        b = v;
        a = a;
        return;
    }

    region = h / 43;
    remainder = (h - (region * 43)) * 6;

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region)
    {
    case 0:
        r = v; g = t; b = p;
        break;
    case 1:
        r = q; g = v; b = p;
        break;
    case 2:
        r = p; g = v; b = t;
        break;
    case 3:
        r = p; g = q; b = v;
        break;
    case 4:
        r = t; g = p; b = v;
        break;
    default:
        r = v; g = p; b = q;
        break;
    }
}