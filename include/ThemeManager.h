#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>

class MainWindow;

class ThemeManger : public QObject
{
    Q_OBJECT

public:
    explicit ThemeManger(QObject *parent);
    static bool isDarkTheme();

private:
    MainWindow* mainWindow;

};

#endif // THEMEMANAGER_H
