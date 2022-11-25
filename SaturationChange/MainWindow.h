#pragma once
#include <QtWidgets>
#include "ui_MainWindow.h"

class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT
public:
    MainWindow();
protected:
private slots:
    void runButtonPressed();
    void satSliderChanged();
private:
};