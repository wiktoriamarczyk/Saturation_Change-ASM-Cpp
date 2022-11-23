#include "MainWindow.h"
#include "Pixel.h"

#include <QMessageBox>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;



MainWindow::MainWindow()
{
    this->setupUi(this);

    connect(runButton, &QPushButton::pressed , [this]{ runButtonPressed(); } );
    connect(runButton, &QPushButton::pressed, this, &MainWindow::runButtonPressed);
}

void MainWindow::runButtonPressed()
{
    //QMessageBox::information(this, QStringLiteral("Hello Message"), QStringLiteral("Hello World From Qt6!"), QMessageBox::Ok);
    int width = 0;
    int height = 0;

    stbi_uc* pImage = stbi_load("goldie.png", &width, &height, nullptr, 4);

    vector<rgb> pixelsSimple;
    pixelsSimple.resize(width * height);

    memcpy(pixelsSimple.data(), pImage, pixelsSimple.size() * sizeof(rgb));

    vector<Pixel> pixels;
    std::copy(pixelsSimple.begin(), pixelsSimple.end(), std::back_inserter(pixels));

    // for increasing the saturation: value between 0 and 1
    // for decreasing the saturation: value between -1 and 0
    vector<float> satLvls = { -1.f, -0.9f, -0.8f, -0.7f, -0.6f, -0.5f, -0.4f, -0.3f, -0.2f, -0.1f,
                             0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f, 0.8f, 0.9f, 1.f };

    auto start = std::chrono::high_resolution_clock::now();

    vector<Pixel> pixelsCopy;
    pixelsCopy.reserve(pixels.size());

    for (auto satLvl : satLvls)
    {
        pixelsCopy = pixels;

        // increasing the saturation
        if (satLvl > 0)
        {
            for (int i = 0; i < pixelsCopy.size(); ++i)
            {
                // colorFactor = 0 - no color at all, gray scale pixel; colorFactor = 1 - max saturation 
                float colorFactor = (float)pixelsCopy[i].getS() / 255.0;

                // (255 - hsv.s) cause max saturation value is 255  // ?
                pixelsCopy[i].setS(pixelsCopy[i].getS() + (255 - pixelsCopy[i].getS()) * satLvl * colorFactor);
            }
        }
        // decreasing the saturation
        else if (satLvl < 0)
        {
            for (int i = 0; i < pixelsCopy.size(); ++i)
            {
                // min saturation value is 0
                pixelsCopy[i].setS(pixelsCopy[i].getS() + pixelsCopy[i].getS() * satLvl);
            }
        }

        pixelsSimple.clear();
        std::copy(pixelsCopy.begin(), pixelsCopy.end(), std::back_inserter(pixelsSimple));
        stbi_write_png(("goldie_new_" + to_string(satLvl) + ".png").c_str(), width, height, 4, pixelsSimple.data(), width * sizeof(rgb));
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    cout << elapsed.count() << " sec" << endl;

    stbi_image_free(pImage);
    pImage = 0;
}



//
//
//rgbcolor hsvtorgb(hsvcolor hsv)
//{
//    rgbcolor rgb;
//    rgb.a = hsv.a;
//    unsigned char region, remainder, p, q, t;
//
//    if (hsv.s == 0)
//    {
//        rgb.r = hsv.v;
//        rgb.g = hsv.v;
//        rgb.b = hsv.v;
//        rgb.a = hsv.a;
//        return rgb;
//    }
//
//    region = hsv.h / 43;
//    remainder = (hsv.h - (region * 43)) * 6;
//
//    p = (hsv.v * (255 - hsv.s)) >> 8;
//    q = (hsv.v * (255 - ((hsv.s * remainder) >> 8))) >> 8;
//    t = (hsv.v * (255 - ((hsv.s * (255 - remainder)) >> 8))) >> 8;
//
//    switch (region)
//    {
//    case 0:
//        rgb.r = hsv.v; rgb.g = t; rgb.b = p;
//        break;
//    case 1:
//        rgb.r = q; rgb.g = hsv.v; rgb.b = p;
//        break;
//    case 2:
//        rgb.r = p; rgb.g = hsv.v; rgb.b = t;
//        break;
//    case 3:
//        rgb.r = p; rgb.g = q; rgb.b = hsv.v;
//        break;
//    case 4:
//        rgb.r = t; rgb.g = p; rgb.b = hsv.v;
//        break;
//    default:
//        rgb.r = hsv.v; rgb.g = p; rgb.b = q;
//        break;
//    }
//
//    return rgb;
//}
//
//hsvcolor rgbtohsv(rgbcolor rgb)
//{
//    hsvcolor hsv;
//    hsv.a = rgb.a;
//    unsigned char rgbmin, rgbmax;
//
//    rgbmin = rgb.r < rgb.g ? (rgb.r < rgb.b ? rgb.r : rgb.b) : (rgb.g < rgb.b ? rgb.g : rgb.b);
//    rgbmax = rgb.r > rgb.g ? (rgb.r > rgb.b ? rgb.r : rgb.b) : (rgb.g > rgb.b ? rgb.g : rgb.b);
//
//    hsv.v = rgbmax;
//    if (hsv.v == 0)
//    {
//        hsv.h = 0;
//        hsv.s = 0;
//        return hsv;
//    }
//
//    hsv.s = 255 * long(rgbmax - rgbmin) / hsv.v;
//    if (hsv.s == 0)
//    {
//        hsv.h = 0;
//        return hsv;
//    }
//
//    if (rgbmax == rgb.r)
//        hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbmax - rgbmin);
//    else if (rgbmax == rgb.g)
//        hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbmax - rgbmin);
//    else
//        hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbmax - rgbmin);
//
//    hsv.a = rgb.a;
//    return hsv;
//}