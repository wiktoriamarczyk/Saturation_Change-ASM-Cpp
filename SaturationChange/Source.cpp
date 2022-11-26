#include <iostream>
#include "MainWindow.h"
#include "ScopeExit.h"

int RunQT(int argc, char** argv)
{
    QApplication app(argc, argv);

    MainWindow Wnd;

    Wnd.show();

    return app.exec();
}


struct TestObject
{
    TestObject()
    {
        printf("Jestem :D\n");
    }
    ~TestObject()
    {
        printf("Rodzicow nie ma w domu :)\n");
    }
};

int main()
{
    RunQT(0, nullptr);

    return 0;
}