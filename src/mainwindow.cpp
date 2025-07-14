#include "mainwindow.hpp"
#include "FileOperationView.h"
#include "MenuBarView.h"
#include "SettingsDialog.h"
#include "ThemeManager.h"
#include "TabManager.h"
#include "TableManager.h"
#include "ToolBarManager.h"
#include "FileModelOperations.h"
#include "TreeManager.h"
#include "ApplicationStateHandler.h"

#include <QAbstractButton>
#include <QDesktopServices>
#include <QToolButton>
#include <QMouseEvent>
#include <QDialog>
#include <QDialogButtonBox>
#include <QProcess>
#include <QScrollBar>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QMessageBox>
#include <QItemSelectionModel>
#include <QAbstractItemModel>
#include <QSplitterHandle>
#include <QVBoxLayout>
#include <QAbstractAnimation>
#include <QFrame>
#include <QHBoxLayout>
#include <QIcon>

MainWindow::UIManager::UIManager(Ui::MainWindow*& theUi, QMainWindow* pWnd)
{
    theUi->setupUi(pWnd);
}

MainWindow::MainWindow(QWidget* parent)
    :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_ui_mgr_(ui, this),
    m_fileModelOpLeft(new FileModelOperations()),
    m_fileModelOpRight(new FileModelOperations()),
    m_columnFileModel(m_fileModelOpLeft->GetFileModel()),
    m_toolBarManager(new ToolBarManager(ui->toolBar, this)),
    m_menuManager(new ThemeManger(this)),
    m_tabManagerLeft(new TabManager(ui->tabWidget, false, this)),
    m_tabManagerRight(new TabManager(ui->tabWidget_2, true, this)),
    m_tableManagerLeft(new TableManager(ui->tableViewLeft, m_fileModelOpLeft, this)),
    m_tableManagerRight(new TableManager(ui->tableViewRight, m_fileModelOpRight, this)),
    m_treeManagerLeft(new TreeManager(ui->FileTreeView, m_fileModelOpLeft, ui->tabWidget, this)),
    m_treeManagerRight(new TreeManager(ui->FileTreeView_2, m_fileModelOpRight, ui->tabWidget_2, this)),
    m_fileOpManager(new FileOperationManager(this)),
    m_appStateHandler(new ApplicationStateHandler(this)),
    m_settingsDialog(new SettingsDialog(this)),
    m_fileOperationView(new FileOperationView(this)),
    m_menuBarView(new MenuBarView(this))
{
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);

    // get FileTreeView changes for command label
    connect(
        ui->FileTreeView->selectionModel(),
        &QItemSelectionModel::currentChanged,
        this,
        &MainWindow::onTreeSelectionChanged
    );

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
}

MainWindow::~MainWindow()
{
    delete ui;
}

auto MainWindow::getUI() -> Ui::MainWindow*
{
    return ui;
}

auto MainWindow::eventFilter(QObject* obj, QEvent* event) -> bool
{
    if (obj == ui->tabWidget && event->type() == QEvent::MouseButtonPress)
    {
        auto* mouseEvent = static_cast<QMouseEvent*>(event);

        if (mouseEvent->button() == Qt::MiddleButton)
        {
            m_tabManagerLeft->addNewTab();
            return true; // event işlendiyse true dön
        }
    }

    return QMainWindow::eventFilter(obj, event);
}

// sekme içerisindeki view'ların en son hangi dosya açıksa onu tekrar açması
void MainWindow::SetTabContent(int tabIndex, bool rightPane)
{
    if (rightPane)
    {
        m_tableManagerRight->SetTableContent(tabIndex);
        m_treeManagerRight->SetTreeContent(tabIndex);
    }
    else
    {
        m_tableManagerLeft->SetTableContent(tabIndex);
        m_treeManagerLeft->SetTreeContent(tabIndex);
    }
}

void MainWindow::OnTabMoved(int toIndex, int fromIndex)
{
    m_treeManagerLeft->swapExpandedPathsMap(toIndex, fromIndex);
    m_fileModelOpLeft->swapTabModelIndexMap(toIndex, fromIndex);
    m_fileModelOpLeft->swapTabHistoryModelIndex(toIndex, fromIndex);
}

void MainWindow::OnTabMoved2(int toIndex, int fromIndex)
{
    m_treeManagerRight->swapExpandedPathsMap(toIndex, fromIndex);
    m_fileModelOpRight->swapTabModelIndexMap(toIndex, fromIndex);
    m_fileModelOpRight->swapTabHistoryModelIndex(toIndex, fromIndex);
}

void MainWindow::SetLabelText_(const QString& path)
{
    ui->label->setText(path);

    // label default size (in the ui editor) should be bigger than needed
    ui->label->setMinimumSize(ui->label->sizeHint());
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_C) {
        ui->lineEdit->setFocus();
    }
}

void MainWindow::onTreeSelectionChanged(const QModelIndex& current, const QModelIndex&)
{
    const auto& path = static_cast<QFileSystemModel*>(ui->FileTreeView->model())->filePath(current);

    SetLabelText_(path);
}

void MainWindow::on_splitter_splitterLeftMoved(int pos, int )
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

void MainWindow::updateNavButtons(int const tabIndex, bool forRightPane)
{
    if(forRightPane)
    {
        const bool backButtonEnable = !m_fileModelOpRight->IsBackHistoryEmpty(tabIndex);
        const bool forwardButtonEnable = !m_fileModelOpRight->IsForwardHistoryEmpty(tabIndex);

        m_toolBarManager->SetBackButtonEnabled(backButtonEnable);
        m_toolBarManager->SetForwardButtonEnabled(forwardButtonEnable);

        const QModelIndex currentIndex = m_fileModelOpRight->GetTabModelIndex(tabIndex);
        const QString currentPath = m_fileModelOpRight->GetFilePath(currentIndex);

        // Eğer current path boş ise daha yukarı çıkılamıyordur.
        const bool upEnable = (currentPath != "");

        m_toolBarManager->SetUpButtonEnabled(upEnable);

        m_tabManagerRight->updateNavButtons(backButtonEnable, forwardButtonEnable, upEnable);
    }
    else
    {
        const bool backButtonEnable = !m_fileModelOpLeft->IsBackHistoryEmpty(tabIndex);
        const bool forwardButtonEnable = !m_fileModelOpLeft->IsForwardHistoryEmpty(tabIndex);

        m_toolBarManager->SetBackButtonEnabled(backButtonEnable);
        m_toolBarManager->SetForwardButtonEnabled(forwardButtonEnable);

        const QModelIndex currentIndex = m_fileModelOpLeft->GetTabModelIndex(tabIndex);
        const QString currentPath = m_fileModelOpLeft->GetFilePath(currentIndex);

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
        const auto tabIndex = ui->tabWidget_2->currentIndex();

        auto parentIndex = m_fileModelOpRight->GetFileModel()->parent(m_fileModelOpRight->GetTabModelIndex(tabIndex));

        m_fileModelOpRight->SetTabModelIndex(tabIndex, parentIndex);
        //tree back onClick missing

        m_tableManagerRight->SetTableContent(tabIndex);
        m_treeManagerRight->SetTreeContent(tabIndex);

        // buton kontrolü:
        updateNavButtons(tabIndex, true);
    }
    else
    {
        const auto tabIndex = ui->tabWidget->currentIndex();

        auto parentIndex = m_fileModelOpLeft->GetFileModel()->parent(m_fileModelOpLeft->GetTabModelIndex(tabIndex));

        m_fileModelOpLeft->SetTabModelIndex(tabIndex, parentIndex);
        //tree back onClick missing

        m_tableManagerLeft->SetTableContent(tabIndex);
        m_treeManagerLeft->SetTreeContent(tabIndex);

        // buton kontrolü:
        updateNavButtons(tabIndex, false);
    }
}

void MainWindow::BackButtonOnClick(bool OnRightPane)
{
    if(OnRightPane){
        const auto tabIndex = ui->tabWidget_2->currentIndex();

        m_fileModelOpRight->OnBackButtonClicked(tabIndex);
        //tree back onClick missing

        m_tableManagerRight->SetTableContent(tabIndex);
        m_treeManagerRight->SetTreeContent(tabIndex);

        // buton kontrolü:
        updateNavButtons(tabIndex, true);
    }
    else if(m_columnViewActive){
        ScrollColumn(-1);
    }
    else{
        const auto tabIndex = ui->tabWidget->currentIndex();

        m_fileModelOpLeft->OnBackButtonClicked(tabIndex);
        //tree back onClick missing

        m_tableManagerLeft->SetTableContent(tabIndex);
        m_treeManagerLeft->SetTreeContent(tabIndex);

        // buton kontrolü:
        updateNavButtons(tabIndex, false);
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
        const auto tabIndex = ui->tabWidget_2->currentIndex();

        m_fileModelOpRight->OnForwardButtonClicked(tabIndex);
        //tree back onClick missing

        m_tableManagerRight->SetTableContent(tabIndex);
        m_treeManagerRight->SetTreeContent(tabIndex);

        // buton kontrolü:
        updateNavButtons(tabIndex, true);
    }
    else if(m_columnViewActive){
        ScrollColumn(1);
    }
    else{
        const auto tabIndex = ui->tabWidget->currentIndex();

        m_fileModelOpLeft->OnForwardButtonClicked(tabIndex);
        //tree back onClick missing

        m_tableManagerLeft->SetTableContent(tabIndex);
        m_treeManagerLeft->SetTreeContent(tabIndex);

        // buton kontrolü:
        updateNavButtons(tabIndex, false);
    }
}

void MainWindow::on_toolUpButton_clicked()
{
    upperFolderOnClick(m_isWorkingOnRightPane);
}
void MainWindow::on_toolBackButton_clicked()
{
    BackButtonOnClick(m_isWorkingOnRightPane);
}
void MainWindow::on_toolForwardButton_clicked()
{
    ForwardButtonOnClick(m_isWorkingOnRightPane);
}


void MainWindow::on_lineEdit_returnPressed()
{
    auto path = m_fileModelOpLeft->GetFilePath(m_fileModelOpLeft->GetTabModelIndex(ui->tabWidget->currentIndex()));

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
    m_tableManagerLeft->SetColumnResize();

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

    m_tableManagerLeft->SetColumnResize();

    m_tabManagerLeft->EnableNavWidget(false);
    m_tabManagerRight->EnableNavWidget(false);
}

void MainWindow::DeactivateColumnView()
{
    m_columnViewActive = false;
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::ActivateColumnView()
{
    m_columnViewActive = true;
    ui->stackedWidget->setCurrentIndex(1);
    m_toolBarManager->SetBackButtonEnabled(true);
    m_toolBarManager->SetForwardButtonEnabled(true);
}

void MainWindow::on_tabWidget_tabBarClicked(int tabIndex)
{
    // Aynı sekmeye tıklanmadıysa
    if (tabIndex != m_tabManagerLeft->_getPreviousLeftTabIndex()
        &&
        tabIndex != -1)
    {
        m_tabManagerLeft->moveTabWidget(tabIndex);
        SetTabContent(tabIndex, false);
        // buton kontrolü:
        updateNavButtons(tabIndex, false);
    }

    SetLabelText_(m_fileModelOpLeft->GetCurrentPath(tabIndex));

    m_isWorkingOnRightPane = false;
}

void MainWindow::on_tabWidget_2_tabBarClicked(int tabIndex)
{
    // Aynı sekmeye tıklanmadıysa
    if (tabIndex != m_tabManagerRight->_getPreviousLeftTabIndex()
        &&
        tabIndex != -1)
    {
        m_tabManagerRight->moveTabWidget(tabIndex);
        SetTabContent(tabIndex, true);
        // buton kontrolü:
        updateNavButtons(tabIndex, true);
    }

    SetLabelText_(m_fileModelOpRight->GetCurrentPath(tabIndex));

    m_isWorkingOnRightPane = true;
}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    if (ui->tabWidget->count() > 1)
    {
        // move widget before closing current tab:
        if (ui->tabWidget->currentIndex() == index)
        {
            if (index >= 1)
            {
                m_tabManagerLeft->moveTabWidget(index - 1);
            }
            else
            {
                m_tabManagerLeft->moveTabWidget(index + 1);
            }
        }

        ui->tabWidget->removeTab(index);

        m_treeManagerLeft->removeTabExpandedPaths(index);
        m_fileModelOpLeft->RemoveTabModelIndex(index);

        m_tabManagerLeft->setPreviousLeftTabIndex(ui->tabWidget->currentIndex());
        SetTabContent(ui->tabWidget->currentIndex(), false);
    }
    else
    {
        m_treeManagerLeft->setTreeToDefault();
        m_tableManagerLeft->SetTableToDefault();

        m_leftTabIsReset = true;

        if (m_rightTabIsReset)
        {
            SetLabelText_("\\\\");
        }
        else
        {
            SetLabelText_(m_fileModelOpRight->GetFilePath(m_fileModelOpRight->GetTabModelIndex(ui->tabWidget_2->currentIndex())));
        }
    }
    m_isWorkingOnRightPane = false;

}

void MainWindow::on_tabWidget_2_tabCloseRequested(int index)
{
    if (ui->tabWidget_2->count() > 1)
    {
        // move widget before closing current tab:
        if (ui->tabWidget_2->currentIndex() == index)
        {
            if (index >= 1)
            {
                m_tabManagerRight->moveTabWidget(index - 1);
            }
            else
            {
                m_tabManagerRight->moveTabWidget(index + 1);
            }
        }

        ui->tabWidget_2->removeTab(index);

        m_treeManagerRight->removeTabExpandedPaths(index);
        m_fileModelOpRight->RemoveTabModelIndex(index);

        m_tabManagerRight->setPreviousLeftTabIndex(ui->tabWidget_2->currentIndex());
        SetTabContent(ui->tabWidget_2->currentIndex(), true);
    }
    else
    {
        m_treeManagerRight->setTreeToDefault();
        m_tableManagerRight->SetTableToDefault();

        m_rightTabIsReset = true;

        if (m_leftTabIsReset)
        {
            SetLabelText_("\\\\");
        }
        else
        {
            SetLabelText_(m_fileModelOpLeft->GetFilePath(m_fileModelOpLeft->GetTabModelIndex(ui->tabWidget->currentIndex())));
        }
    }
    m_isWorkingOnRightPane = true;

}


void MainWindow::on_FileTreeView_clicked(const QModelIndex &modelIndex)
{
    int const tabIndex = ui->tabWidget->currentIndex();
    m_treeManagerLeft->navigateToFolder(modelIndex, tabIndex);

    auto* fileModel = m_fileModelOpLeft->GetFileModel();
    // girilen yer klasör ise table view set edilir:
    if (fileModel->hasChildren(modelIndex))
    {
        ui->tableViewLeft->setRootIndex(modelIndex);
    }

    // buton kontrolü:
    updateNavButtons(tabIndex, false);

    m_leftTabIsReset = false;

    m_isWorkingOnRightPane = false;

}

void MainWindow::on_FileTreeView_2_clicked(const QModelIndex &modelIndex)
{
    int const tabIndex = ui->tabWidget_2->currentIndex();
    m_treeManagerRight->navigateToFolder(modelIndex, tabIndex);

    auto* fileModel = m_fileModelOpRight->GetFileModel();
    // girilen yer klasör ise table view set edilir:
    if (fileModel->hasChildren(modelIndex))
    {
        ui->tableViewRight->setRootIndex(modelIndex);
    }

    // buton kontrolü:
    updateNavButtons(tabIndex, true);

    m_rightTabIsReset = false;

    m_isWorkingOnRightPane = true;
}

void MainWindow::on_tableViewLeft_doubleClicked(const QModelIndex &modelIndex)
{
    const auto firstColumnIndex = modelIndex.siblingAtColumn(0); // her zaman ilk sütunu al

    m_tableManagerLeft->navigateToFolder(ui->tabWidget->currentIndex(), firstColumnIndex);

    m_treeManagerLeft->ExpandTreeView(firstColumnIndex);

    const auto tabIndex = ui->tabWidget->currentIndex();

    // buton kontrolü:
    updateNavButtons(tabIndex, false);

    const auto& path = static_cast<QFileSystemModel*>(ui->FileTreeView->model())->filePath(firstColumnIndex);
    SetLabelText_(path);

    m_leftTabIsReset = false;

    m_isWorkingOnRightPane = false;

}

void MainWindow::on_tableViewRight_doubleClicked(const QModelIndex &modelIndex)
{
    const auto firstColumnIndex = modelIndex.siblingAtColumn(0); // her zaman ilk sütunu al

    m_tableManagerRight->navigateToFolder(ui->tabWidget_2->currentIndex(), firstColumnIndex);

    m_treeManagerRight->ExpandTreeView(firstColumnIndex);

    const auto tabIndex = ui->tabWidget_2->currentIndex();

    // buton kontrolü:
    updateNavButtons(tabIndex, true);

    const auto& path = static_cast<QFileSystemModel*>(ui->FileTreeView_2->model())->filePath(firstColumnIndex);
    SetLabelText_(path);

    m_rightTabIsReset = false;

    m_isWorkingOnRightPane = true;

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
