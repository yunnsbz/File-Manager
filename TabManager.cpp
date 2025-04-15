#include "TabManager.h"
#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QToolButton>
#include <QTabBar>
#include <qboxlayout.h>
#include <qfilesystemmodel.h>

TabManager::TabManager(QTabWidget* tabWidget, QFileSystemModel* fileModel, QObject* parent)
    :
    QObject(parent),
    tabWidget(tabWidget),
    fileModel(fileModel),
    mainWindow(static_cast<MainWindow*>(parent))
{
    Setup_();
}

void TabManager::Setup_()
{
    // yeni sekme ekleme butonunun eklenmesi, işlevi ve ilk açılıştaki sekmeleri ayarlama

    // Başlangıçta bir sekme ve bir '+' sekmesi ekle
    tabWidget->removeTab(1);

    connect(tabWidget->tabBar(), &QTabBar::tabMoved, this, &TabManager::onTabMoved);

    auto* addTabButton = new QToolButton();
    addTabButton->setText("+");
    tabWidget->setCornerWidget(addTabButton, Qt::TopLeftCorner);

    // add button onClick:
    connect(addTabButton, &QToolButton::clicked, this, &TabManager::addNewTab);

    // tree view açılma ve kapanma durumlarında değişiklikleri tabContents içine kaydetmeliyiz:
    connect(mainWindow->getUI()->FileTreeView, &QTreeView::expanded, this, [=, this](const QModelIndex &index) {
        QString path = fileModel->filePath(index);
        int currentTab = tabWidget->currentIndex();
        tabContentMap[currentTab].ExpandedPaths.insert(path);
    });

    connect(mainWindow->getUI()->FileTreeView, &QTreeView::collapsed, this, [=, this](const QModelIndex &index) {
        QString path = fileModel->filePath(index);
        int currentTab = tabWidget->currentIndex();
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
    if(tabContentMap.contains(currentTab)){
        tabContentMap[currentTab].ModelIndex = index;
        // expanded paths signal ile set edilir.
    }
    else{
        tabContentMap[currentTab] = TabContent(index,*new QSet<QString>());
    }

}

QModelIndex TabManager::getTabModelIndex(int tabIndex) const
{
    if (!tabContentMap.contains(tabIndex))
    {
        return QModelIndex();
    }

    return tabContentMap[tabIndex].ModelIndex;
}

QSet<QString> TabManager::getTreeExpandedPaths(int tabIndex) const
{
    if (!tabContentMap.contains(tabIndex))
    {
        return QSet<QString>();
    }
    return tabContentMap[tabIndex].ExpandedPaths;
}

void TabManager::RemoveTabContent(int tabIndex)
{
    tabContentMap.remove(tabIndex);
}

void TabManager::onTabMoved(int to, int from)
{
    //qDebug() << "Sekme" << from << "indeksinden" << to << "indeksine taşındı";

    if (from == to)
    {
        return;
    }

    // İki öğeyi birbirleriyle takas et
    TabContent temp = tabContentMap.value(from);
    tabContentMap[from] = tabContentMap.value(to);
    tabContentMap[to] = temp;

    // lastLeftTabIndex is the one moved it should register
    if (from == lastLeftTabIndex)
    {
        lastLeftTabIndex = to;
    }
    else if (to == lastRightTabIndex)
    {
        lastLeftTabIndex = from;
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
