#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <qobject.h>

class MainWindow;

class ThemeManger : public QObject
{
    Q_OBJECT

public:
    explicit ThemeManger(QObject *parent);
    bool isDarkTheme();

private:
    MainWindow* mainWindow;

};

#endif // THEMEMANAGER_H
