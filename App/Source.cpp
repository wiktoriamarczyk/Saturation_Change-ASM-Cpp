#include <iostream>
#include "MainWindow.h"
#include "ScopeExit.h"

int RunQT(int argc, char** argv)
{
    std::filesystem::current_path(R"(C:/Users/wikto/source/repos/Mój Git - C++/SaturationChange/build/App/Debug)");

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