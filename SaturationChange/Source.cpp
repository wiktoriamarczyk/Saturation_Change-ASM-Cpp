#include <iostream>
#include "MainWindow.h"

using namespace std;


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

    

    return 0;
}