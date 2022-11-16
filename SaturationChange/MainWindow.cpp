#include "MainWindow.h"
#include <QMessageBox>

MainWindow::MainWindow()
{
    this->setupUi(this);

    //connect( pushButton , &QPushButton::pressed , [this]{ Test(); } );
    //connect(pushButton, &QPushButton::pressed, this, &MainWindow::Test);
}

void MainWindow::Test()
{
    QMessageBox::information(this, QStringLiteral("Hello Message"), QStringLiteral("Hello World From Qt6!"), QMessageBox::Ok);
}