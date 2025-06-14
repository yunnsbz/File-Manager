#include "ApplicationStateHandler.h"
#include "mainwindow.hpp"



ApplicationStateHandler::ApplicationStateHandler(QObject *parent)
    :
    mainWindow_(static_cast<MainWindow*>(parent))
{

}

void ApplicationStateHandler::RestoreViewState()
{
    QSettings settings("Yunnsbz-Fatpound", "File-Manager");
    int saved = settings.value(SettingName_ViewState, static_cast<int>(ViewStates::SINGLE_TABLE_W_TREE)).toInt();
    auto currentTheme = static_cast<ViewStates>(saved);

    switch (currentTheme)
    {
    case ViewStates::SINGLE_TABLE_W_TREE:
        mainWindow_->ActivateTreeView();
        mainWindow_->DeactivateDualPane();
        qDebug() << "SINGLE_TABLE_W_TREE";
        break;
    case ViewStates::SINGLE_TABLE:
        mainWindow_->DeactivateTreeView();
        mainWindow_->DeactivateDualPane();
        qDebug() << "SINGLE_TABLE";
        break;
    case ViewStates::DUAL_PANE_W_TREE:
        mainWindow_->ActivateTreeView();
        mainWindow_->ActivateDualPane();
        qDebug() << "DUAL_PANE_W_TREE";
        break;
    case ViewStates::DUAL_PANE:
        mainWindow_->DeactivateTreeView();
        mainWindow_->ActivateDualPane();
        qDebug() << "DUAL_PANE";
        break;
    case ViewStates::COLUMN_VIEW:
        mainWindow_->ActivateColumnView();
        qDebug() << "COLUMN_VIEW";
        break;
    }
}

void ApplicationStateHandler::SetCurrentViewState(ViewStates newState)
{
    QSettings settings("Yunnsbz-Fatpound", "File-Manager");
    settings.setValue(SettingName_ViewState,static_cast<int>(newState));
    qDebug() << "new state: " << static_cast<int>(newState);
}

ViewStates ApplicationStateHandler::GetCurrentViewState()
{
    QSettings settings("Yunnsbz-Fatpound", "File-Manager");
    int saved = settings.value(SettingName_ViewState, static_cast<int>(ViewStates::SINGLE_TABLE_W_TREE)).toInt();
    return static_cast<ViewStates>(saved);
}
