#include "mainwindow.hpp"

#include <QApplication>

// NOLINTBEGIN(misc-include-cleaner)
// NOLINTBEGIN(readability-static-accessed-through-instance)
int main(int argc, char **argv)
{
    const QApplication app(argc, argv);

    MainWindow wnd{};
    wnd.setWindowTitle("File-Manager");
    wnd.show();

    return app.exec();

}
// NOLINTEND(readability-static-accessed-through-instance)
// NOLINTEND(misc-include-cleaner)
