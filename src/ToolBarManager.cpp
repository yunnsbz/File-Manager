#include "ToolBarManager.h"
#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QStyle>

FM_BEGIN_NAMESPACE

ToolBarManager::ToolBarManager(QHBoxLayout* toolBar, QObject* parent)
    :
    QObject(parent),
    m_toolBar_(toolBar),
    m_mainWindow_(static_cast<MainWindow*>(parent))
{
    // uygulama ilk açıldığında back ve forward history boş olur bu yüzden butonlar disable edilir.
    m_mainWindow_->getUI()->toolBackButton->setDisabled(true);
    m_mainWindow_->getUI()->toolForwardButton->setDisabled(true);
    m_mainWindow_->getUI()->toolUpButton->setDisabled(true);
}

void ToolBarManager::setBackButtonEnabled(bool enabled)
{
    m_mainWindow_->getUI()->toolBackButton->setEnabled(enabled);
}

void ToolBarManager::setForwardButtonEnabled(bool enabled)
{
    m_mainWindow_->getUI()->toolForwardButton->setEnabled(enabled);
}

void ToolBarManager::setUpButtonEnabled(bool enabled)
{
    m_mainWindow_->getUI()->toolUpButton->setEnabled(enabled);
}

FM_END_NAMESPACE
