#include <QApplication>

#include <QImage>
#include <QPixmap>
#include <QLabel>

#include <QDebug>
#include "map.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Map map;
    map.show();

    qInfo("Map showed");
    return a.exec();
}
