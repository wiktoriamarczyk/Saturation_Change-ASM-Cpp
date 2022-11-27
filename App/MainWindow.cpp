#include "MainWindow.h"
#include "Pixel.h"
#include "ScopeExit.h"
//#include "../LibCpp/saturationChangeCpp.h"

#include <QMessageBox>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include<filesystem>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

using TestFunctionType1 = void (_stdcall *) (Pixel*, int, float);

MainWindow::MainWindow()
{
    this->setupUi(this);

    //connect(saturationSlider, &QSlider::valueChanged, [this] {satSliderChanged(); });
    connect(runButton, &QPushButton::pressed, this, &MainWindow::runButtonPressed);
    connect(loadButton, &QPushButton::pressed, this, &MainWindow::loadButtonPressed);
    connect(saturationSlider, &QSlider::valueChanged, this, &MainWindow::satSliderChanged);
}

void MainWindow::satSliderChanged()
{
    satLvlLabel->setText(QString::number(saturationSlider->value()));
}

void MainWindow::loadButtonPressed()
{
    // choose file
    QFileDialog dialog(this);
    QString tmpfilePath = dialog.getOpenFileName();

    // check if the file is valid
    if (!isFileValid(tmpfilePath))
    {
        isImageLoaded = false;
        return;
    }

    // set the file path
    filePath      = tmpfilePath;
    // set the file name
    fileName      = getFileNameFromThePath(std::filesystem::path(filePath.toStdString()));
    isImageLoaded = true;

    // display loaded image
    QGraphicsScene* scene = new QGraphicsScene();
    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap(QString::fromStdString(fileName)));
    originalPicture->setScene(scene);
    scene->addItem(item);
    originalPicture->show();
}

bool MainWindow::isFileValid(QString inputFilePath)
{
    // check if the file exists
    if (inputFilePath.isNull() || inputFilePath.isEmpty())
    {
        return false;
    }

    string tmpFileName = getFileNameFromThePath(std::filesystem::path(inputFilePath.toStdString()));
    std::filesystem::path fsFileName = tmpFileName;

    // check if the file has proper extension
    if (fsFileName.extension() != ".png" && fsFileName.extension() != ".jpg" && fsFileName.extension() != ".bmp")
    {
        QMessageBox::information(this, QStringLiteral("Error"), QString("Cannot load %1").arg(QString::fromStdString(tmpFileName)));
        return false;
    }

    return true;
}

string getFileNameFromThePath(std::filesystem::path inputFilePath)
{
    string result = inputFilePath.string();
    result = result.substr(result.find_last_of("/\\") + 1);
    return result;
}

void MainWindow::runButtonPressed()
{
    // if image is not loaded, return
    if (!isImageLoaded && filePath.isEmpty())
    {
        QMessageBox::information(this, QStringLiteral("Error"), QString("File not loaded!"));
        return;
    }

    // image dimensions
    int width = 0;
    int height = 0;
    // load the image
    stbi_uc* pImage = stbi_load(fileName.c_str(), &width, &height, nullptr, 4);

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

    // make a copy of the pixels array where the modified pixels will be stored
    vector<Pixel> modifiedPixels;
    modifiedPixels.reserve(pixels.size());
    modifiedPixels = pixels;

    // for increasing the saturation: value between 0 and 1
    // for decreasing the saturation: value between -1 and 0
    float satLvl = saturationSlider->value() / 100.f;

    // call cpp or asm function to change the saturation
    if (cppRadioButton->isChecked())
    {
        if (auto pFunctionRawPtr = reinterpret_cast<TestFunctionType1>(QLibrary("./LibCpp.dll").resolve("changeSaturation")))
            pFunctionRawPtr(modifiedPixels.data(), modifiedPixels.size(), satLvl);
        else
            QMessageBox::information(nullptr, QStringLiteral("Hello Message"), QStringLiteral("No Lib! :("), QMessageBox::Ok);
    }
    else if (asmRadioButton->isChecked())
    {
        /* call asm function */
    }

    // copy all modified pixels to the array containing simple 4 BYTE pixels
    pixels4B.clear();
    std::copy(modifiedPixels.begin(), modifiedPixels.end(), std::back_inserter(pixels4B));

    // create new image with modified pixels
    std::filesystem::path fileNameWithoutEx(fileName);
    fileNameWithoutEx.replace_extension();
    string newFileName = fileNameWithoutEx.string() + "_modified_";

    stbi_write_png((newFileName + to_string(satLvl) + ".png").c_str(), width, height, 4, pixels4B.data(), width * sizeof(rgb));

    // display result image
    QGraphicsScene* scene = new QGraphicsScene();
    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap((newFileName + to_string(satLvl) + ".png").c_str()));
    resultPicture->setScene(scene);
    scene->addItem(item);
    resultPicture->show();
}

float setPrecision(float number, int places)
{
    float n = std::pow(10.0f, places);
    return std::round(f * n) / n;
}