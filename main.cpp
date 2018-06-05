#include "mrvizifymain.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MrVizifyMain w;
    w.show();

    return a.exec();
}
