#include "ApplicationStateHandler.h"
#include "mainwindow.hpp"

#include <QSettings>

ApplicationStateHandler::ApplicationStateHandler(QObject *parent)
    :
    mainWindow_(static_cast<MainWindow*>(parent))
{
    QSettings settings("Yunnsbz-Fatpound", "File-Manager");
    int saved = settings.value(SettingName_ViewState, static_cast<int>(ViewStates::SINGLE_TABLE_W_TREE)).toInt();
    auto currentTheme = static_cast<ViewStates>(saved);

    switch (currentTheme) {
    case ViewStates::SINGLE_TABLE_W_TREE:

        break;
    case ViewStates::SINGLE_TABLE:

        break;
    case ViewStates::DUAL_PANE_W_TREE:

        break;
    case ViewStates::DUAL_PANE:

        break;
    case ViewStates::COLUMN_VIEW:

        break;
    default:
        break;
    }
}
