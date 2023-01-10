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
#include<thread>

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
    connect(threadsSlider, &QSlider::valueChanged, this, &MainWindow::threadsNumberChanged);
}

/*
 Changes the label text to the current value of the threads number slider.
*/
void MainWindow::threadsNumberChanged()
{
    threadsNumLabel->setText(QString::number(threadsSlider->value()));
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
    fileName      = getFileNameFromPath(path(filePath.toStdString()));
    isImageLoaded = true;

    // display loaded image
    displayImage(originalPicture, fileName);
}

/*
 Checks if the file is valid (has proper extension .png/.jpg/.bmp). If not, a message box is displayed.

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

    string tmpFileName = getFileNameFromPath(path(inputFilePath.toStdString()));
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
 Execute the algorithm from loaded dll and save its result - the modified image. Function triggered by pressing the run button.
*/
void MainWindow::runButtonPressed()
{
    // ---------- IMAGE LOAD ----------

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


    // ---------- VARIABLES CREATION ----------

    // create an array of simple 4 BYTE pixel objects
    vector<rgb> pixelsRGB;
    // resize the array with the number of pixels from the loaded image
    const auto dataSize         = width * height;
    // assuring that the buffer size is a multiplication of: number of pixels processed simultaneously in ASM * number of threads
    const auto dataBufferSize   = roundUpTo((int)dataSize, 4 * threadsSlider->value());
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


    // ---------- DLL SELECTECTION ----------

    TestFunctionType1 selectedFunction = nullptr;
    bool isCppChosen = true;

    // select cpp or asm function to change the saturation
    if (cppRadioButton->isChecked())
    {
        if (auto pFunctionRawPtr = reinterpret_cast<TestFunctionType1>(QLibrary("./LibCpp.dll").resolve("changeSaturation")))
            selectedFunction = pFunctionRawPtr;
        else
            QMessageBox::information(nullptr, QStringLiteral("Error"), QStringLiteral("No Lib! :("), QMessageBox::Ok);
    }
    else if (asmRadioButton->isChecked())
    {
        if (auto pFunctionRawPtr = reinterpret_cast<TestFunctionType1>(QLibrary("./LibASM.dll").resolve("changeSaturation")))
            selectedFunction = pFunctionRawPtr;
        else
            QMessageBox::information(nullptr, QStringLiteral("Error"), QStringLiteral("No Lib! :("), QMessageBox::Ok);

        isCppChosen = false;
    }

    // if dll is not loaded, abort operation
    if (!selectedFunction)
        return;


    // ---------- THREADS CREATION ----------

    // create requested number of threads and run the algorithm
    vector<std::thread> threads;

    std::atomic_bool Go = false;

    auto threadFunction = [&](hsv* pixelBuffer, int bufferSize)
    {
        while (!Go)
        {}
        selectedFunction(pixelBuffer, bufferSize, satLvl);
    };

    const int pixelsCountPerThread = pixelsHSV.size() / threadsSlider->value();
    const int roundedPixelsCountPerThread = roundUpTo(pixelsCountPerThread, 4);

    for (int i = 0; i < threadsSlider->value(); i++)
    {
        // calculate buffer start for each thread
        hsv* threadBufferStart = pixelsHSV.data() + i * roundedPixelsCountPerThread;
        // create a new thread
        threads.push_back(std::thread(threadFunction, threadBufferStart, roundedPixelsCountPerThread));
    }


    // ---------- TIME MEASUREMENT ----------

    auto start = std::chrono::high_resolution_clock::now();
    Go = true;

    for (int i = 0; i < threadsSlider->value(); i++)
    {
        threads[i].join();
    }

    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = (finish - start) * 1000;

    if (isCppChosen)
        cppTimeLabel->setText( (getPrecisedValue(elapsed.count(), 2) + " ms").c_str() );
    else
        asmTimeLabel->setText( (getPrecisedValue(elapsed.count(), 2) + " ms").c_str() );


    // ---------- IMAGE SAVE ----------

    // copy all modified pixels to the array containing simple 4 BYTE pixels in RGB
    pixelsRGB.clear();
    copy(pixelsHSV.begin(), pixelsHSV.end(), back_inserter(pixelsRGB));

    // create new image with modified pixels
    path fileNameWithoutEx(fileName);
    fileNameWithoutEx.replace_extension();
    string newFileName = fileNameWithoutEx.string() + "_modified_" + getPrecisedValue(satLvl, 2) + ".png";

    // save the image
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
 Returns the file name from the given path.

 @param inputFilePath - path to the file
 @return file name as a string
*/
string getFileNameFromPath(path inputFilePath)
{
    string result = inputFilePath.string();
    result = result.substr(result.find_last_of("/\\") + 1);
    return result;
}

/*
 Sets a precision to the float value.

 @param value - value to be modified
 @param precision - number of digits after the decimal point
 @return value with the given precision as string
*/
string getPrecisedValue(float value, int precision)
{
    string result = to_string(value);
    result = result.substr(0, result.find(".") + precision + 1);
    return result;
}

/*
 Rounds a number to the nearest multiple of a given integer.

 @param value - value to be rounded
 @paaram multiple - number to which the value will be rounded
 @return rounded number
*/
int roundUpTo(int value, int multiple)
{
    return ((value + multiple - 1) / multiple) * multiple;
}