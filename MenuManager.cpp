#include "MenuManager.h"
#include "mainwindow.hpp"


MenuManger::MenuManger(QObject *parent)
    :
    QObject(parent),
    mainWindow(static_cast<MainWindow*>(parent))
{
    const bool isDarkTheme = mainWindow->isDarkTheme();
    auto* ui = mainWindow->getUI();

    // cihaz temasını bulup menü ikonlarının ona göre beyaz yada siyah olarak değiştir:
    if (isDarkTheme) {
        ui->actionColumn_View->setIcon(QIcon(":/resources/img/view_column_white.svg"));
        ui->actionDetails_View->setIcon(QIcon(":/resources/img/table_rows_white.svg"));
        ui->actionDual_Pane_View->setIcon(QIcon(":/resources/img/two_pager_white.svg"));
        ui->actionTiles_View->setIcon(QIcon(":/resources/img/tiles_view_white.svg"));
        ui->actionTree_View->setIcon(QIcon(":/resources/img/tree_view_white.svg"));
    } else {
        ui->actionColumn_View->setIcon(QIcon(":/resources/img/view_column_black.svg"));
        ui->actionDetails_View->setIcon(QIcon(":/resources/img/table_rows_black.svg"));
        ui->actionDual_Pane_View->setIcon(QIcon(":/resources/img/two_pager_black.svg"));
        ui->actionTiles_View->setIcon(QIcon(":/resources/img/tiles_view_black.svg"));
        ui->actionTree_View->setIcon(QIcon(":/resources/img/tree_view_black.svg"));
    }

}
