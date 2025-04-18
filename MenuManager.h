#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include <qobject.h>

class MainWindow;

class MenuManger : public QObject
{
    Q_OBJECT

public:
    explicit MenuManger(QObject *parent);


private:
    MainWindow* mainWindow;
};

#endif // MENUMANAGER_H
