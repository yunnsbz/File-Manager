#include "TabManager.h"
#include "mainwindow.hpp"
#include <QToolButton>
#include <QTabBar>
#include <qboxlayout.h>


TabManager::TabManager(QTabWidget* tabWidget, QFileSystemModel* fileModel, QObject* parent)
    :
    QObject(parent),
    tabWidget(tabWidget),
    fileModel(fileModel),
    mainWindow(static_cast<MainWindow*>(parent))
{
    // Başlangıçta tab setup yapılabilir
    setup(); // bu senin daha önce yazdığın setupTabs() fonksiyonu olur.
}

// yeni sekme ekleme butonunun eklenmesi, işlevi ve ilk açılıştaki sekmeleri ayarlama
void TabManager::setup() {
    // Başlangıçta bir sekme ve bir '+' sekmesi ekle
    tabWidget->removeTab(1);

    connect(tabWidget->tabBar(), &QTabBar::tabMoved, this, &TabManager::onTabMoved);

    auto* addTabButton = new QToolButton();
    addTabButton->setText("+");
    tabWidget->setCornerWidget(addTabButton, Qt::TopLeftCorner);

    // add button onClick:
    connect(addTabButton, &QToolButton::clicked, this, &TabManager::addNewTab);

    tabWidget->installEventFilter(this);
}

// sekme içeriğini kaydetmek içindir.
void TabManager::setFileIndexMap(QTableView* tableView)
{
    auto currentTab = tabWidget->currentIndex();
    const QModelIndex index = tableView->rootIndex();
    tabContentMap[currentTab] = index;
}

QModelIndex TabManager::getTabModelIndex(int tabIndex) const
{
    if (!tabContentMap.contains(tabIndex)){
        return QModelIndex();
    }

    return tabContentMap[tabIndex];
}

void TabManager::RemoveTabContent(int tabIndex)
{
    tabContentMap.remove(tabIndex);
}

void TabManager::onTabMoved(int to, int from) {
    //qDebug() << "Sekme" << from << "indeksinden" << to << "indeksine taşındı";

    if (from == to) return;

    // İki öğeyi birbirleriyle takas et
    QModelIndex temp = tabContentMap.value(from);
    tabContentMap[from] = tabContentMap.value(to);
    tabContentMap[to] = temp;

    // lastLeftTabIndex is the one moved it should register
    if(from == lastLeftTabIndex){
        lastLeftTabIndex = to;
    }else if(to == lastRightTabIndex){
        lastLeftTabIndex = from;
    }
}

void TabManager::addNewTab()
{
    QWidget* currentTabWidget = tabWidget->currentWidget();
    auto* currentSplitter = currentTabWidget->findChild<QSplitter*>();

    if (currentSplitter != nullptr) {
        auto* newTabWidget = new QWidget();
        auto* layout = new QVBoxLayout(newTabWidget);
        layout->addWidget(currentSplitter);
        layout->setContentsMargins(0,0,0,0);

        tabWidget->addTab(newTabWidget, "new tab");
        tabWidget->setCurrentIndex(tabWidget->count() - 1);
        lastLeftTabIndex = tabWidget->count() - 1;

        mainWindow->SetTabContent(tabWidget->currentIndex());
    }
}

// sekme içerisini kopyalamak yerine taşıyarak sekmeler arasında geçiş yaparız
void TabManager::moveTabWidget(int index)
{
    QWidget* currentTabWidget = tabWidget->widget(lastLeftTabIndex);
    QSplitter* currentSplitter = currentTabWidget->findChild<QSplitter*>();

    if (currentSplitter != nullptr) {

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


