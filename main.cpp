#include "mainwindow.hpp"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w{};
    w.setWindowTitle("File-Manager");
    w.show();

    return a.exec();
}
