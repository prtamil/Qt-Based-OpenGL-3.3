#include "mainwindow.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);


    QGLFormat format;
    format.setVersion(3,3);
    format.setProfile(QGLFormat::CoreProfile);
    GLWindow w(format);
    w.setWindowTitle("FirstQt GL");
    w.resize(640,480);
    w.show();

    return a.exec();
}
