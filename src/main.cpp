#include "mainwindow.hpp"

#include <QApplication>
#include <QString>
#include <QStyleFactory>
#include <QDir>

// NOLINTBEGIN(misc-include-cleaner)
// NOLINTBEGIN(readability-static-accessed-through-instance)
int main(int argc, char **argv)
{
    qApp->setStyle("windowsVista");

    const QApplication app(argc, argv);

    const MainWindow wnd{};

    QString qss;
    QFile file1(":/resources/styles/style.qss");
    QFile file2(":/resources/styles/button-style-new.qss");

    file1.open(QFile::ReadOnly);
    file2.open(QFile::ReadOnly);

    qss += file1.readAll();
    qss += file2.readAll();

    qApp->setStyleSheet(qss);
    return app.exec();

}
// NOLINTEND(readability-static-accessed-through-instance)
// NOLINTEND(misc-include-cleaner)
