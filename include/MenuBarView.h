#ifndef MENUBARVIEW_H
#define MENUBARVIEW_H

#include <QObject>

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

#endif // MENUBARVIEW_H
