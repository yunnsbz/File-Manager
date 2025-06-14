#include "mainwindow.hpp"
#include "SettingsDialog.h"
#include "ThemeManager.h"
#include "TabManager.h"
#include "./ui_mainwindow.h"
#include "TableManager.h"
#include "ToolBarManager.h"
#include "FileModelOperations.h"
#include "TreeManager.h"
#include "ApplicationStateHandler.h"


#include <QFileSystemModel>
#include <QAbstractButton>
#include <QDesktopServices>
#include <QToolButton>
#include <QMouseEvent>
#include <QDir>
#include <QDialog>
#include <QDialogButtonBox>
#include <QProcess>
#include <QScrollBar>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QToolButton>
#include <QMessageBox>

MainWindow::UIManager::UIManager(Ui::MainWindow*& theUi, QMainWindow* pWnd)
{
    theUi->setupUi(pWnd);
}

MainWindow::MainWindow(QWidget* parent)
    :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_ui_mgr_(ui, this),
    fileModelOp(new FileModelOperations()),
    fileModelOp2(new FileModelOperations()),
    toolBarManager(new ToolBarManager(ui->toolBar, this)),
    menuManager(new ThemeManger(this)),
    tabManager(new TabManager(ui->tabWidget, false, this)),
    tabManager2(new TabManager(ui->tabWidget_2, true, this)),
    tableManager(new TableManager(ui->tableView, fileModelOp, this)),
    tableManager2(new TableManager(ui->tableView_2, fileModelOp2, this)),
    treeManager(new TreeManager(ui->FileTreeView, fileModelOp, ui->tabWidget, this)),
    treeManager2(new TreeManager(ui->FileTreeView_2, fileModelOp2, ui->tabWidget_2, this)),
    FileOpManager(new FileOperationManager(this)),
    AppStateHandler(new ApplicationStateHandler(this))
{
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);

    // get FileTreeView changes for command label
    connect(
        ui->FileTreeView->selectionModel(),
        &QItemSelectionModel::currentChanged,
        this,
        &MainWindow::onTreeSelectionChanged
    );

    columnFileModel = fileModelOp->GetFileModel();
    ui->columnView->setModel(columnFileModel);

    // tree view daha küçük olmalı
    ui->splitter->setSizes({100,400});
    ui->splitter_2->setSizes({100,400});

    // spliterlar ekranda sadece tree view olacak şekilde sürüklenmemeli
    ui->splitter->setCollapsible(1,false);
    ui->splitter_2->setCollapsible(1,false);

    //remove search line edit for now
    ui->lineEdit_2->setMaximumWidth(0);

    setWindowTitle("File Manager");
    show();

    // load and restore view:
    AppStateHandler->RestoreViewState();
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
            tabManager->addNewTab();
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
        tableManager2->SetTableContent(tabIndex);
        treeManager2->SetTreeContent(tabIndex);
    }
    else
    {
        tableManager->SetTableContent(tabIndex);
        treeManager->SetTreeContent(tabIndex);
    }
}

void MainWindow::OnTabMoved(int toIndex, int fromIndex)
{
    treeManager->swapExpandedPathsMap(toIndex, fromIndex);
    fileModelOp->swapTabModelIndexMap(toIndex, fromIndex);
    fileModelOp->swapTabHistoryModelIndex(toIndex, fromIndex);
}

void MainWindow::OnTabMoved2(int toIndex, int fromIndex)
{
    treeManager2->swapExpandedPathsMap(toIndex, fromIndex);
    fileModelOp2->swapTabModelIndexMap(toIndex, fromIndex);
    fileModelOp2->swapTabHistoryModelIndex(toIndex, fromIndex);
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

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::onTreeSelectionChanged(const QModelIndex& current, const QModelIndex&)
{
    const auto& path = static_cast<QFileSystemModel*>(ui->FileTreeView->model())->filePath(current);

    SetLabelText_(path);
}

void MainWindow::on_splitter_splitterMoved(int pos, int )
{
    if (pos < 10 && treeViewActive )
    {
        treeViewActive = false;
    }
    else if (pos > 10 && ! treeViewActive)
    {
        treeViewActive = true;
    }
}

void MainWindow::DeactivateTreeView()
{
    treeViewActive = false;
    ui->splitter->setSizes({0,400});
    ui->splitter_2->setSizes({0,400});

    // sürüklemeyi devre dışı bırakma:
    for (int i = 0; i < 2; ++i)
    {
        QSplitterHandle* handle = ui->splitter->handle(i);
        QSplitterHandle* handle2 = ui->splitter_2->handle(i);
        if (handle != nullptr && handle2 != nullptr)
        {
            handle->setEnabled(false);
            handle2->setEnabled(false);
        }
    }

    ui->splitter->setHandleWidth(0);
    ui->splitter_2->setHandleWidth(0);
}

void MainWindow::ActivateTreeView()
{
    treeViewActive = true;
    ui->splitter->setSizes({100,400});
    ui->splitter_2->setSizes({100,400});
    //  ilk view'ın sağ kenarı ve ikinci view'ın sol kenarı olmak üzere iki handle olur:
    for (int i = 0; i < 2; ++i)
    {
        QSplitterHandle* handle = ui->splitter->handle(i);
        QSplitterHandle* handle2 = ui->splitter_2->handle(i);
        if (handle != nullptr && handle2 != nullptr)
        {
            handle->setEnabled(true);
            handle2->setEnabled(true);
        }
    }

    ui->splitter->setHandleWidth(5);
    ui->splitter_2->setHandleWidth(5);
}

void MainWindow::on_actionTree_View_triggered()
{
    if (treeViewActive)
    {
        DeactivateTreeView();

        // save state update
        if(dualPaneActive){
            AppStateHandler->SetCurrentViewState(ViewStates::DUAL_PANE);
            qDebug()<<"dual pane açık ve ağaç kapatıldı";
        }
        else{
            AppStateHandler->SetCurrentViewState(ViewStates::SINGLE_TABLE);
            qDebug()<<"dual pane kapalı ve ağaç kapatıldı";
        }
    }
    else
    {
        ActivateTreeView();

        // save state update
        if(dualPaneActive){
            AppStateHandler->SetCurrentViewState(ViewStates::DUAL_PANE_W_TREE);
            qDebug()<<"dual pane açık ve ağaç açıldı";
        }
        else{
            AppStateHandler->SetCurrentViewState(ViewStates::SINGLE_TABLE_W_TREE);
            qDebug()<<"dual pane kapalı ve ağaç açıldı";
        }
    }
}

void MainWindow::updateNavButtons(int const tabIndex, bool forRightPane)
{
    if(forRightPane)
    {
        const bool backButtonEnable = !fileModelOp2->IsBackHistoryEmpty(tabIndex);
        const bool forwardButtonEnable = !fileModelOp2->IsForwardHistoryEmpty(tabIndex);

        toolBarManager->SetBackButtonEnabled(backButtonEnable);
        toolBarManager->SetForwardButtonEnabled(forwardButtonEnable);

        const QModelIndex currentIndex = fileModelOp2->GetTabModelIndex(tabIndex);
        const QString currentPath = fileModelOp2->GetFilePath(currentIndex);

        // Eğer current path boş ise daha yukarı çıkılamıyordur.
        const bool upEnable = (currentPath != "");

        toolBarManager->SetUpButtonEnabled(upEnable);

        tabManager2->updateNavButtons(backButtonEnable, forwardButtonEnable, upEnable);
    }
    else
    {
        bool backButtonEnable = !fileModelOp->IsBackHistoryEmpty(tabIndex);
        bool forwardButtonEnable = !fileModelOp->IsForwardHistoryEmpty(tabIndex);

        toolBarManager->SetBackButtonEnabled(backButtonEnable);
        toolBarManager->SetForwardButtonEnabled(forwardButtonEnable);

        const QModelIndex currentIndex = fileModelOp->GetTabModelIndex(tabIndex);
        const QString currentPath = fileModelOp->GetFilePath(currentIndex);

        // Eğer current path boş ise daha yukarı çıkılamıyordur.
        const bool upEnable = (currentPath != "");

        toolBarManager->SetUpButtonEnabled(upEnable);

        tabManager->updateNavButtons(backButtonEnable, forwardButtonEnable, upEnable);
    }
}

void MainWindow::upperFolderOnClick(bool OnRightPane)
{
    if(OnRightPane)
    {
        const auto tabIndex = ui->tabWidget_2->currentIndex();

        auto parentIndex = fileModelOp2->GetFileModel()->parent(fileModelOp2->GetTabModelIndex(tabIndex));

        fileModelOp2->SetTabModelIndex(tabIndex, parentIndex);
        //tree back onClick missing

        tableManager2->SetTableContent(tabIndex);
        treeManager2->SetTreeContent(tabIndex);

        // buton kontrolü:
        updateNavButtons(tabIndex, true);
    }
    else
    {
        const auto tabIndex = ui->tabWidget->currentIndex();

        auto parentIndex = fileModelOp->GetFileModel()->parent(fileModelOp->GetTabModelIndex(tabIndex));

        fileModelOp->SetTabModelIndex(tabIndex, parentIndex);
        //tree back onClick missing

        tableManager->SetTableContent(tabIndex);
        treeManager->SetTreeContent(tabIndex);

        // buton kontrolü:
        updateNavButtons(tabIndex, false);
    }
}

void MainWindow::BackButtonOnClick(bool OnRightPane)
{
    if(OnRightPane){
        const auto tabIndex = ui->tabWidget_2->currentIndex();

        fileModelOp2->OnBackButtonClicked(tabIndex);
        //tree back onClick missing

        tableManager2->SetTableContent(tabIndex);
        treeManager2->SetTreeContent(tabIndex);

        // buton kontrolü:
        updateNavButtons(tabIndex, true);
    }
    else if(ColumnViewActive){
        ScrollColumn(-1);
    }
    else{
        const auto tabIndex = ui->tabWidget->currentIndex();

        fileModelOp->OnBackButtonClicked(tabIndex);
        //tree back onClick missing

        tableManager->SetTableContent(tabIndex);
        treeManager->SetTreeContent(tabIndex);

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
                ui->columnView->horizontalScrollBar()->value() + direction * scrollAmount
                );
}

void MainWindow::ForwardButtonOnClick(bool OnRightPane)
{
    if(OnRightPane){
        const auto tabIndex = ui->tabWidget_2->currentIndex();

        fileModelOp2->OnForwardButtonClicked(tabIndex);
        //tree back onClick missing

        tableManager2->SetTableContent(tabIndex);
        treeManager2->SetTreeContent(tabIndex);

        // buton kontrolü:
        updateNavButtons(tabIndex, true);
    }
    else if(ColumnViewActive){
        ScrollColumn(1);
    }
    else{
        const auto tabIndex = ui->tabWidget->currentIndex();

        fileModelOp->OnForwardButtonClicked(tabIndex);
        //tree back onClick missing

        tableManager->SetTableContent(tabIndex);
        treeManager->SetTreeContent(tabIndex);

        // buton kontrolü:
        updateNavButtons(tabIndex, false);
    }
}

void MainWindow::on_toolUpButton_clicked()
{
    upperFolderOnClick(isWorkingOnRightPane);
}
void MainWindow::on_toolBackButton_clicked()
{
    BackButtonOnClick(isWorkingOnRightPane);
}
void MainWindow::on_toolForwardButton_clicked()
{
    ForwardButtonOnClick(isWorkingOnRightPane);
}

void MainWindow::on_actionAbout_triggered()
{
    QDialog dialog(this);
    dialog.setWindowTitle("About");

    auto* layout = new QVBoxLayout(&dialog);

    auto* label = new QLabel("Made by fatpound & yunns\nCopyright (c) 2025", &dialog);
    label->setAlignment(Qt::AlignmentFlag::AlignCenter);
    label->setWordWrap(true);
    layout->addWidget(label);

    auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok, &dialog);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    layout->addWidget(buttonBox);

    dialog.setLayout(layout);
    dialog.setFixedSize(300, 200);
    dialog.exec();
}

void MainWindow::on_lineEdit_returnPressed()
{
    auto path = fileModelOp->GetFilePath(fileModelOp->GetTabModelIndex(ui->tabWidget->currentIndex()));

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
    dualPaneActive = true;
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
    tableManager->SetColumnResize();

    tabManager->EnableNavWidget(true);
    tabManager2->EnableNavWidget(true);
}

void MainWindow::DeactivateDualPane()
{
    dualPaneActive = false;
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

    tableManager->SetColumnResize();

    tabManager->EnableNavWidget(false);
    tabManager2->EnableNavWidget(false);
}

void MainWindow::on_actionDual_Pane_View_triggered()
{
    // if current stackedWidget is not on column view then open or close dual pane
    if (ui->stackedWidget->currentIndex() == 0)
    {
        if (dualPaneActive)
        {
            DeactivateDualPane();

            // save state update
            if(treeViewActive) AppStateHandler->SetCurrentViewState(ViewStates::SINGLE_TABLE_W_TREE);
            else AppStateHandler->SetCurrentViewState(ViewStates::SINGLE_TABLE);
        }
        else
        {
            ActivateDualPane();

            // save state update
            if(treeViewActive) AppStateHandler->SetCurrentViewState(ViewStates::DUAL_PANE_W_TREE);
            else AppStateHandler->SetCurrentViewState(ViewStates::DUAL_PANE);
        }
    }
    else{
        // if column view is active the deactivate it and open dual pane
        ui->stackedWidget->setCurrentIndex(0);
        ColumnViewActive = false;

        // column'dan çıktıktan sonra dual pane açılmalı:
        ActivateDualPane();
    }
}


void MainWindow::ActivateColumnView()
{
    ColumnViewActive = true;
    ui->stackedWidget->setCurrentIndex(1);
    toolBarManager->SetBackButtonEnabled(true);
    toolBarManager->SetForwardButtonEnabled(true);
}

void MainWindow::on_actionColumn_View_triggered()
{
    if(ui->stackedWidget->currentIndex() == 1){
        ColumnViewActive = false;
        ui->stackedWidget->setCurrentIndex(0);
        if(dualPaneActive){
            if(treeViewActive){
                AppStateHandler->SetCurrentViewState(ViewStates::DUAL_PANE_W_TREE);
            }
            else{
                AppStateHandler->SetCurrentViewState(ViewStates::DUAL_PANE);
            }
        }
        else{
            if(treeViewActive){
                AppStateHandler->SetCurrentViewState(ViewStates::SINGLE_TABLE_W_TREE);
            }
            else{
                AppStateHandler->SetCurrentViewState(ViewStates::SINGLE_TABLE);
            }
        }
    }
    else{
        ActivateColumnView();
        AppStateHandler->SetCurrentViewState(ViewStates::COLUMN_VIEW);
    }
}

void MainWindow::on_tabWidget_tabBarClicked(int tabIndex)
{
    // Aynı sekmeye tıklanmadıysa
    if (tabIndex != tabManager->_getPreviousLeftTabIndex()
        &&
        tabIndex != -1)
    {
        tabManager->moveTabWidget(tabIndex);
        SetTabContent(tabIndex, false);
        // buton kontrolü:
        updateNavButtons(tabIndex, false);
    }

    SetLabelText_(fileModelOp->GetCurrentPath(tabIndex));

    isWorkingOnRightPane = false;
}

void MainWindow::on_tabWidget_2_tabBarClicked(int tabIndex)
{
    // Aynı sekmeye tıklanmadıysa
    if (tabIndex != tabManager2->_getPreviousLeftTabIndex()
        &&
        tabIndex != -1)
    {
        tabManager2->moveTabWidget(tabIndex);
        SetTabContent(tabIndex, true);
        // buton kontrolü:
        updateNavButtons(tabIndex, true);
    }

    SetLabelText_(fileModelOp2->GetCurrentPath(tabIndex));

    isWorkingOnRightPane = true;
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
                tabManager->moveTabWidget(index - 1);
            }
            else
            {
                tabManager->moveTabWidget(index + 1);
            }
        }

        ui->tabWidget->removeTab(index);

        treeManager->removeTabExpandedPaths(index);
        fileModelOp->RemoveTabModelIndex(index);

        tabManager->setPreviousLeftTabIndex(ui->tabWidget->currentIndex());
        SetTabContent(ui->tabWidget->currentIndex(), false);
    }
    else
    {
        treeManager->setTreeToDefault();
        tableManager->SetTableToDefault();

        leftTabIsReset_ = true;

        if (rightTabIsReset_)
        {
            SetLabelText_("\\\\");
        }
        else
        {
            SetLabelText_(fileModelOp2->GetFilePath(fileModelOp2->GetTabModelIndex(ui->tabWidget_2->currentIndex())));
        }
    }
    isWorkingOnRightPane = false;

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
                tabManager2->moveTabWidget(index - 1);
            }
            else
            {
                tabManager2->moveTabWidget(index + 1);
            }
        }

        ui->tabWidget_2->removeTab(index);

        treeManager2->removeTabExpandedPaths(index);
        fileModelOp2->RemoveTabModelIndex(index);

        tabManager2->setPreviousLeftTabIndex(ui->tabWidget_2->currentIndex());
        SetTabContent(ui->tabWidget_2->currentIndex(), true);
    }
    else
    {
        treeManager2->setTreeToDefault();
        tableManager2->SetTableToDefault();

        rightTabIsReset_ = true;

        if (leftTabIsReset_)
        {
            SetLabelText_("\\\\");
        }
        else
        {
            SetLabelText_(fileModelOp->GetFilePath(fileModelOp->GetTabModelIndex(ui->tabWidget->currentIndex())));
        }
    }
    isWorkingOnRightPane = true;

}


void MainWindow::on_FileTreeView_clicked(const QModelIndex &modelIndex)
{
    int const tabIndex = ui->tabWidget->currentIndex();
    treeManager->navigateToFolder(modelIndex, tabIndex);

    auto* fileModel = fileModelOp->GetFileModel();
    // girilen yer klasör ise table view set edilir:
    if (fileModel->hasChildren(modelIndex))
    {
        ui->tableView->setRootIndex(modelIndex);
    }

    // buton kontrolü:
    updateNavButtons(tabIndex, false);

    leftTabIsReset_ = false;

    isWorkingOnRightPane = false;

}

void MainWindow::on_FileTreeView_2_clicked(const QModelIndex &modelIndex)
{
    int const tabIndex = ui->tabWidget_2->currentIndex();
    treeManager2->navigateToFolder(modelIndex, tabIndex);

    auto* fileModel = fileModelOp2->GetFileModel();
    // girilen yer klasör ise table view set edilir:
    if (fileModel->hasChildren(modelIndex))
    {
        ui->tableView_2->setRootIndex(modelIndex);
    }

    // buton kontrolü:
    updateNavButtons(tabIndex, true);

    rightTabIsReset_ = false;

    isWorkingOnRightPane = true;
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &modelIndex)
{
    const auto firstColumnIndex = modelIndex.siblingAtColumn(0); // her zaman ilk sütunu al

    tableManager->navigateToFolder(ui->tabWidget->currentIndex(), firstColumnIndex);

    treeManager->ExpandTreeView(firstColumnIndex);

    const auto tabIndex = ui->tabWidget->currentIndex();

    // buton kontrolü:
    updateNavButtons(tabIndex, false);

    const auto& path = static_cast<QFileSystemModel*>(ui->FileTreeView->model())->filePath(firstColumnIndex);
    SetLabelText_(path);

    leftTabIsReset_ = false;

    isWorkingOnRightPane = false;

}

void MainWindow::on_tableView_2_doubleClicked(const QModelIndex &modelIndex)
{
    const auto firstColumnIndex = modelIndex.siblingAtColumn(0); // her zaman ilk sütunu al

    tableManager2->navigateToFolder(ui->tabWidget_2->currentIndex(), firstColumnIndex);

    treeManager2->ExpandTreeView(firstColumnIndex);

    const auto tabIndex = ui->tabWidget_2->currentIndex();

    // buton kontrolü:
    updateNavButtons(tabIndex, true);

    const auto& path = static_cast<QFileSystemModel*>(ui->FileTreeView_2->model())->filePath(firstColumnIndex);
    SetLabelText_(path);

    rightTabIsReset_ = false;

    isWorkingOnRightPane = true;

}

void MainWindow::on_toolSearchButton_clicked()
{
    if(searchOn){
        if (ui->lineEdit_2 != nullptr) {
            searchOn = false;
            QPropertyAnimation *animation = new QPropertyAnimation(ui->lineEdit_2, "maximumWidth");
            animation->setDuration(300);
            animation->setStartValue(ui->lineEdit_2->width());
            animation->setEndValue(0); // önceki boyut
            animation->start(QAbstractAnimation::DeleteWhenStopped);
        }
    }
    else{
        if (ui->lineEdit_2 != nullptr) {
            searchOn = true;
            QPropertyAnimation *animation = new QPropertyAnimation(ui->lineEdit_2, "maximumWidth");
            animation->setDuration(300);
            animation->setStartValue(ui->lineEdit_2->width());
            animation->setEndValue(250); // hedef genişlik
            animation->start(QAbstractAnimation::DeleteWhenStopped);

            ui->lineEdit_2->setFocus();
        }
    }
}




void MainWindow::on_actionOptions_triggered()
{
    if(tabCloseButtonOld){
        // style:
        QString qss;
        QFile file1(":/resources/styles/styleDark.qss");
        QFile file2(":/resources/styles/button-style-new.qss");

        file1.open(QFile::ReadOnly);
        file2.open(QFile::ReadOnly);

        qss += file1.readAll();
        qss += file2.readAll();

        qApp->setStyleSheet(qss);
        tabCloseButtonOld = false;
    }
    else{
        // style:
        QString qss;
        QFile file1(":/resources/styles/styleDark.qss");

        file1.open(QFile::ReadOnly);

        qss += file1.readAll();

        qApp->setStyleSheet(qss);
        tabCloseButtonOld = true;
    }
}


void MainWindow::on_columnView_clicked(const QModelIndex &index)
{
    if (!columnFileModel->hasChildren(index))
    {
        const QString filePath = columnFileModel->filePath(index);
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }
}


void MainWindow::on_toolHistoryButton_clicked()
{
    QFrame* popupFrame = new QFrame(this, Qt::Popup); // Menü gibi görünür
    QVBoxLayout* layout = new QVBoxLayout(popupFrame);
    popupFrame->setObjectName("HistoryPopupFrame");

    QScrollArea* scrollArea = new QScrollArea(popupFrame);
    scrollArea->setWidgetResizable(true);

    QWidget* scrollContent = new QWidget();
    QVBoxLayout* contentLayout = new QVBoxLayout(scrollContent);
    scrollContent->setObjectName("HistoryScrollContent");
    scrollContent->setAttribute(Qt::WA_Hover, true);

    contentLayout->setContentsMargins(1,2,1,2);
    contentLayout->setSpacing(0);

    for (int i = 0; i < 20; i++) {
        QWidget* itemWidget = new QWidget;
        QHBoxLayout* rowLayout = new QHBoxLayout(itemWidget);

        rowLayout->setContentsMargins(1, 2, 1, 2); // min dikey boşluk
        rowLayout->setSpacing(0);

        QLabel* label = new QLabel("operation description (... moved to ...)");
        QToolButton* undoButton = new QToolButton();
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


void MainWindow::on_toolCopyButton_clicked()
{
    // seçilmiş satırların indexlerini al
    // QModelIndexList selectedIndexes = ui->tableView->selectionModel()->selectedRows();
    // if(selectedIndexes.count() != 0)
    // {
    //     for (const QModelIndex &index : selectedIndexes) {
    //         QString filePath = fileModelOp->GetFileModel()->filePath(index);
    //         qDebug()<< "path added to copy (table 1):" << filePath;
    //         FileOpManager->addToCopy(filePath);
    //     }
    // }
    // else
    // {
    //     // ilk tabloda seçilmemişse
    //     selectedIndexes = ui->tableView_2->selectionModel()->selectedRows();
    //     for (const QModelIndex &index : selectedIndexes) {
    //         QString filePath = fileModelOp->GetFileModel()->filePath(index);
    //         qDebug()<< "path added to copy (table 2): " << filePath;
    //         FileOpManager->addToCopy(filePath);
    //     }
    // }
}


void MainWindow::on_toolPasteButton_clicked()
{
    // FileOpManager->PasteOperation(fileModelOp->GetCurrentPath(ui->tabWidget->currentIndex()));
}


void MainWindow::on_toolDelButton_clicked()
{
    QList<QString> srcList;

    QModelIndexList selectedIndexes = (! isWorkingOnRightPane)
            ? ui->tableView->selectionModel()->selectedRows()
            : ui->tableView_2->selectionModel()->selectedRows()
            ;

    if ( ! selectedIndexes.empty())
    {
        for (const QModelIndex& index : selectedIndexes)
        {
            srcList.append(fileModelOp->GetFileModel()->filePath(index));
        }

        const QString text = QString("Seçili %1 dosyayı silmek üzeresiniz. Emin misiniz?")
                               .arg(srcList.size());

        QMessageBox::StandardButton const reply = QMessageBox::question(
                this,
                "Dosyaları Sil",
                text,
                QMessageBox::Yes | QMessageBox::No,
                QMessageBox::No
            );

        if(reply == QMessageBox::Yes){
            FileOpManager->DeleteOperation(srcList);
        }
    }
}


void MainWindow::on_actionSettings_triggered()
{
    SettingsDialog settingsDialog(this);
    settingsDialog.exec(); // Modal olarak açar
}

