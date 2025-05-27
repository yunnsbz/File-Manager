#ifndef APPLICATIONSTATEHANDLER_H
#define APPLICATIONSTATEHANDLER_H


#include <QObject>

class MainWindow;

enum class ViewStates : char{
    DUAL_PANE,
    DUAL_PANE_W_TREE,
    SINGLE_TABLE,
    SINGLE_TABLE_W_TREE,
    COLUMN_VIEW
};

class ApplicationStateHandler{
    Q_OBJECT

public:
    explicit ApplicationStateHandler(QObject* parent);
    void SetCurrentViewState(ViewStates newState);
    ViewStates GetCurrentViewState();

private:
    MainWindow* mainWindow_;

    static inline const char* SettingName_ViewState = "view_state";
};

#endif // APPLICATIONSTATEHANDLER_H
