#include "TabManager.h"
#include "mainwindow.hpp"
#include "ToolBarManager.h"
#include "ThemeManager.h"

#include <QToolButton>
#include <QTabBar>
#include <QBoxLayout>
#include <QFileSystemModel>

TabManager::TabManager(QTabWidget* tabWidget, bool forRightPane, QObject* parent)
    :
    QObject(parent),
    mainWindow(static_cast<MainWindow*>(parent)),
    tabWidget(tabWidget),
    forRightPane_(forRightPane)
{
    Setup_();
}

void TabManager::EnableNavWidget(bool enable)
{
    if(enable){
        cornerNavButtons->show();
        tabWidget->setCornerWidget(cornerNavButtons, Qt::TopRightCorner);
    }
    else{
        tabWidget->setCornerWidget(nullptr, Qt::TopRightCorner);
    }
}

void TabManager::Setup_()
{
    // yeni sekme ekleme butonunun eklenmesi, işlevi ve ilk açılıştaki sekmeleri ayarlama

    connect(tabWidget->tabBar(), &QTabBar::tabMoved, this, &TabManager::onTabMoved);

    cornerNavButtons = new QWidget;
    cornerNavButtons->setObjectName("cornerNavButtons");
    auto *layout = new QHBoxLayout(cornerNavButtons);
    layout->setContentsMargins(0, 0, 0, 0); // sıkı olsun

    auto* backTabButton = new QToolButton();
    auto* forwTabButton = new QToolButton();
    auto* upTabButton = new QToolButton();

    bool isDarkTheme = ThemeManger::isDarkTheme();
    if(isDarkTheme){
        const QIcon backIcon(":/resources/img/arrow_circle_left_white.svg");
        backTabButton->setIcon(backIcon);
        const QIcon forwardIcon(":/resources/img/arrow_circle_right_white.svg");
        forwTabButton->setIcon(forwardIcon);
        const QIcon upIcon(":/resources/img/arrow_circle_up_white.svg");
        upTabButton->setIcon(upIcon);
    }
    else{
        const QIcon backIcon(":/resources/img/arrow_circle_left_black.svg");
        backTabButton->setIcon(backIcon);
        const QIcon forwardIcon(":/resources/img/arrow_circle_right_black.svg");
        forwTabButton->setIcon(forwardIcon);
        const QIcon upIcon(":/resources/img/arrow_circle_up_black.svg");
        upTabButton->setIcon(upIcon);
    }

    layout->addWidget(upTabButton);
    layout->addWidget(backTabButton);
    layout->addWidget(forwTabButton);

    auto* addTabButton = new QToolButton();
    addTabButton->setText("+");
    tabWidget->setCornerWidget(addTabButton, Qt::TopLeftCorner);
    EnableNavWidget(true);

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

    if (forRightPane_)
    {
        mainWindow->OnTabMoved2(toIndex, fromIndex);
    }
    else
    {
        mainWindow->OnTabMoved(toIndex, fromIndex);
    }

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

auto TabManager::getPersistentPreviousLeftTabIndex() const ->int
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

    mainWindow->SetTabContent(tabWidget->currentIndex(), forRightPane_);
}

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
