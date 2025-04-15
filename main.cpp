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
        height: 100%;
    }

    QTabBar::tab {
        background-color: #22222255;
        padding: 3px 20px;
        border: none;
        border-radius: 5px;
        margin-left: 2px;
    }
    QTabBar::tab:selected {
        background: #222222ff;
    }

    QToolButton:hover {
        background-color: #999;
    }
)";

    qApp->setStyleSheet(styleSheet);
    app.setStyle("windows11");

    return app.exec();

}
// NOLINTEND(readability-static-accessed-through-instance)
