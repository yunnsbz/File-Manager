#ifndef APPLICATIONSTATEHANDLER_H
#define APPLICATIONSTATEHANDLER_H

#include <QObject>
#include <QSettings>
#include <FM_Macros.hpp>
FM_BEGIN_NAMESPACE

class MainWindow;

enum class ViewStates : char
{
    DUAL_PANE,
    DUAL_PANE_WITH_TREE,
    SINGLE_PANE,
    SINGLE_PANE_WITH_TREE,
    COLUMN_VIEW
};

class ApplicationStateHandler : public QObject
{
    Q_OBJECT

public:
    explicit ApplicationStateHandler(QObject* parent);

    static void SetCurrentViewState(ViewStates newState);
    static ViewStates GetCurrentViewState();

    void RestoreViewState();


private:
    MainWindow* m_mainWindow_;

    static constexpr const char* SettingName_ViewState = "view_state";
};

FM_END_NAMESPACE
#endif // APPLICATIONSTATEHANDLER_H
