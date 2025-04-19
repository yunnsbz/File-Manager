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
    // sekmelerin sürüklenmesi/yer değiştirmesi hareketlerini algılamak için:
    connect(tabWidget->tabBar(), &QTabBar::tabMoved, this, &TabManager::onTabMoved);

    SetCornerNavButtons();

    SetAddTabButton();

    tabWidget->installEventFilter(this);

    // corner nav button'ları en başta root directory'de olduklarından disable edilir
    updateNavButtons(false, false, false);
}

void TabManager::SetAddTabButton()
{
    auto* addTabButton = new QToolButton();
    addTabButton->setObjectName("add-tab-button");
    addTabButton->setText("+");
    tabWidget->setCornerWidget(addTabButton, Qt::TopLeftCorner);
    EnableNavWidget(true);

    // add button onClick:
    connect(addTabButton, &QToolButton::clicked, this, &TabManager::addNewTab);
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

void TabManager::updateNavButtons(bool BackEnable, bool ForwardEnable, bool UpEnabled)
{
    backTabButton->setEnabled(BackEnable);
    forwTabButton->setEnabled(ForwardEnable);
    upTabButton->setEnabled(UpEnabled);
}

void TabManager::SetCornerNavButtons()
{
    cornerNavButtons = new QWidget;
    cornerNavButtons->setObjectName("cornerNavButtons");
    auto *layout = new QHBoxLayout(cornerNavButtons);
    layout->setContentsMargins(0, 0, 0, 0);

    backTabButton = new QToolButton();
    forwTabButton = new QToolButton();
    upTabButton = new QToolButton();

    SetNavButtonThemes();

    // adding nav buttons to a parent widget
    layout->addWidget(backTabButton);
    layout->addWidget(forwTabButton);
    layout->addWidget(upTabButton);

    connect(upTabButton, &QToolButton::clicked, this, [this]{mainWindow->upperFolderOnClick(forRightPane_);});
    connect(forwTabButton, &QToolButton::clicked, this, [this]{mainWindow->ForwardButtonOnClick(forRightPane_);});
    connect(backTabButton, &QToolButton::clicked, this, [this]{mainWindow->BackButtonOnClick(forRightPane_);});
}

void TabManager::SetNavButtonThemes()
{
    bool isDarkTheme = ThemeManger::isDarkTheme();
    // tool button disable olma durumunda otomatik renk değişikliği için
    if (isDarkTheme) {
        QIcon icon;
        icon.addPixmap(QPixmap(":/resources/img/arrow_circle_left_white.svg"), QIcon::Normal, QIcon::Off);
        icon.addPixmap(QPixmap(":/resources/img/arrow_circle_left_gray.svg"), QIcon::Disabled, QIcon::Off);
        backTabButton->setIcon(icon);

        icon.addPixmap(QPixmap(":/resources/img/arrow_circle_right_white.svg"), QIcon::Normal, QIcon::Off);
        icon.addPixmap(QPixmap(":/resources/img/arrow_circle_right_gray.svg"), QIcon::Disabled, QIcon::Off);
        forwTabButton->setIcon(icon);

        icon.addPixmap(QPixmap(":/resources/img/arrow_circle_up_white.svg"), QIcon::Normal, QIcon::Off);
        icon.addPixmap(QPixmap(":/resources/img/arrow_circle_up_gray.svg"), QIcon::Disabled, QIcon::Off);
        upTabButton->setIcon(icon);
    } else {
        QIcon icon;
        icon.addPixmap(QPixmap(":/resources/img/arrow_circle_left_black.svg"), QIcon::Normal, QIcon::Off);
        icon.addPixmap(QPixmap(":/resources/img/arrow_circle_left_gray.svg"), QIcon::Disabled, QIcon::Off);
        backTabButton->setIcon(icon);

        icon.addPixmap(QPixmap(":/resources/img/arrow_circle_right_black.svg"), QIcon::Normal, QIcon::Off);
        icon.addPixmap(QPixmap(":/resources/img/arrow_circle_right_gray.svg"), QIcon::Disabled, QIcon::Off);
        forwTabButton->setIcon(icon);

        icon.addPixmap(QPixmap(":/resources/img/arrow_circle_up_black.svg"), QIcon::Normal, QIcon::Off);
        icon.addPixmap(QPixmap(":/resources/img/arrow_circle_up_gray.svg"), QIcon::Disabled, QIcon::Off);
        upTabButton->setIcon(icon);
    }
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
