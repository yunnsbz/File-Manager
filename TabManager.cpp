#include "TabManager.h"
#include "mainwindow.hpp"
#include "ToolBarManager.h"
#include "FileOperations.h"

#include <QToolButton>
#include <QTabBar>
#include <qboxlayout.h>
#include <qfilesystemmodel.h>

TabManager::TabManager(QTabWidget* tabWidget,  QObject* parent)
    :
    QObject(parent),
    tabWidget(tabWidget),
    mainWindow(static_cast<MainWindow*>(parent))
{
    Setup_();
}

void TabManager::Setup_()
{
    // yeni sekme ekleme butonunun eklenmesi, işlevi ve ilk açılıştaki sekmeleri ayarlama

    // default olarak gelen ikinci sekmeyi kaldır:
    tabWidget->removeTab(1);

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

    FileOperations::swapTabModelIndexMap(toIndex, fromIndex);
    mainWindow->OnTabMoved(toIndex, fromIndex);


    // lastLeftTabIndex is the one moved it should register
    if (fromIndex == lastLeftTabIndex)
    {
        lastLeftTabIndex = toIndex;
    }
    else if (toIndex == lastRightTabIndex)
    {
        lastLeftTabIndex = fromIndex;
    }
}

auto TabManager::GetSplitter()
{
    return tabWidget->widget(lastLeftTabIndex)->findChild<QSplitter*>();
}

void TabManager::addNewTab()
{
    auto* currentSplitter = GetSplitter();

    if (currentSplitter != nullptr)
    {
        auto* newTabWidget = new QWidget();
        auto* layout = new QVBoxLayout(newTabWidget);
        layout->addWidget(currentSplitter);
        layout->setContentsMargins(0, 0, 0, 0);

        tabWidget->addTab(newTabWidget, "new tab");
        tabWidget->setCurrentIndex(tabWidget->count() - 1);
        lastLeftTabIndex = tabWidget->count() - 1;

        mainWindow->SetTabContent(tabWidget->currentIndex());
    }
}

void TabManager::moveTabWidget(int index)
{
    // sekme içerisini kopyalamak yerine taşıyarak sekmeler arasında geçiş yaparız

    auto* currentSplitter = GetSplitter();

    if (currentSplitter != nullptr)
    {
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

        lastLeftTabIndex = index;
    }
}
