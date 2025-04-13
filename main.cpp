#include "mainwindow.hpp"

#include <QApplication>

// NOLINTBEGIN(readability-static-accessed-through-instance)
int main(int argc, char **argv)
{
    const QApplication app(argc, argv);

    const MainWindow wnd{};

    return app.exec();

}
// NOLINTEND(readability-static-accessed-through-instance)
