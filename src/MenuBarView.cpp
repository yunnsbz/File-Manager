#include "MenuBarView.h"
#include "ApplicationStateHandler.h"
#include "SettingsDialog.h"
#include "mainwindow.hpp"

#include <QAction>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>

FM_BEGIN_NAMESPACE

MenuBarView::MenuBarView(QObject *parent)
    :
    QObject(parent),
    m_mainWindow(static_cast<MainWindow*>(parent))
{
    connect(m_mainWindow->getActionAbout(), &QAction::triggered, this, &MenuBarView::onAboutTriggered);
    connect(m_mainWindow->getActionColumnView(), &QAction::triggered, this, &MenuBarView::onColumnViewTriggered);
    connect(m_mainWindow->getActionExit(), &QAction::triggered, this, &MenuBarView::onExitTriggered);
    connect(m_mainWindow->getActionTreeView(), &QAction::triggered, this, &MenuBarView::onTreeViewTriggered);
    connect(m_mainWindow->getActionSettings(), &QAction::triggered, this, &MenuBarView::onSettingsTriggered);
    connect(m_mainWindow->getActionDualPaneView(), &QAction::triggered, this, &MenuBarView::onDualPaneViewTriggered);
}

void MenuBarView::onColumnViewTriggered()
{
    if(m_mainWindow->getStackedWidget()->currentIndex() == 1){
        m_mainWindow->DeactivateColumnView();
        if(m_mainWindow->isDualPaneActive()){
            if(m_mainWindow->isTreeViewActive()){
                ApplicationStateHandler::SetCurrentViewState(ViewStates::DUAL_PANE_W_TREE);
            }
            else{
                ApplicationStateHandler::SetCurrentViewState(ViewStates::DUAL_PANE);
            }
        }
        else{
            if(m_mainWindow->isTreeViewActive()){
                ApplicationStateHandler::SetCurrentViewState(ViewStates::SINGLE_TABLE_W_TREE);
            }
            else{
                ApplicationStateHandler::SetCurrentViewState(ViewStates::SINGLE_TABLE);
            }
        }
    }
    else{
        m_mainWindow->ActivateColumnView();
        ApplicationStateHandler::SetCurrentViewState(ViewStates::COLUMN_VIEW);
    }
}

void MenuBarView::onExitTriggered()
{
    m_mainWindow->close();
}

void MenuBarView::onTreeViewTriggered()
{
    if (m_mainWindow->isTreeViewActive())
    {
        m_mainWindow->DeactivateTreeView();

        // save state update
        if(m_mainWindow->isDualPaneActive()){
            ApplicationStateHandler::SetCurrentViewState(ViewStates::DUAL_PANE);
        }
        else{
            ApplicationStateHandler::SetCurrentViewState(ViewStates::SINGLE_TABLE);
        }
    }
    else
    {
        m_mainWindow->ActivateTreeView();

        // save state update
        if(m_mainWindow->isDualPaneActive()){
            ApplicationStateHandler::SetCurrentViewState(ViewStates::DUAL_PANE_W_TREE);
        }
        else{
            ApplicationStateHandler::SetCurrentViewState(ViewStates::SINGLE_TABLE_W_TREE);
        }
    }
}

void MenuBarView::onSettingsTriggered()
{
    m_mainWindow->getSettingsWindow()->exec();
}

void MenuBarView::onDualPaneViewTriggered()
{
    // if current stackedWidget is not on column view then open or close dual pane
    if (m_mainWindow->getStackedWidget()->currentIndex() == 0)
    {
        if (m_mainWindow->isDualPaneActive())
        {
            m_mainWindow->DeactivateDualPane();

            // save state update
            if(m_mainWindow->isTreeViewActive())
                ApplicationStateHandler::SetCurrentViewState(ViewStates::SINGLE_TABLE_W_TREE);
            else
                ApplicationStateHandler::SetCurrentViewState(ViewStates::SINGLE_TABLE);
        }
        else
        {
            m_mainWindow->ActivateDualPane();

            // save state update
            if(m_mainWindow->isTreeViewActive())
                ApplicationStateHandler::SetCurrentViewState(ViewStates::DUAL_PANE_W_TREE);
            else
                ApplicationStateHandler::SetCurrentViewState(ViewStates::DUAL_PANE);
        }
    }
    else{
        // if column view is active the deactivate it and open dual pane
        m_mainWindow->DeactivateColumnView();

        // column'dan çıktıktan sonra dual pane açılmalı:
        m_mainWindow->ActivateDualPane();
    }
}

void MenuBarView::onAboutTriggered()
{
    QDialog dialog(m_mainWindow);
    dialog.setWindowTitle("About");

    auto* layout = new QVBoxLayout(&dialog);

    auto* label = new QLabel("Made by fatpound & yunns\n\nCopyright (c) 2025\n\n[ early development ]", &dialog);
    label->setAlignment(Qt::AlignmentFlag::AlignCenter);
    label->setWordWrap(true);
    layout->addWidget(label);

    auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, &dialog);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    layout->addWidget(buttonBox);

    dialog.setLayout(layout);
    dialog.setFixedSize(300, 200);
    dialog.exec();
}

FM_END_NAMESPACE
