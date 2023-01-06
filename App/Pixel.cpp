#include "Pixel.h"


rgb hsv::calcRGB()const
{
    rgb RGB;

    uint8_t region, remainder, p, q, t;
    RGB.a = a;

    if (s == 0)
    {
        RGB.r = v;
        RGB.g = v;
        RGB.b = v;
        return RGB;
    }

    region = h / 43;
    remainder = (h - (region * 43)) * 6;

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region)
    {
    case 0:
        RGB.r = v; RGB.g = t; RGB.b = p;
        break;
    case 1:
        RGB.r = q; RGB.g = v; RGB.b = p;
        break;
    case 2:
        RGB.r = p; RGB.g = v; RGB.b = t;
        break;
    case 3:
        RGB.r = p; RGB.g = q; RGB.b = v;
        break;
    case 4:
        RGB.r = t; RGB.g = p; RGB.b = v;
        break;
    default:
        RGB.r = v; RGB.g = p; RGB.b = q;
        break;
    }

    return RGB;
}

hsv rgb::calcHSV()const
{
    hsv HSV;

    uint8_t rgbmin, rgbmax;
    HSV.a = a;

    rgbmin = r < g ? (r < b ? r : b) : (g < b ? g : b);
    rgbmax = r > g ? (r > b ? r : b) : (g > b ? g : b);

    HSV.v = rgbmax;
    if (HSV.v == 0)
    {
        HSV.h = 0;
        HSV.s = 0;
        return HSV;
    }

    HSV.s = 255 * long(rgbmax - rgbmin) / HSV.v;
    if (HSV.s == 0)
    {
        HSV.h = 0;
        return HSV;
    }

    if (rgbmax == r)
        HSV.h = 0 + 43 * (g - b) / (rgbmax - rgbmin);
    else if (rgbmax == g)
        HSV.h = 85 + 43 * (b - r) / (rgbmax - rgbmin);
    else
        HSV.h = 171 + 43 * (r - g) / (rgbmax - rgbmin);

    return HSV;
}

rgb::operator hsv()
{
    return calcHSV();
}

hsv::operator rgb()
{
    return calcRGB();
}