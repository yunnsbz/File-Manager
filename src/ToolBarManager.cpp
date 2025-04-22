#include "ToolBarManager.h"
#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QStyle>

ToolBarManager::ToolBarManager(QHBoxLayout* toolBar, QObject* parent)
    :
    QObject(parent),
    toolBar_(toolBar),
    mainWindow_(static_cast<MainWindow*>(parent))
{
    // uygulama ilk açıldığında back ve forward history boş olur bu yüzden butonlar disable edilir.
    mainWindow_->getUI()->toolBackButton->setDisabled(true);
    mainWindow_->getUI()->toolForwardButton->setDisabled(true);
    mainWindow_->getUI()->toolUpButton->setDisabled(true);
}

void ToolBarManager::SetBackButtonEnabled(bool enabled)
{
    mainWindow_->getUI()->toolBackButton->setEnabled(enabled);
}

void ToolBarManager::SetForwardButtonEnabled(bool enabled)
{
    mainWindow_->getUI()->toolForwardButton->setEnabled(enabled);
}

void ToolBarManager::SetUpButtonEnabled(bool enabled)
{
    mainWindow_->getUI()->toolUpButton->setEnabled(enabled);
}
