#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QObject>
#include <FM_Macros.hpp>
FM_BEGIN_NAMESPACE


class MainWindow;

class ThemeManger : public QObject
{
    Q_OBJECT

public:
    explicit ThemeManger(QObject *parent);
    static bool isDarkTheme();

private:
    MainWindow* m_mainWindow;

};

FM_END_NAMESPACE
#endif // THEMEMANAGER_H
