#include "ToolBarManager.h"
#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QStyle>

ToolBarManager::ToolBarManager(QHBoxLayout *toolBar, QObject *parent)
:
    QObject(parent),
    toolBar(toolBar),
    mainWindow(static_cast<MainWindow*>(parent))
{
    // uygulama ilk açıldığında back ve forward history boş olur bu yüzden butonlar disable edilir.
    mainWindow->getUI()->toolBackButton->setDisabled(true);
    mainWindow->getUI()->toolForwardButton->setDisabled(true);
}

void ToolBarManager::SetBackButtonEnabled(bool enabled)
{
    mainWindow->getUI()->toolBackButton->setEnabled(enabled);
}

void ToolBarManager::SetForwardButtonEnabled(bool enabled)
{
    mainWindow->getUI()->toolForwardButton->setEnabled(enabled);
}
