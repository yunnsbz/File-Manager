#ifndef APPLICATIONSTATEHANDLER_H
#define APPLICATIONSTATEHANDLER_H

#include <QObject>

enum class ViewStates : char{
    DUAL_PANE,
    SINGLE_TABLE,
    COLUMN_VIEW
};

class ApplicationStateHandler{
    Q_OBJECT

public:
    explicit ApplicationStateHandler(QObject* parent);
    void SetCurrentViewState(ViewStates newState);
    ViewStates GetCurrentViewState();

};

#endif // APPLICATIONSTATEHANDLER_H
