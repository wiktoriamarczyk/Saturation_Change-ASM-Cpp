#include "MainWindow.h"
#include "Pixel.h"
#include "ScopeExit.h"

#include <QMessageBox>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include<filesystem>
#include<cmath>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using std::vector;
using std::to_string;
using std::filesystem::path;
using std::copy;
using std::back_inserter;

/*
 Project:   The application responsible for changing the color saturation in images.
            The program uses the stb_image library to load and save images and the Qt library to implement the user interface.

 Algorithm: The algorithm is based on the HSV color model. For each pixel of the loaded image, the saturation is changed by adding to saturation parameter its value multiplied
            by a given factor which is calculated based on the slider value representing the percentage of change. Slider value is then clamped to the range of 0 to 1
            (for increasing the saturation) or to the range of -1 to 0 if a decrease in saturation was selected. The new pixel saturation value can be a maximum of 255 and a minimum of 0.
            Modified pixels by the given saturation value are then converted to the RGB model and saved to a new image.

 Author:     Wiktoria Marczyk
 Version:    1.0

 sem. 5, 2022/2023
*/

using TestFunctionType1 = void (_stdcall *) (hsv*, int, float);

/*
 Main Window constructor in which components are assigned to functions.
*/
MainWindow::MainWindow()
{
    this->setupUi(this);

    //connect(saturationSlider, &QSlider::valueChanged, [this] {satSliderChanged(); });
    connect(runButton, &QPushButton::pressed, this, &MainWindow::runButtonPressed);
    connect(loadButton, &QPushButton::pressed, this, &MainWindow::loadButtonPressed);
    connect(saturationSlider, &QSlider::valueChanged, this, &MainWindow::satSliderChanged);
}

/*
 Changes the label text to the current value of the saturation slider.
*/
void MainWindow::satSliderChanged()
{
    satLvlLabel->setText(QString::number(saturationSlider->value()) + "%");
}

/*
 Loads the image from the given path and displays it on the screen. Function triggered by pressing the load button.
*/
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
    fileName      = getFileNameFromThePath(path(filePath.toStdString()));
    isImageLoaded = true;

    // display loaded image
    displayImage(originalPicture, fileName);
}

/*
 Checks if the file is valid (has proper extension .png/.jpg/.bmp). If not a message box is displayed.

 @param inputFilePath - path to the file
 @return true if the file is valid, false otherwise
*/
bool MainWindow::isFileValid(QString inputFilePath)
{
    // check if the file exists
    if (inputFilePath.isNull() || inputFilePath.isEmpty())
    {
        return false;
    }

    string tmpFileName = getFileNameFromThePath(path(inputFilePath.toStdString()));
    path fsFileName = tmpFileName;

    // check if the file has proper extension
    if (fsFileName.extension() != ".png" && fsFileName.extension() != ".jpg" && fsFileName.extension() != ".bmp")
    {
        QMessageBox::information(this, QStringLiteral("Error"), QString("Cannot load %1").arg(QString::fromStdString(tmpFileName)));
        return false;
    }

    return true;
}

/*
 Returns the file name from the given path.

 @param inputFilePath - path to the file
 @return file name as a string
*/
string getFileNameFromThePath(path inputFilePath)
{
    string result = inputFilePath.string();
    result = result.substr(result.find_last_of("/\\") + 1);
    return result;
}

/*
 Execute the algorithm from loaded dll and save its result - the modified image. Function triggered by pressing the run button.
*/
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
    vector<rgb> pixelsRGB;
    // resize the array with the number of pixels from the loaded image

    const auto dataSize         = width * height;
    const auto dataBufferSize   = ((dataSize + 3) / 4) * 4;

    pixelsRGB.resize(dataBufferSize, rgb{0,0,0,0});
    // copy all 4 BYTE pixel data from the loaded image to the array
    memcpy(pixelsRGB.data(), pImage, dataSize * sizeof(rgb));

    // make a copy of the pixels array where the modified pixels will be stored
    vector<hsv> pixelsHSV;
    // copy the 4 BYTE pixel data from the array to the new array
    copy(pixelsRGB.begin(), pixelsRGB.end(), back_inserter(pixelsHSV));

    // for increasing the saturation: value between 0 and 1
    // for decreasing the saturation: value between -1 and 0
    float satLvl = saturationSlider->value() / 100.f;

    // call cpp or asm function to change the saturation
    if (cppRadioButton->isChecked())
    {
        if (auto pFunctionRawPtr = reinterpret_cast<TestFunctionType1>(QLibrary("./LibCpp.dll").resolve("changeSaturation")))
            pFunctionRawPtr(pixelsHSV.data(), pixelsHSV.size(), satLvl);
        else
            QMessageBox::information(nullptr, QStringLiteral("Hello Message"), QStringLiteral("No Lib! :("), QMessageBox::Ok);
    }
    else if (asmRadioButton->isChecked())
    {
        if (auto pFunctionRawPtr = reinterpret_cast<TestFunctionType1>(QLibrary("./LibASM.dll").resolve("changeSaturation")))
            pFunctionRawPtr(pixelsHSV.data(), pixelsHSV.size(), satLvl);
        else
            QMessageBox::information(nullptr, QStringLiteral("Hello Message"), QStringLiteral("No Lib! :("), QMessageBox::Ok);
    }

    // copy all modified pixels to the array containing simple 4 BYTE pixels
    pixelsRGB.clear();
    copy(pixelsHSV.begin(), pixelsHSV.end(), back_inserter(pixelsRGB));

    // create new image with modified pixels
    path fileNameWithoutEx(fileName);
    fileNameWithoutEx.replace_extension();
    string newFileName = fileNameWithoutEx.string() + "_modified_" + getPrecisedValueAsStr(satLvl, 2) + ".png";

    stbi_write_png((newFileName).c_str(), width, height, 4, pixelsRGB.data(), width * sizeof(rgb));

    // display result image
    displayImage(resultPicture, newFileName);
}

/*
 Displays the image on the screen.

 @param view - pointer to the QGraphicsView - a widget for displaying the contents of a QGraphicsScene
 @param inputFileName - name of the file to be displayed
*/
void MainWindow::displayImage(QGraphicsView *view, string inputFileName)
{
    // create a scene which provides a surface for managing 2D graphical items to display the image
    QGraphicsScene* scene = new QGraphicsScene();
    // create a pixmap from the image file
    QGraphicsPixmapItem* item = new QGraphicsPixmapItem(QPixmap(QString::fromStdString(inputFileName)));
    // pass the address of the scene to visualize
    view->setScene(scene);
    // add the pixmap to the scene
    scene->addItem(item);
    // display items on the scene (image)
    view->show();
}

/*
 Sets a precision to the float value.

 @param value - value to be modified
 @param precision - number of digits after the decimal point
 @return value with the given precision
*/
string getPrecisedValueAsStr(float value, int precision)
{
    string result = to_string(value);
    result = result.substr(0, result.find(".") + precision + 1);
    return result;
}