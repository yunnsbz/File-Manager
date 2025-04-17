#include "TabManager.h"
#include "mainwindow.hpp"
#include "ToolBarManager.h"
#include "FileModelOperations.h"

#include <QToolButton>
#include <QTabBar>
#include <QBoxLayout>
#include <QFileSystemModel>

TabManager::TabManager(QTabWidget* tabWidget, QObject* parent)
    :
    QObject(parent),
    mainWindow(static_cast<MainWindow*>(parent)),
    tabWidget(tabWidget)
{
    Setup_();
}

void TabManager::Setup_()
{
    // yeni sekme ekleme butonunun eklenmesi, işlevi ve ilk açılıştaki sekmeleri ayarlama

    connect(tabWidget->tabBar(), &QTabBar::tabMoved, this, &TabManager::onTabMoved);

    auto* addTabButton = new QToolButton();
    addTabButton->setText("+");
    tabWidget->setCornerWidget(addTabButton, Qt::TopLeftCorner);

    // add button onClick:
    connect(addTabButton, &QToolButton::clicked, this, &TabManager::addNewTab);

    tabWidget->installEventFilter(this);
}

void TabManager::onTabMoved(int toIndex, int fromIndex)
{
    //qDebug() << "Sekme" << fromIndex << "indeksinden" << toIndex << "indeksine taşındı";

    if (fromIndex == toIndex)
    {
        return;
    }

    FileModelOperations::swapTabModelIndexMap(toIndex, fromIndex);
    FileModelOperations::swapTabHistoryModelIndex(toIndex, fromIndex);
    mainWindow->OnTabMoved(toIndex, fromIndex);


    // if lastLeftTabIndex is the one moved it should register
    if (fromIndex == _previousLeftTabIndex)
    {
        _previousLeftTabIndex = toIndex;
        persistentPreviousLeftTabIndex = toIndex;
    }
    else if (toIndex == lastRightTabIndex)
    {
        _previousLeftTabIndex = fromIndex;
        persistentPreviousLeftTabIndex = fromIndex;
    }
}

auto TabManager::GetPreviousSplitter()
{
    return tabWidget->widget(_previousLeftTabIndex)->findChild<QSplitter*>();
}

auto TabManager::_getPreviousLeftTabIndex() const -> int
{
    return _previousLeftTabIndex;
}

int TabManager::getPersistentPreviousLeftTabIndex() const
{
    return persistentPreviousLeftTabIndex;
}

void TabManager::setPreviousLeftTabIndex(int value)
{
    persistentPreviousLeftTabIndex = _previousLeftTabIndex;
    _previousLeftTabIndex = value;
}

void TabManager::addNewTab()
{
    auto* currentSplitter = GetPreviousSplitter();

    if (currentSplitter == nullptr)
    {
        return;
    }

    auto* newTabWidget = new QWidget();
    auto* layout = new QVBoxLayout(newTabWidget);
    layout->addWidget(currentSplitter);
    layout->setContentsMargins(0, 0, 0, 0);

    tabWidget->addTab(newTabWidget, "new tab");
    tabWidget->setCurrentIndex(tabWidget->count() - 1);

    setPreviousLeftTabIndex(tabWidget->count() - 1);

    mainWindow->SetTabContent(tabWidget->currentIndex());
}

// void TabManager::addTab(const QString& title)
// {
//     TabData newTab;
//
//     newTab.tabWidget = new QWidget;
//     newTab.treeView = new QTreeView(newTab.tabWidget);
//     newTab.tableView = new QTableView(newTab.tabWidget);
//
//     QVBoxLayout* layout = new QVBoxLayout(newTab.tabWidget);
//     layout->addWidget(newTab.treeView);
//     layout->addWidget(newTab.tableView);
//     newTab.tabWidget->setLayout(layout);
//
//     tabWidget->addTab(newTab.tabWidget, title);
//     m_tabs_.append(newTab);
// }


void TabManager::moveTabWidget(int index)
{
    // sekme içerisini kopyalamak yerine taşıyarak sekmeler arasında geçiş yaparız

    auto* currentSplitter = GetPreviousSplitter();

    if (currentSplitter == nullptr)
    {
        return;
    }

    // Splitter'ı mevcut yerinden kopar
    currentSplitter->setParent(nullptr);

    // Yeni container oluştur
    auto* newTabWidget = new QWidget();
    auto* layout = new QVBoxLayout(newTabWidget);
    layout->addWidget(currentSplitter);
    layout->setContentsMargins(0, 0, 0, 0);

    // Yeni sekmeyi oluştur
    tabWidget->insertTab(index, newTabWidget, tabWidget->tabText(index));
    tabWidget->removeTab(index + 1); // eski widget'ı kaldır
    tabWidget->setCurrentIndex(index);

    setPreviousLeftTabIndex(index);
}
