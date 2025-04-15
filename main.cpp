#include "mainwindow.hpp"

#include <QApplication>
#include <QStyleFactory>

// NOLINTBEGIN(readability-static-accessed-through-instance)
int main(int argc, char **argv)
{
    const QApplication app(argc, argv);

    const MainWindow wnd{};

    QString styleSheet = R"(

    QToolButton {
        background-color: #00000000;
        border: none;
        padding: 5px;
        border-radius: 5px;
    }

    QTabWidget QToolButton {
        padding: 5px 5px 8px 5px;
    }

    QTabBar::tab {
        background: #444;
        color: white;
        padding: 3px 10px;
        border: none;
        border-radius: 5px;
        margin-left: 2px;
    }

    QToolButton:hover {
        background-color: #999;
    }
)";

    qApp->setStyleSheet(styleSheet);


    return app.exec();

}
// NOLINTEND(readability-static-accessed-through-instance)
