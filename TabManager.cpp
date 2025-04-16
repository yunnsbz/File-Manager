#include "TabManager.h"
#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "ToolBarManager.h"

#include <QToolButton>
#include <QTabBar>
#include <qboxlayout.h>
#include <qfilesystemmodel.h>

TabManager::TabManager(QTabWidget* tabWidget, QFileSystemModel* fileModel, ToolBarManager* toolBarManager, QObject* parent)
    :
    QObject(parent),
    tabWidget(tabWidget),
    fileModel(fileModel),
    mainWindow(static_cast<MainWindow*>(parent)),
    toolBarManager(toolBarManager)
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

    // tree view açılma ve kapanma durumlarında değişiklikleri tabContents içine kaydetmeliyiz:
    connect(mainWindow->getUI()->FileTreeView, &QTreeView::expanded, this, [this](const QModelIndex &index) {
        const QString path = fileModel->filePath(index);
        const int currentTab = tabWidget->currentIndex();
        tabContentMap[currentTab].ExpandedPaths.insert(path);
    });

    connect(mainWindow->getUI()->FileTreeView, &QTreeView::collapsed, this, [this](const QModelIndex &index) {
        const QString path = fileModel->filePath(index);
        const int currentTab = tabWidget->currentIndex();
        tabContentMap[currentTab].ExpandedPaths.remove(path);
    });

    tabWidget->installEventFilter(this);
}

void TabManager::setFileIndexMap(QTableView* tableView)
{
    // sekme içeriğini kaydetmek için

    auto currentTab = tabWidget->currentIndex();
    const QModelIndex index = tableView->rootIndex();

    // sekme içeriğini tutan struct içerisinde indexsi set et. eğer yoksa tree view için expanded paths listesini de oluşturur.
    if(tabContentMap.contains(currentTab))
    {
        // yeni bir yer açıldığında geçmiş set edilir
        BackHistoryTabContent.append(tabContentMap[currentTab]);
        qDebug()<< "history added";

        // yeni index'e geç:
        tabContentMap[currentTab].ModelIndex = index;
    }
    else
    {
        tabContentMap[currentTab] = TabContent(index,*new QSet<QString>());
    }
    // boş sayfa açıldığında ileri geçmiş silinir:
    ForwardHistoryTabContent.clear();

    // buton kontrolü:
    toolBarManager->SetBackButtonEnabled(!BackHistoryTabContent.isEmpty());
    toolBarManager->SetForwardButtonEnabled(!ForwardHistoryTabContent.isEmpty());
}

auto TabManager::getTabModelIndex(int tabIndex) const -> QModelIndex
{
    if (!tabContentMap.contains(tabIndex))
    {
        return {};
    }

    return tabContentMap[tabIndex].ModelIndex;
}

auto TabManager::getTreeExpandedPaths(int tabIndex) const -> QSet<QString>
{
    if (!tabContentMap.contains(tabIndex))
    {
        return {};
    }
    return tabContentMap[tabIndex].ExpandedPaths;
}

void TabManager::RemoveTabContent(int tabIndex)
{
    // sekme silinir:
    tabContentMap.remove(tabIndex);

    // sekmeden sonra gelenlerin map key'leri (tabIndex'leri) bir geriye alınır:
    auto keys = tabContentMap.keys();
    std::sort(keys.begin(), keys.end(), std::greater<int>()); // sıralanmazsa yanlış sekmenin üzerine yazabilir
    for(auto _tabIndex : keys)
    {
        if(_tabIndex > tabIndex)
        {
            auto content = tabContentMap[_tabIndex];
            tabContentMap.remove(_tabIndex);
            tabContentMap[_tabIndex-1] = content;
        }
    }
}

void TabManager::onBackButtonClicked()
{
    if (!BackHistoryTabContent.isEmpty()) {
        ForwardHistoryTabContent.append(tabContentMap[tabWidget->currentIndex()]);
        auto backContent = BackHistoryTabContent.takeLast();
        tabContentMap[tabWidget->currentIndex()] = backContent;
        mainWindow->SetTabContent(tabWidget->currentIndex());

        // buton kontrolü:
        toolBarManager->SetBackButtonEnabled(!BackHistoryTabContent.isEmpty());
        toolBarManager->SetForwardButtonEnabled(!ForwardHistoryTabContent.isEmpty());
    }
}

void TabManager::onForwardButtonClicked()
{
    if (!ForwardHistoryTabContent.isEmpty()) {
        BackHistoryTabContent.append(tabContentMap[tabWidget->currentIndex()]);
        auto backContent = ForwardHistoryTabContent.takeLast();
        tabContentMap[tabWidget->currentIndex()] = backContent;
        mainWindow->SetTabContent(tabWidget->currentIndex());

        // buton kontrolü:
        toolBarManager->SetBackButtonEnabled(!BackHistoryTabContent.isEmpty());
        toolBarManager->SetForwardButtonEnabled(!ForwardHistoryTabContent.isEmpty());
    }
}

void TabManager::onTabMoved(int toIndex, int fromIndex)
{
    //qDebug() << "Sekme" << fromIndex << "indeksinden" << toIndex << "indeksine taşındı";

    if (fromIndex == toIndex)
    {
        return;
    }

    // İki öğeyi birbirleriyle takas et
    const TabContent temp = tabContentMap.value(fromIndex);
    tabContentMap[fromIndex] = tabContentMap.value(toIndex);
    tabContentMap[toIndex] = temp;

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

void TabManager::addNewTab()
{
    auto* currentTabWidget = tabWidget->currentWidget();
    auto* currentSplitter  = currentTabWidget->findChild<QSplitter*>();

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

    auto* currentTabWidget = tabWidget->widget(lastLeftTabIndex);
    auto* currentSplitter = currentTabWidget->findChild<QSplitter*>();

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
