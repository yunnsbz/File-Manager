#include "mainwindow.hpp"
#include "EventHandler.h"
#include "FileModelOperations.h"
#include "FileOperationManager.h"
#include "FileOperationView.h"
#include "MenuBarView.h"
#include "SettingsDialog.h"
#include "ThemeManager.h"
#include "TabManager.h"
#include "ToolBarManager.h"
#include "ApplicationStateHandler.h"

#include <QAbstractButton>
#include <QAbstractItemView>
#include <QDesktopServices>
#include <QToolButton>
#include <QMouseEvent>
#include <QDialog>
#include <QDialogButtonBox>
#include <QProcess>
#include <QScrollBar>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QItemSelectionModel>
#include <QAbstractItemModel>
#include <QSplitterHandle>
#include <QVBoxLayout>
#include <QAbstractAnimation>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>
#include <QDir>

FM_BEGIN_NAMESPACE

MainWindow::UIManager::UIManager(Ui::MainWindow*& theUi, QMainWindow* pWnd)
{
    theUi->setupUi(pWnd);
}

MainWindow::MainWindow(QWidget* parent)
    :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_ui_mgr_(ui, this),
    //m_columnFileModel(m_fileModelOpLeft->GetFileModel()),
    m_toolBarManager(new ToolBarManager(ui->toolBar, this)),
    m_menuManager(new ThemeManger(this)),
    m_tabManagerLeft(new TabManager(ui->tabWidgetLeft, ui->fileTreeViewLeft, ui->tableViewLeft, false, this)),
    m_tabManagerRight(new TabManager(ui->tabWidgetRight, ui->fileTreeViewRight, ui->tableViewRight, true, this)),
    m_fileOpManager(new FileOperationManager(this)),
    m_appStateHandler(new ApplicationStateHandler(this)),
    m_settingsDialog(new SettingsDialog(this)),
    m_fileOperationView(new FileOperationView(this)),
    m_menuBarView(new MenuBarView(this)),
    m_eventHandler(new EventHandler(this, ui->tabWidgetLeft, ui->tabWidgetRight))
{
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);

    ui->columnView->setModel(m_columnFileModel);
    ui->columnView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // tree view daha küçük olmalı
    ui->splitterLeft->setSizes({100,400});
    ui->splitterRight->setSizes({100,400});

    // spliterlar ekranda sadece tree view olacak şekilde sürüklenmemeli
    ui->splitterLeft->setCollapsible(1,false);
    ui->splitterRight->setCollapsible(1,false);

    //remove search line edit for now
    ui->lineEdit_2->setMaximumWidth(0);

    setWindowTitle("File Manager");
    show();

    // load and restore view:
    m_appStateHandler->RestoreViewState();

    // get FileTreeView changes for command label
    connect(
        getTreeViewLeft()->selectionModel(),
        &QItemSelectionModel::currentChanged,
        this,
        &MainWindow::onTreeLeftSelectionChanged
    );
    connect(
        getTreeViewRight()->selectionModel(),
        &QItemSelectionModel::currentChanged,
        this,
        &MainWindow::onTreeRightSelectionChanged
    );

    // tabWidget focus control
    connect(m_eventHandler, &EventHandler::tabLeftClicked, this, [this](){
        m_isWorkingOnLeftPane = true;
        m_isWorkingOnRightPane = false;
        const int tabIndex = getTabWidgetLeft()->currentIndex();
        updateNavButtons(tabIndex);
        SetLabelText_(m_tabManagerLeft->getFileModelOp()->GetCurrentPath(tabIndex));
    });
    connect(m_eventHandler, &EventHandler::tabRightClicked, this, [this](){
        m_isWorkingOnLeftPane = false;
        m_isWorkingOnRightPane = true;
        const int tabIndex = getTabWidgetRight()->currentIndex();
        updateNavButtons(tabIndex);
        SetLabelText_(m_tabManagerRight->getFileModelOp()->GetCurrentPath(tabIndex));
    });

    // command line focus handling
    connect(m_eventHandler, &EventHandler::keyEvent_C, this, [this](){
        ui->lineEdit->setFocus();
    });

    connect(m_tabManagerLeft, &TabManager::tabChanged, this, [this](int tabIndex, QString filePath){
        updateNavButtons(tabIndex);
        SetLabelText_(filePath);
    });

    connect(m_tabManagerRight, &TabManager::tabChanged, this, [this](int tabIndex, QString filePath){
        updateNavButtons(tabIndex);
        SetLabelText_(filePath);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

auto MainWindow::getUI() -> Ui::MainWindow*
{
    return ui;
}

void MainWindow::SetLabelText_(QString path)
{
    if(path.isEmpty())
        path = "\\\\";

    ui->label->setText(path);

    // label default size (in the ui editor) should be bigger than needed
    ui->label->setMinimumSize(ui->label->sizeHint());
}

void MainWindow::onTreeLeftSelectionChanged(const QModelIndex& current, const QModelIndex&)
{
    auto path = static_cast<QFileSystemModel*>(getTreeViewLeft()->model())->filePath(current);

    SetLabelText_(path);
}

void MainWindow::onTreeRightSelectionChanged(const QModelIndex &current, const QModelIndex &)
{
    auto path = static_cast<QFileSystemModel*>(getTreeViewRight()->model())->filePath(current);

    SetLabelText_(path);
}

void MainWindow::on_splitterLeft_splitterMoved(int pos, int )
{
    // TODO(yunus): this doesn't work for right pane
    if (pos < 10 && m_treeViewActive )
    {
        m_treeViewActive = false;
    }
    else if (pos > 10 && ! m_treeViewActive)
    {
        m_treeViewActive = true;
    }
}

void MainWindow::DeactivateTreeView()
{
    m_treeViewActive = false;
    ui->splitterLeft->setSizes({0,400});
    ui->splitterRight->setSizes({0,400});

    // sürüklemeyi devre dışı bırakma:
    for (int i = 0; i < 2; ++i)
    {
        QSplitterHandle* handle = ui->splitterLeft->handle(i);
        QSplitterHandle* handle2 = ui->splitterRight->handle(i);
        if (handle != nullptr && handle2 != nullptr)
        {
            handle->setEnabled(false);
            handle2->setEnabled(false);
        }
    }

    ui->splitterLeft->setHandleWidth(0);
    ui->splitterRight->setHandleWidth(0);
}

void MainWindow::ActivateTreeView()
{
    m_treeViewActive = true;
    ui->splitterLeft->setSizes({100,400});
    ui->splitterRight->setSizes({100,400});
    //  ilk view'ın sağ kenarı ve ikinci view'ın sol kenarı olmak üzere iki handle olur:
    for (int i = 0; i < 2; ++i)
    {
        QSplitterHandle* handle = ui->splitterLeft->handle(i);
        QSplitterHandle* handle2 = ui->splitterRight->handle(i);
        if (handle != nullptr && handle2 != nullptr)
        {
            handle->setEnabled(true);
            handle2->setEnabled(true);
        }
    }

    ui->splitterLeft->setHandleWidth(5);
    ui->splitterRight->setHandleWidth(5);
}

void MainWindow::updateNavButtons(const int tabIndex)
{
    if(isWorkingOnRightPane())
    {
        auto fileModelOp = m_tabManagerRight->getFileModelOp();

        const bool backButtonEnable = !fileModelOp->IsBackHistoryEmpty(tabIndex);
        const bool forwardButtonEnable = !fileModelOp->IsForwardHistoryEmpty(tabIndex);

        m_toolBarManager->SetBackButtonEnabled(backButtonEnable);
        m_toolBarManager->SetForwardButtonEnabled(forwardButtonEnable);

        const QModelIndex currentIndex = fileModelOp->GetTabModelIndex(tabIndex);
        const QString currentPath = fileModelOp->GetFilePath(currentIndex);

        // Eğer current path boş ise daha yukarı çıkılamıyordur.
        const bool upEnable = (currentPath != "");

        m_toolBarManager->SetUpButtonEnabled(upEnable);

        m_tabManagerRight->updateNavButtons(backButtonEnable, forwardButtonEnable, upEnable);
    }
    else
    {
        auto fileModelOp = m_tabManagerLeft->getFileModelOp();

        const bool backButtonEnable = !fileModelOp->IsBackHistoryEmpty(tabIndex);
        const bool forwardButtonEnable = !fileModelOp->IsForwardHistoryEmpty(tabIndex);

        m_toolBarManager->SetBackButtonEnabled(backButtonEnable);
        m_toolBarManager->SetForwardButtonEnabled(forwardButtonEnable);

        const QModelIndex currentIndex = fileModelOp->GetTabModelIndex(tabIndex);
        const QString currentPath = fileModelOp->GetFilePath(currentIndex);

        // Eğer current path boş ise daha yukarı çıkılamıyordur.
        const bool upEnable = (currentPath != "");

        m_toolBarManager->SetUpButtonEnabled(upEnable);

        m_tabManagerLeft->updateNavButtons(backButtonEnable, forwardButtonEnable, upEnable);
    }
}

void MainWindow::upperFolderOnClick(bool OnRightPane)
{
    if(OnRightPane)
    {
        auto fileModelOp = m_tabManagerRight->getFileModelOp();

        const auto tabIndex = getTabWidgetRight()->currentIndex();

        auto parentIndex = fileModelOp->GetFileModel()->parent(fileModelOp->GetTabModelIndex(tabIndex));

        fileModelOp->SetTabModelIndex(tabIndex, parentIndex);
        //TODO(yunus): tree back onClick missing

        m_tabManagerRight->SetTabContent(tabIndex);

        // buton kontrolü:
        updateNavButtons(tabIndex);
    }
    else
    {
        auto fileModelOp = m_tabManagerLeft->getFileModelOp();

        const auto tabIndex = getTabWidgetLeft()->currentIndex();

        auto parentIndex = fileModelOp->GetFileModel()->parent(fileModelOp->GetTabModelIndex(tabIndex));

        fileModelOp->SetTabModelIndex(tabIndex, parentIndex);
        //TODO(yunus): tree back onClick missing

        m_tabManagerLeft->SetTabContent(tabIndex);

        // buton kontrolü:
        updateNavButtons(tabIndex);
    }
}

void MainWindow::BackButtonOnClick(bool OnRightPane)
{
    if(OnRightPane){
        const auto tabIndex = getTabWidgetRight()->currentIndex();

        m_tabManagerRight->getFileModelOp()->OnBackButtonClicked(tabIndex);
        //TODO(yunus): tree back onClick missing

        m_tabManagerRight->SetTabContent(tabIndex);

        // buton kontrolü:
        updateNavButtons(tabIndex);
    }
    else if(m_columnViewActive){
        ScrollColumn(-1);
    }
    else{
        const auto tabIndex = getTabWidgetLeft()->currentIndex();

        m_tabManagerLeft->getFileModelOp()->OnBackButtonClicked(tabIndex);
        //TODO(yunus): tree back onClick missing

        m_tabManagerLeft->SetTabContent(tabIndex);

        // buton kontrolü:
        updateNavButtons(tabIndex);
    }
}

void MainWindow::ScrollColumn(int direction)
{
    const QList<QAbstractItemView*> columns = ui->columnView->findChildren<QAbstractItemView*>();

    int scrollAmount = 0;
    for (auto *col : columns) {
        const int xPoint = ui->columnView->viewport()->mapFromGlobal(col->mapToGlobal(QPoint(0, 0))).x();
        if ((direction > 0 && xPoint + col->width() > 0) || (direction < 0 && xPoint < ui->columnView->viewport()->width())) {
            scrollAmount = col->width();
            break;
        }
    }

    ui->columnView->horizontalScrollBar()->setValue(
                ui->columnView->horizontalScrollBar()->value() + (direction * scrollAmount)
                );
}

void MainWindow::ForwardButtonOnClick(bool OnRightPane)
{
    if(OnRightPane){
        const auto tabIndex = getTabWidgetRight()->currentIndex();

        m_tabManagerRight->getFileModelOp()->OnForwardButtonClicked(tabIndex);
        //tree back onClick missing

        m_tabManagerRight->SetTabContent(tabIndex);

        // buton kontrolü:
        updateNavButtons(tabIndex);
    }
    else if(m_columnViewActive){
        ScrollColumn(1);
    }
    else{
        const auto tabIndex = getTabWidgetLeft()->currentIndex();

        m_tabManagerLeft->getFileModelOp()->OnForwardButtonClicked(tabIndex);
        //tree back onClick missing

        m_tabManagerLeft->SetTabContent(tabIndex);

        // buton kontrolü:
        updateNavButtons(tabIndex);
    }
}

void MainWindow::on_toolUpButton_clicked()
{
    upperFolderOnClick(isWorkingOnRightPane());
}
void MainWindow::on_toolBackButton_clicked()
{
    BackButtonOnClick(isWorkingOnRightPane());
}
void MainWindow::on_toolForwardButton_clicked()
{
    ForwardButtonOnClick(isWorkingOnRightPane());
}


void MainWindow::on_lineEdit_returnPressed()
{
    auto fileModelOp = m_tabManagerLeft->getFileModelOp();

    auto path = fileModelOp->GetFilePath(fileModelOp->GetTabModelIndex(getTabWidgetRight()->currentIndex()));

    if (path.isEmpty())
    {
        path = "\\";
    }

    path = QDir::toNativeSeparators(path);

    const auto args = QStringList() << "/c " << "start cd " << path;

    QString str = "";
    for (int i{}; i < args.size(); ++i)
    {
        str += args[i];
    }

    QProcess::startDetached("cmd", QStringList() << str);
}

void MainWindow::on_toolCmdButton_pressed()
{
    on_lineEdit_returnPressed();
}


void MainWindow::ActivateDualPane()
{
    m_dualPaneActive = true;
    ui->splitter_dualPane->setChildrenCollapsible(true);
    ui->splitter_dualPane->setSizes({1,1});
    ui->splitter_dualPane->setChildrenCollapsible(false);
    //  ilk view'ın sağ kenarı ve ikinci view'ın sol kenarı olmak üzere iki handle olur:
    for (int i = 0; i < 2; ++i)
    {
        QSplitterHandle* handle = ui->splitter_dualPane->handle(i);
        if (handle != nullptr)
        {
            handle->setEnabled(true);
        }
    }

    ui->splitter_dualPane->setHandleWidth(5);

    // dual pane geldiğinde tablo sütun genişliklerini uygun hale getir
    //TODO(yunus): table resize implementasyonunu güncelle
    //m_tableManagerLeft->SetColumnResize();

    m_tabManagerLeft->EnableNavWidget(true);
    m_tabManagerRight->EnableNavWidget(true);
}

void MainWindow::DeactivateDualPane()
{
    m_dualPaneActive = false;
    ui->splitter_dualPane->setChildrenCollapsible(true);
    ui->splitter_dualPane->setSizes({1,0});
    ui->splitter_dualPane->setChildrenCollapsible(false);

    // disable middle splitter:
    for (int i = 0; i < 2; ++i)
    {
        QSplitterHandle* handle = ui->splitter_dualPane->handle(i);
        if (handle != nullptr)
        {
            handle->setEnabled(false);
        }
    }
    ui->splitter_dualPane->setHandleWidth(0);

    //TODO(yunus): table resize implementasyonunu güncelle
    //m_tableManagerLeft->SetColumnResize();

    m_tabManagerLeft->EnableNavWidget(false);
    m_tabManagerRight->EnableNavWidget(false);
}

void MainWindow::DeactivateColumnView()
{
    m_columnViewActive = false;
    getStackedWidget()->setCurrentIndex(0);
}

auto MainWindow::getFileModelOpLeft() const ->FileModelOperations*
{
    return m_tabManagerLeft->getFileModelOp();
}

auto MainWindow::getFileModelOpRight() const ->FileModelOperations*
{
    return m_tabManagerRight->getFileModelOp();
}

void MainWindow::ActivateColumnView()
{
    m_columnViewActive = true;
    getStackedWidget()->setCurrentIndex(1);
    m_toolBarManager->SetBackButtonEnabled(true);
    m_toolBarManager->SetForwardButtonEnabled(true);
}

void MainWindow::on_toolSearchButton_clicked()
{
    if(m_searchOn){
        if (ui->lineEdit_2 != nullptr) {
            m_searchOn = false;
            auto *animation = new QPropertyAnimation(ui->lineEdit_2, "maximumWidth");
            animation->setDuration(300);
            animation->setStartValue(ui->lineEdit_2->width());
            animation->setEndValue(0); // önceki boyut
            animation->start(QAbstractAnimation::DeleteWhenStopped);
        }
    }
    else{
        if (ui->lineEdit_2 != nullptr) {
            m_searchOn = true;
            auto *animation = new QPropertyAnimation(ui->lineEdit_2, "maximumWidth");
            animation->setDuration(300);
            animation->setStartValue(ui->lineEdit_2->width());
            animation->setEndValue(250); // hedef genişlik
            animation->start(QAbstractAnimation::DeleteWhenStopped);

            ui->lineEdit_2->setFocus();
        }
    }
}

void MainWindow::on_columnView_clicked(const QModelIndex &index)
{
    if (!m_columnFileModel->hasChildren(index))
    {
        const QString filePath = m_columnFileModel->filePath(index);
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }
}

void MainWindow::on_toolHistoryButton_clicked()
{
    auto* popupFrame = new QFrame(this, Qt::Popup); // Menü gibi görünür
    auto* layout = new QVBoxLayout(popupFrame);
    popupFrame->setObjectName("HistoryPopupFrame");

    auto* scrollArea = new QScrollArea(popupFrame);
    scrollArea->setWidgetResizable(true);

    auto* scrollContent = new QWidget();
    auto* contentLayout = new QVBoxLayout(scrollContent);
    scrollContent->setObjectName("HistoryScrollContent");
    scrollContent->setAttribute(Qt::WA_Hover, true);

    contentLayout->setContentsMargins(1,2,1,2);
    contentLayout->setSpacing(0);

    for (int i = 0; i < 20; i++) {
        auto* itemWidget = new QWidget;
        auto* rowLayout = new QHBoxLayout(itemWidget);

        rowLayout->setContentsMargins(1, 2, 1, 2); // min dikey boşluk
        rowLayout->setSpacing(0);

        auto* label = new QLabel("operation description (... moved to ...)");
        auto* undoButton = new QToolButton();
        undoButton->setIcon(QIcon(":/resources/img/history_white.svg"));

        rowLayout->addWidget(label);
        rowLayout->addWidget(undoButton);
        itemWidget->setLayout(rowLayout);
        itemWidget->setObjectName("HistoryItem");

        contentLayout->addWidget(itemWidget);
    }

    scrollContent->setLayout(contentLayout);
    scrollArea->setWidget(scrollContent);

    layout->addWidget(scrollArea);
    popupFrame->setLayout(layout);
    popupFrame->resize(300, 400); // Max boyut
    popupFrame->move(ui->toolHistoryButton->mapToGlobal(QPoint(-270, ui->toolHistoryButton->height())));
    popupFrame->show();
}

FM_END_NAMESPACE
