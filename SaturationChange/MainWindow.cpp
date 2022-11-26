#include "MainWindow.h"
#include "Pixel.h"
#include "ScopeExit.h"

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

    //connect(saturationSlider, &QSlider::valueChanged, [this] {satSliderChanged(); });
    connect(runButton, &QPushButton::pressed, this, &MainWindow::runButtonPressed);
    connect(saturationSlider, &QSlider::valueChanged, this, &MainWindow::satSliderChanged);
}

void MainWindow::satSliderChanged()
{
    satLvlLabel->setText(QString::number(saturationSlider->value()));
}

void MainWindow::runButtonPressed()
{
    //QMessageBox::information(this, QStringLiteral("Hello Message"), QStringLiteral("Hello World From Qt6!"), QMessageBox::Ok);

    // display original image
    QGraphicsScene* scene = new QGraphicsScene();
    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap("goldie.png"));
    originalPicture->setScene(scene);
    scene->addItem(item);
    originalPicture->show();
    // image dimensions
    int width = 0;
    int height = 0;
    // load the image
    stbi_uc* pImage = stbi_load("goldie.png", &width, &height, nullptr, 4);

    SCOPE_EXIT
    {
        stbi_image_free(pImage);
        pImage = 0;
    };

    // create an array of simple 4 BYTE pixel objects
    vector<rgb> pixels4B;
    // resize the array with the number of pixels from the loaded image
    pixels4B.resize(width * height);
    // copy all 4 BYTE pixel data from the loaded image to the array
    memcpy(pixels4B.data(), pImage, pixels4B.size() * sizeof(rgb));

    // create an array of complex pixel objects
    vector<Pixel> pixels;
    // copy the 4 BYTE pixel data from the array to the new array
    std::copy(pixels4B.begin(), pixels4B.end(), std::back_inserter(pixels));

    auto start = std::chrono::high_resolution_clock::now();
    // make a copy of the pixels array
    vector<Pixel> pixelsCopy;
    pixelsCopy.reserve(pixels.size());
    pixelsCopy = pixels;

    // for increasing the saturation: value between 0 and 1
    // for decreasing the saturation: value between -1 and 0
    float satLvl = saturationSlider->value() / 100.f;

    // increasing the saturation
    if (satLvl > 0)
    {
        for (int i = 0; i < pixelsCopy.size(); ++i)
        {
            // colorFactor = 0 - no color at all, gray scale pixel; colorFactor = 1 - max saturation
            float colorFactor = (float)pixelsCopy[i].getS() / 255.0;
            // max saturation value is 255
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

    pixels4B.clear();
    // copy all modified pixels to the array containing simple 4 BYTE pixels
    std::copy(pixelsCopy.begin(), pixelsCopy.end(), std::back_inserter(pixels4B));
    // create new image with modified pixels
    stbi_write_png(("goldie_new_" + to_string(satLvl) + ".png").c_str(), width, height, 4, pixels4B.data(), width * sizeof(rgb));

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;
    cout << elapsed.count() << " sec" << endl;

    // display result image
    scene = new QGraphicsScene();
    item = new QGraphicsPixmapItem(QPixmap(("goldie_new_" + to_string(satLvl) + ".png").c_str()));
    resultPicture->setScene(scene);
    scene->addItem(item);
    resultPicture->show();
}