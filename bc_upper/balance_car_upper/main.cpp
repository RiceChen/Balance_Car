#include "bc_upper.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    bc_upper w;
    w.show();
    return a.exec();
}
