#include "ThemeManager.h"
#include "mainwindow.hpp"


ThemeManger::ThemeManger(QObject *parent)
    :
    QObject(parent),
    mainWindow(static_cast<MainWindow*>(parent))
{
    const bool DarkTheme = isDarkTheme();
    auto* ui = mainWindow->getUI();

    // cihaz temasını bulup menü ikonlarının ona göre beyaz yada siyah olarak değiştir:
    if (DarkTheme) {
        ui->actionColumn_View->setIcon(QIcon(":/resources/img/view_column_white.svg"));
        ui->actionDetails_View->setIcon(QIcon(":/resources/img/table_rows_white.svg"));
        ui->actionDual_Pane_View->setIcon(QIcon(":/resources/img/two_pager_white.svg"));
        ui->actionTiles_View->setIcon(QIcon(":/resources/img/tiles_view_white.svg"));
        ui->actionTree_View->setIcon(QIcon(":/resources/img/tree_view_white.svg"));
        ui->toolCmdButton->setIcon(QIcon(":/resources/img/terminal_white.svg"));
    } else {
        ui->actionColumn_View->setIcon(QIcon(":/resources/img/view_column_black.svg"));
        ui->actionDetails_View->setIcon(QIcon(":/resources/img/table_rows_black.svg"));
        ui->actionDual_Pane_View->setIcon(QIcon(":/resources/img/two_pager_black.svg"));
        ui->actionTiles_View->setIcon(QIcon(":/resources/img/tiles_view_black.svg"));
        ui->actionTree_View->setIcon(QIcon(":/resources/img/tree_view_black.svg"));
        ui->toolCmdButton->setIcon(QIcon(":/resources/img/terminal_black.svg"));
    }
/*
    // tool button disable olma durumunda otomatik renk değişikliği için
    if (DarkTheme) {
        QIcon icon;
        icon.addPixmap(QPixmap(":/resources/img/share_white.svg"), QIcon::Normal, QIcon::Off);
        icon.addPixmap(QPixmap(":/resources/img/share_gray.svg"), QIcon::Disabled, QIcon::Off);
        //ui->toolShareButton->setIcon(icon);
    } else {
        QIcon icon;
        icon.addPixmap(QPixmap(":/resources/img/share_black.svg"), QIcon::Normal, QIcon::Off);
        icon.addPixmap(QPixmap(":/resources/img/share_gray.svg"), QIcon::Disabled, QIcon::Off);
        ui->toolShareButton->setIcon(icon);
    }
*/
}

bool ThemeManger::isDarkTheme() {
    QPalette palette = qApp->palette(); // qApp yerine QApplication::palette() de olur
    QColor bg = palette.color(QPalette::Window); // pencere arka planı

    // Parlaklık hesaplaması: İnsan gözünün algıladığı parlaklık değeri
    int brightness = (bg.red() * 299 + bg.green() * 587 + bg.blue() * 114) / 1000;

    return brightness < 128; // 0-255; 128'den düşükse koyu temadır
}
