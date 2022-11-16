#include <iostream>
#include "MainWindow.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <vector>
#include <string>
#include <chrono>

using namespace std;


typedef struct rgbcolor
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

typedef struct hsvcolor
{
    unsigned char h;
    unsigned char s;
    unsigned char v;
    unsigned char a;
};

rgbcolor hsvtorgb(hsvcolor hsv)
{
    rgbcolor rgb;
    rgb.a = hsv.a;
    unsigned char region, remainder, p, q, t;

    if (hsv.s == 0)
    {
        rgb.r = hsv.v;
        rgb.g = hsv.v;
        rgb.b = hsv.v;
        rgb.a = hsv.a;
        return rgb;
    }

    region = hsv.h / 43;
    remainder = (hsv.h - (region * 43)) * 6;

    p = (hsv.v * (255 - hsv.s)) >> 8;
    q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
    t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;

    switch (region)
    {
    case 0:
        rgb.r = hsv.v; rgb.g = t; rgb.b = p;
        break;
    case 1:
        rgb.r = q; rgb.g = hsv.v; rgb.b = p;
        break;
    case 2:
        rgb.r = p; rgb.g = hsv.v; rgb.b = t;
        break;
    case 3:
        rgb.r = p; rgb.g = q; rgb.b = hsv.v;
        break;
    case 4:
        rgb.r = t; rgb.g = p; rgb.b = hsv.v;
        break;
    default:
        rgb.r = hsv.v; rgb.g = p; rgb.b = q;
        break;
    }

    return rgb;
}

hsvcolor rgbtohsv(rgbcolor rgb)
{
    hsvcolor hsv;
    hsv.a = rgb.a;
    unsigned char rgbmin, rgbmax;

    rgbmin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
    rgbmax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);

    hsv.v = rgbmax;
    if (hsv.v == 0)
    {
        hsv.h = 0;
        hsv.s = 0;
        return hsv;
    }

    hsv.s = 255 * long(rgbmax - rgbmin) / hsv.v;
    if (hsv.s == 0)
    {
        hsv.h = 0;
        return hsv;
    }

    if (rgbmax == rgb.r)
        hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbmax - rgbmin);
    else if (rgbmax == rgb.g)
        hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbmax - rgbmin);
    else
        hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbmax - rgbmin);

    hsv.a = rgb.a;
    return hsv;
}

int RunQT(int argc, char** argv)
{
    QApplication app(argc, argv);

    MainWindow Wnd;

    Wnd.show();

    return app.exec();
}



int main()
{
    RunQT(0, nullptr);



    int width = 0;

    rgbcolor testrgb = { 104, 85, 16, 255 };
    hsvcolor testhsv = rgbtohsv(testrgb);

    int height = 0;

    stbi_uc* pImage = stbi_load("goldie.png", &width, &height, nullptr, 4);

    vector<rgbcolor> Pixels;
    Pixels.resize(width * height);

    memcpy(Pixels.data(), pImage, Pixels.size() * sizeof(rgbcolor));


    // for increasing the saturation: value between 0 and 1
    // for decreasing the saturation: value between -1 and 0
    vector<float> satLvls = { -1.f, -0.9f, -0.8f, -0.7f, -0.6f, -0.5f, -0.4f, -0.3f, -0.2f, -0.1f,
                             0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.f };

    auto start = std::chrono::high_resolution_clock::now();

    vector<rgbcolor> PixelsCopy;
    PixelsCopy.reserve(Pixels.size());

    for (auto satLvl : satLvls)
    {
        PixelsCopy = Pixels;

        // increasing the saturation
        if (satLvl > 0)
        {
            for (int i = 0; i < PixelsCopy.size(); ++i)
            {
                hsvcolor hsv = rgbtohsv(PixelsCopy[i]);

                // colorFactor = 0 - no color at all, gray scale pixel; colorFactor = 1 - max saturation 
                float colorFactor = (float)hsv.s / 255.0;

                // (255 - hsv.s) cause max saturation value is 255  // ?
                hsv.s = hsv.s + (255 - hsv.s) * satLvl              * colorFactor;

                rgbcolor rgb = hsvtorgb(hsv);
                PixelsCopy[i] = rgb;
            }
        }
        // decreasing the saturation
        else if (satLvl < 0)
        {
            for (int i = 0; i < PixelsCopy.size(); ++i)
            {
                hsvcolor hsv = rgbtohsv(PixelsCopy[i]);

                // min saturation value is 0
                hsv.s = hsv.s + hsv.s * satLvl;

                rgbcolor rgb = hsvtorgb(hsv);
                PixelsCopy[i] = rgb;
            }
        }

        stbi_write_png(("goldie_new_" + to_string(satLvl) + ".png").c_str(), width, height, 4, PixelsCopy.data(), width*sizeof(rgbcolor));
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    cout << elapsed.count() << " sec" << endl;

    stbi_image_free(pImage);
    pImage = 0;


    return 0;
}