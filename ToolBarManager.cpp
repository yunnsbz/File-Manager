#include "ToolBarManager.h"
#include "mainwindow.hpp"

#include <QStyle>

ToolBarManager::ToolBarManager(QHBoxLayout *toolBar, QObject *parent)
:
    QObject(parent),
    toolBar(toolBar),
    mainWindow(static_cast<MainWindow*>(parent))
{
    // Currently disabled. Will be enabled once themes are implemented
    // icon updates:
    //mainWindow->getUI()->actionback->setIcon(QIcon(":/resources/img/arrow-left-white.svg"));
    //mainWindow->getUI()->actionforward->setIcon(QIcon(":/resources/img/arrow-right-white.svg"));
}
