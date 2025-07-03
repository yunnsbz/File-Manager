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
    mainWindow_(static_cast<MainWindow*>(parent)),
    tabWidget_(tabWidget),
    m_forRightPane(forRightPane)
{
    // sekmelerin sürüklenmesi/yer değiştirmesi hareketlerini algılamak için:
    connect(tabWidget->tabBar(), &QTabBar::tabMoved, this, &TabManager::onTabMoved);

    // should change the first tab default name
    tabWidget->tabBar()->setTabText(0,"new tab");

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
    tabWidget_->setCornerWidget(addTabButton, Qt::TopLeftCorner);
    EnableNavWidget(true);

    // add button onClick:
    connect(addTabButton, &QToolButton::clicked, this, &TabManager::addNewTab);
}

void TabManager::EnableNavWidget(bool enable)
{
     if(enable){
        cornerNavButtons->show();
        tabWidget_->setCornerWidget(cornerNavButtons, Qt::TopRightCorner);
    }
    else{
        tabWidget_->setCornerWidget(nullptr, Qt::TopRightCorner);
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

    connect(upTabButton, &QToolButton::clicked, this, [this]{mainWindow_->upperFolderOnClick(m_forRightPane);});
    connect(forwTabButton, &QToolButton::clicked, this, [this]{mainWindow_->ForwardButtonOnClick(m_forRightPane);});
    connect(backTabButton, &QToolButton::clicked, this, [this]{mainWindow_->BackButtonOnClick(m_forRightPane);});
}

void TabManager::SetNavButtonThemes()
{
    const bool isDarkTheme = ThemeManger::isDarkTheme();
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

    if (m_forRightPane)
    {
        mainWindow_->OnTabMoved2(toIndex, fromIndex);
    }
    else
    {
        mainWindow_->OnTabMoved(toIndex, fromIndex);
    }

    // if lastLeftTabIndex is the one moved it should register
    if (fromIndex == m_previousLeftTabIndex)
    {
        m_previousLeftTabIndex = toIndex;
        m_persistentPreviousLeftTabIndex = toIndex;
    }
    else if (toIndex == m_lastRightTabIndex)
    {
        m_previousLeftTabIndex = fromIndex;
        m_persistentPreviousLeftTabIndex = fromIndex;
    }
}

auto TabManager::GetPreviousSplitter()
{
    return tabWidget_->widget(m_previousLeftTabIndex)->findChild<QSplitter*>();
}

auto TabManager::_getPreviousLeftTabIndex() const -> int
{
    return m_previousLeftTabIndex;
}

auto TabManager::getPersistentPreviousLeftTabIndex() const ->int
{
    return m_persistentPreviousLeftTabIndex;
}

void TabManager::setPreviousLeftTabIndex(int value)
{
    m_persistentPreviousLeftTabIndex = m_previousLeftTabIndex;
    m_previousLeftTabIndex = value;
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

    tabWidget_->addTab(newTabWidget, "new tab");
    tabWidget_->setCurrentIndex(tabWidget_->count() - 1);

    setPreviousLeftTabIndex(tabWidget_->count() - 1);

    mainWindow_->SetTabContent(tabWidget_->currentIndex(), m_forRightPane);
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
    tabWidget_->insertTab(index, newTabWidget, tabWidget_->tabText(index));
    tabWidget_->removeTab(index + 1); // eski widget'ı kaldır
    tabWidget_->setCurrentIndex(index);

    setPreviousLeftTabIndex(index);
}
