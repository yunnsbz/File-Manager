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

TabManager::TabManager(QTabWidget* tabWidget, QTreeView* treeView, QTableView* tableView, QObject* parent)
    :
    QObject(parent),
    m_mainWindow_(static_cast<MainWindow*>(parent)),
    m_tabWidget_(tabWidget),
    m_fileModelOp(new FileModelOperations()),
    m_tableManager(new TableManager(tabWidget, tableView, m_fileModelOp, this)),
    m_treeManager(new TreeManager(treeView, m_fileModelOp, tabWidget, this))
{
    // sekmelerin sürüklenmesi/yer değiştirmesi hareketlerini algılamak için:
    connect(tabWidget->tabBar(), &QTabBar::tabMoved, this, &TabManager::onTabMoved);
    connect(tabWidget->tabBar(), &QTabBar::tabBarClicked, this, &TabManager::onTabBarClicked);
    connect(tabWidget->tabBar(), &QTabBar::tabCloseRequested, this, &TabManager::onTabClosed);

    // tree view'deki değişiklikleri al:
    connect(m_treeManager, &TreeManager::treeViewClicked, this, [this](const QModelIndex &modelIndex){
        auto* fileModel = getFileModelOp()->getFileModel();
        // girilen yer klasör ise table view set edilir:
        if (fileModel->hasChildren(modelIndex))
        {
            m_tableManager->setRootIndex(modelIndex);
        }
        const int tabIndex = m_tabWidget_->currentIndex();
        const QString path  = getFileModelOp()->getCurrentPath(tabIndex);
        emit tabChanged(tabIndex, path);
    });

    connect(m_tableManager, &TableManager::tableDoubleClicked, this, [this](const QModelIndex &modelIndex){
        const auto firstColumnIndex = modelIndex.siblingAtColumn(0); // her zaman ilk sütunu al

        m_treeManager->expandTreeView(firstColumnIndex);

        const int tabIndex = m_tabWidget_->currentIndex();
        const QString path  = getFileModelOp()->getCurrentPath(tabIndex);
        emit tabChanged(tabIndex, path);
    });


    // should change the first tab default name
    tabWidget->tabBar()->setTabText(0,"new tab");

    setCornerNavButtons();

    setAddTabButton();

    tabWidget->installEventFilter(this);

    // corner nav button'ları en başta root directory'de olduklarından disable edilir
    updateNavButtons(false, false, false);
}

void TabManager::setAddTabButton()
{
    auto* addTabButton = new QToolButton();
    addTabButton->setObjectName("add-tab-button");
    addTabButton->setText("+");
    m_tabWidget_->setCornerWidget(addTabButton, Qt::TopLeftCorner);
    enableNavWidget(true);

    // add button onClick:
    connect(addTabButton, &QToolButton::clicked, this, &TabManager::addNewTab);
}

void TabManager::enableNavWidget(bool enable)
{
     if(enable){
        m_cornerNavButtons->show();
        m_tabWidget_->setCornerWidget(m_cornerNavButtons, Qt::TopRightCorner);
    } else {
        m_tabWidget_->setCornerWidget(nullptr, Qt::TopRightCorner);
    }
}

void TabManager::updateNavButtons(bool BackEnable, bool ForwardEnable, bool UpEnabled)
{
    m_backTabButton->setEnabled(BackEnable);
    m_forwTabButton->setEnabled(ForwardEnable);
    m_upTabButton->setEnabled(UpEnabled);
}

void TabManager::setCornerNavButtons()
{
    m_cornerNavButtons = new QWidget;
    m_cornerNavButtons->setObjectName("cornerNavButtons");
    auto *layout = new QHBoxLayout(m_cornerNavButtons);
    layout->setContentsMargins(0, 0, 0, 0);

    m_backTabButton = new QToolButton();
    m_forwTabButton = new QToolButton();
    m_upTabButton = new QToolButton();

    setNavButtonThemes();

    // adding nav buttons to a parent widget
    layout->addWidget(m_backTabButton);
    layout->addWidget(m_forwTabButton);
    layout->addWidget(m_upTabButton);

    connect(m_upTabButton, &QToolButton::clicked, this, [this]{m_mainWindow_->upperFolderOnClick();});
    connect(m_forwTabButton, &QToolButton::clicked, this, [this]{m_mainWindow_->forwardButtonOnClick();});
    connect(m_backTabButton, &QToolButton::clicked, this, [this]{m_mainWindow_->backButtonOnClick();});
}

void TabManager::setNavButtonThemes()
{
    const bool isDarkTheme = ThemeManger::isDarkTheme();
    // tool button disable olma durumunda otomatik renk değişikliği için
    if (isDarkTheme) {
        QIcon icon;
        icon.addPixmap(QPixmap(":/resources/img/arrow_circle_left_white.svg"), QIcon::Normal, QIcon::Off);
        icon.addPixmap(QPixmap(":/resources/img/arrow_circle_left_gray.svg"), QIcon::Disabled, QIcon::Off);
        m_backTabButton->setIcon(icon);

        icon.addPixmap(QPixmap(":/resources/img/arrow_circle_right_white.svg"), QIcon::Normal, QIcon::Off);
        icon.addPixmap(QPixmap(":/resources/img/arrow_circle_right_gray.svg"), QIcon::Disabled, QIcon::Off);
        m_forwTabButton->setIcon(icon);

        icon.addPixmap(QPixmap(":/resources/img/arrow_circle_up_white.svg"), QIcon::Normal, QIcon::Off);
        icon.addPixmap(QPixmap(":/resources/img/arrow_circle_up_gray.svg"), QIcon::Disabled, QIcon::Off);
        m_upTabButton->setIcon(icon);
    } else {
        QIcon icon;
        icon.addPixmap(QPixmap(":/resources/img/arrow_circle_left_black.svg"), QIcon::Normal, QIcon::Off);
        icon.addPixmap(QPixmap(":/resources/img/arrow_circle_left_gray.svg"), QIcon::Disabled, QIcon::Off);
        m_backTabButton->setIcon(icon);

        icon.addPixmap(QPixmap(":/resources/img/arrow_circle_right_black.svg"), QIcon::Normal, QIcon::Off);
        icon.addPixmap(QPixmap(":/resources/img/arrow_circle_right_gray.svg"), QIcon::Disabled, QIcon::Off);
        m_forwTabButton->setIcon(icon);

        icon.addPixmap(QPixmap(":/resources/img/arrow_circle_up_black.svg"), QIcon::Normal, QIcon::Off);
        icon.addPixmap(QPixmap(":/resources/img/arrow_circle_up_gray.svg"), QIcon::Disabled, QIcon::Off);
        m_upTabButton->setIcon(icon);
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

auto TabManager::getPreviousSplitter() const -> QSplitter*
{
    return m_tabWidget_->widget(m_previousLeftTabIndex)->findChild<QSplitter*>();
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
    auto* currentSplitter = getPreviousSplitter();

    if (currentSplitter == nullptr)
    {
        return;
    }

    auto* newTabWidget = new QWidget();
    auto* layout = new QVBoxLayout(newTabWidget);
    layout->addWidget(currentSplitter);
    layout->setContentsMargins(0, 0, 0, 0);

    m_tabWidget_->addTab(newTabWidget, "new tab");
    m_tabWidget_->setCurrentIndex(m_tabWidget_->count() - 1);

    setPreviousLeftTabIndex(m_tabWidget_->count() - 1);

    const int tabIndex = m_tabWidget_->currentIndex();

    setTabContent(tabIndex);

    const QString path  = getFileModelOp()->getCurrentPath(tabIndex);
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
        setTabContent(tabIndex);
    }
    const QString path  = getFileModelOp()->getCurrentPath(tabIndex);
    emit tabChanged(tabIndex, path);
}

void TabManager::onTabClosed(int index)
{
    if (m_tabWidget_->count() > 1)
    {
        // move widget before closing current tab:
        if (m_tabWidget_->currentIndex() == index)
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

        m_tabWidget_->removeTab(index);

        m_treeManager->removeTabExpandedPaths(index);
        m_fileModelOp->removeTabModelIndex(index);

        setPreviousLeftTabIndex(m_tabWidget_->currentIndex());
        setTabContent(m_tabWidget_->currentIndex());

        const int newTabIndex = m_tabWidget_->currentIndex();
        const QString path  = getFileModelOp()->getCurrentPath(newTabIndex);
        emit tabChanged(newTabIndex, path);
    }
    else
    {
        m_treeManager->setTreeToDefault();
        m_tableManager->setTableToDefault();

        // navigasyon sıfırlaması
        m_fileModelOp->removeTabModelIndex(index);

        const int newTabIndex = m_tabWidget_->currentIndex();
        const QString path  = getFileModelOp()->getCurrentPath(newTabIndex);
        emit tabChanged(newTabIndex, path);
    }
}

void TabManager::moveTabWidget(int index)
{
    // sekme içerisini kopyalamak yerine taşıyarak sekmeler arasında geçiş yaparız

    auto* currentSplitter = getPreviousSplitter();

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
    m_tabWidget_->insertTab(index, newTabWidget, m_tabWidget_->tabText(index));
    m_tabWidget_->removeTab(index + 1); // eski widget'ı kaldır
    m_tabWidget_->setCurrentIndex(index);

    setPreviousLeftTabIndex(index);
}

// sekme içerisindeki view'ların en son hangi dosya açıksa onu tekrar açması
void TabManager::setTabContent(int tabIndex)
{
    m_tableManager->setTableContent(tabIndex);
    m_treeManager->setTreeContent(tabIndex);
}

FM_END_NAMESPACE
