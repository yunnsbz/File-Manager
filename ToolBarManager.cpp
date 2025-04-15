#include "ToolBarManager.h"
#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QStyle>

ToolBarManager::ToolBarManager(QToolBar *toolBar, QObject *parent)
:
    QObject(parent),
    toolBar(toolBar),
    mainWindow(static_cast<MainWindow*>(parent))
{

    // icon updates:
    mainWindow->getUI()->actionback->setIcon(QIcon(":/resources/arrow-left-white.svg"));


    mainWindow->getUI()->actionforward->setIcon(QIcon(":/resources/arrow-right-white.svg"));
}
