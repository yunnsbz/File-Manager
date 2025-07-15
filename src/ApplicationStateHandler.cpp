#include "ApplicationStateHandler.h"
#include "mainwindow.hpp"

FM_BEGIN_NAMESPACE

ApplicationStateHandler::ApplicationStateHandler(QObject *parent)
    :
    m_mainWindow_(static_cast<MainWindow*>(parent))
{

}

void ApplicationStateHandler::RestoreViewState()
{
    const QSettings settings("Yunnsbz-Fatpound", "File-Manager");
    const int saved = settings.value(SettingName_ViewState, static_cast<int>(ViewStates::SINGLE_PANE_WITH_TREE)).toInt();
    auto currentTheme = static_cast<ViewStates>(saved);

    switch (currentTheme)
    {
    case ViewStates::SINGLE_PANE_WITH_TREE:
        m_mainWindow_->activateTreeView();
        m_mainWindow_->deactivateDualPane();
        break;
    case ViewStates::SINGLE_PANE:
        m_mainWindow_->deactivateTreeView();
        m_mainWindow_->deactivateDualPane();
        break;
    case ViewStates::DUAL_PANE_WITH_TREE:
        m_mainWindow_->activateTreeView();
        m_mainWindow_->activateDualPane();
        break;
    case ViewStates::DUAL_PANE:
        m_mainWindow_->deactivateTreeView();
        m_mainWindow_->activateDualPane();
        break;
    case ViewStates::COLUMN_VIEW:
        m_mainWindow_->activateColumnView();
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
    const int saved = settings.value(SettingName_ViewState, static_cast<int>(ViewStates::SINGLE_PANE_WITH_TREE)).toInt();
    return static_cast<ViewStates>(saved);
}

FM_END_NAMESPACE
