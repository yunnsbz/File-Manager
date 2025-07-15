#ifndef MENUBARVIEW_H
#define MENUBARVIEW_H

#include <QObject>

#include <FM_Macros.hpp>
FM_BEGIN_NAMESPACE

class MainWindow;

class MenuBarView : public QObject
{
    Q_OBJECT

public:
    explicit MenuBarView(QObject* parent);

    void onAboutTriggered();
    void onColumnViewTriggered();
    void onDualPaneViewTriggered();
    void onExitTriggered();
    void onSettingsTriggered();
    void onTreeViewTriggered();

private:
    MainWindow* m_mainWindow;

};

FM_END_NAMESPACE
#endif // MENUBARVIEW_H
