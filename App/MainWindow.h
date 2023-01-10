#pragma once
#include <QtWidgets>
#include "ui_MainWindow.h"
#include <string>

using std::string;

class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT
public:
    MainWindow();
protected:
private slots:
    void runButtonPressed();
    void loadButtonPressed();
    void satSliderChanged();
    void threadsNumberChanged();
    bool isFileValid(QString inputFilePath);
    void displayImage(QGraphicsView *view, string fileName);
private:
    QString filePath;
    string  fileName;
    bool    isImageLoaded = false;
};

string getFileNameFromPath(std::filesystem::path inputFilePath);
string getPrecisedValue(float value, int precision);
int roundUpTo(int value, int multiple);