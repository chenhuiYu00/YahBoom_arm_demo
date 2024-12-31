#include "include/base.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    base w;
    w.show();
    return a.exec();
}
