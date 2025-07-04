#include "ApplicationStateHandler.h"
#include "mainwindow.hpp"

ApplicationStateHandler::ApplicationStateHandler(QObject *parent)
    :
    mainWindow_(static_cast<MainWindow*>(parent))
{

}

void ApplicationStateHandler::RestoreViewState()
{
    const QSettings settings("Yunnsbz-Fatpound", "File-Manager");
    const int saved = settings.value(SettingName_ViewState, static_cast<int>(ViewStates::SINGLE_TABLE_W_TREE)).toInt();
    auto currentTheme = static_cast<ViewStates>(saved);

    switch (currentTheme)
    {
    case ViewStates::SINGLE_TABLE_W_TREE:
        mainWindow_->ActivateTreeView();
        mainWindow_->DeactivateDualPane();
        break;
    case ViewStates::SINGLE_TABLE:
        mainWindow_->DeactivateTreeView();
        mainWindow_->DeactivateDualPane();
        break;
    case ViewStates::DUAL_PANE_W_TREE:
        mainWindow_->ActivateTreeView();
        mainWindow_->ActivateDualPane();
        break;
    case ViewStates::DUAL_PANE:
        mainWindow_->DeactivateTreeView();
        mainWindow_->ActivateDualPane();
        break;
    case ViewStates::COLUMN_VIEW:
        mainWindow_->ActivateColumnView();
        break;
    }
}

void ApplicationStateHandler::SetCurrentViewState(ViewStates newState)
{
    QSettings settings("Yunnsbz-Fatpound", "File-Manager");
    settings.setValue(SettingName_ViewState,static_cast<int>(newState));
}

ViewStates ApplicationStateHandler::GetCurrentViewState()
{
    const QSettings settings("Yunnsbz-Fatpound", "File-Manager");
    const int saved = settings.value(SettingName_ViewState, static_cast<int>(ViewStates::SINGLE_TABLE_W_TREE)).toInt();
    return static_cast<ViewStates>(saved);
}
