#include "AuthApp.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AuthApp w;
    w.show();
    return a.exec();
}
