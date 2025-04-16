#include "mainwindow.hpp"

#include <QApplication>
#include <QString>
#include <QStyleFactory>
#include <QDir>

// NOLINTBEGIN(misc-include-cleaner)
// NOLINTBEGIN(readability-static-accessed-through-instance)
int main(int argc, char **argv)
{
    const QApplication app(argc, argv);

    const MainWindow wnd{};

    // style:
    QString qss;
    QFile file1(":/resources/styles/style.qss");

    file1.open(QFile::ReadOnly);

    qss += file1.readAll();
    qApp->setStyleSheet(qss);

    return app.exec();

}
// NOLINTEND(readability-static-accessed-through-instance)
// NOLINTEND(misc-include-cleaner)
