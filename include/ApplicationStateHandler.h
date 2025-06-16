#ifndef APPLICATIONSTATEHANDLER_H
#define APPLICATIONSTATEHANDLER_H

#include <QObject>
#include <QSettings>

class MainWindow;

enum class ViewStates : char
{
    DUAL_PANE,
    DUAL_PANE_W_TREE,
    SINGLE_TABLE,
    SINGLE_TABLE_W_TREE,
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
    MainWindow* mainWindow_;

    static constexpr const char* SettingName_ViewState = "view_state";
};

#endif // APPLICATIONSTATEHANDLER_H
