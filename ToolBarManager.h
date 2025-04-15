#ifndef TOOLBARMANAGER_H
#define TOOLBARMANAGER_H

#include <qobject.h>
#include <qtoolbar.h>

class MainWindow;

class ToolBarManager : public QObject{

    Q_OBJECT

public:
    explicit ToolBarManager(QToolBar* toolBar, QObject* parent);


private:
    QToolBar* toolBar;
    MainWindow* mainWindow;

};


#endif // TOOLBARMANAGER_H
