#include "TabManager.h"
#include "FileModelOperations.h"
#include "TreeManager.h"
#include "mainwindow.hpp"
#include "ToolBarManager.h"
#include "ThemeManager.h"
#include "TableManager.h"

#include <QToolButton>
#include <QTabBar>
#include <QBoxLayout>

FM_BEGIN_NAMESPACE

TabManager::TabManager(QTabWidget* tabWidget, QTreeView* treeView, QTableView* tableView, bool forRightPane, QObject* parent)
    :
    QObject(parent),
    mainWindow_(static_cast<MainWindow*>(parent)),
    tabWidget_(tabWidget),
    m_fileModelOp(new FileModelOperations()),
    m_tableManager(new TableManager(tabWidget, tableView, m_fileModelOp, this)),
    m_treeManager(new TreeManager(treeView, m_fileModelOp, tabWidget, this)),
    m_forRightPane(forRightPane)
{
    // sekmelerin sürüklenmesi/yer değiştirmesi hareketlerini algılamak için:
    connect(tabWidget->tabBar(), &QTabBar::tabMoved, this, &TabManager::onTabMoved);
    connect(tabWidget->tabBar(), &QTabBar::tabBarClicked, this, &TabManager::onTabBarClicked);
    connect(tabWidget->tabBar(), &QTabBar::tabCloseRequested, this, &TabManager::onTabClosed);

    // tree view'deki değişiklikleri al:
    connect(m_treeManager, &TreeManager::treeViewClicked, this, [this](const QModelIndex &modelIndex){
        auto* fileModel = getFileModelOp()->GetFileModel();
        // girilen yer klasör ise table view set edilir:
        if (fileModel->hasChildren(modelIndex))
        {
            m_tableManager->setRootIndex(modelIndex);
        }
        const int tabIndex = tabWidget_->currentIndex();
        const QString path  = getFileModelOp()->GetCurrentPath(tabIndex);
        emit tabChanged(tabIndex, path);
    });

    connect(m_tableManager, &TableManager::tableDoubleClicked, this, [this](const QModelIndex &modelIndex){
        const auto firstColumnIndex = modelIndex.siblingAtColumn(0); // her zaman ilk sütunu al

        m_treeManager->ExpandTreeView(firstColumnIndex);

        const int tabIndex = tabWidget_->currentIndex();
        const QString path  = getFileModelOp()->GetCurrentPath(tabIndex);
        emit tabChanged(tabIndex, path);
    });


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
    } else {
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

    m_treeManager->swapExpandedPathsMap(toIndex, fromIndex);
    getFileModelOp()->swapTabModelIndexMap(toIndex, fromIndex);
    getFileModelOp()->swapTabHistoryModelIndex(toIndex, fromIndex);

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

    const int tabIndex = tabWidget_->currentIndex();

    SetTabContent(tabIndex);

    const QString path  = getFileModelOp()->GetCurrentPath(tabIndex);
    emit tabChanged(tabIndex, path);
}

void TabManager::onTabBarClicked(int tabIndex)
{
    // Aynı sekmeye tıklanmadıysa
    if (tabIndex != _getPreviousLeftTabIndex()
        &&
        tabIndex != -1)
    {
        moveTabWidget(tabIndex);
        SetTabContent(tabIndex);
    }
    const QString path  = getFileModelOp()->GetCurrentPath(tabIndex);
    emit tabChanged(tabIndex, path);
}

void TabManager::onTabClosed(int index)
{
    if (tabWidget_->count() > 1)
    {
        // move widget before closing current tab:
        if (tabWidget_->currentIndex() == index)
        {
            if (index >= 1)
            {
                moveTabWidget(index - 1);
            }
            else
            {
                moveTabWidget(index + 1);
            }
        }

        tabWidget_->removeTab(index);

        m_treeManager->removeTabExpandedPaths(index);
        m_fileModelOp->RemoveTabModelIndex(index);

        setPreviousLeftTabIndex(tabWidget_->currentIndex());
        SetTabContent(tabWidget_->currentIndex());

        const int newTabIndex = tabWidget_->currentIndex();
        const QString path  = getFileModelOp()->GetCurrentPath(newTabIndex);
        emit tabChanged(newTabIndex, path);
    }
    else
    {
        m_treeManager->setTreeToDefault();
        m_tableManager->SetTableToDefault();

        // navigasyon sıfırlaması
        m_fileModelOp->RemoveTabModelIndex(index);

        const int newTabIndex = tabWidget_->currentIndex();
        const QString path  = getFileModelOp()->GetCurrentPath(newTabIndex);
        emit tabChanged(newTabIndex, path);
    }
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

// sekme içerisindeki view'ların en son hangi dosya açıksa onu tekrar açması
void TabManager::SetTabContent(int tabIndex)
{
    m_tableManager->SetTableContent(tabIndex);
    m_treeManager->SetTreeContent(tabIndex);
}

FM_END_NAMESPACE
