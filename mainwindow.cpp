#include "mainwindow.hpp"
#include "TabManager.h"
#include "./ui_mainwindow.h"
#include "TableManager.h"
#include "ToolBarManager.h"
#include "FileModelOperations.h"
#include "TreeManager.h"

#include <QFileSystemModel>
#include <QAbstractButton>
#include <QDesktopServices>
#include <QToolButton>
#include <QMouseEvent>
#include <QDir>
#include <QDialog>
#include <QDialogButtonBox>
#include <QProcess>

UIManager::UIManager(Ui::MainWindow*& theUi, QMainWindow* pWnd)
{
    theUi->setupUi(pWnd);
}

MainWindow::MainWindow(QWidget *parent)
    :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_ui_mgr_(ui, this),
    fileModelOp(new FileModelOperations()),
    fileModelOp2(new FileModelOperations()),
    toolBarManager(new ToolBarManager(ui->toolBar, this)),
    tabManager(new TabManager(ui->tabWidget, false, this)),
    tabManager2(new TabManager(ui->tabWidget_2, true, this)),
    tableManager(new TableManager(ui->tableView, fileModelOp, this)),
    tableManager2(new TableManager(ui->tableView_2, fileModelOp2, this)),
    treeManager(new TreeManager(ui->FileTreeView, fileModelOp, this)),
    treeManager2(new TreeManager(ui->FileTreeView_2, fileModelOp2, this))
{
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);

    // get FileTreeView changes for command label
    connect(
        ui->FileTreeView->selectionModel(),
        &QItemSelectionModel::currentChanged,
        this,
        &MainWindow::onTreeSelectionChanged
    );

    auto* fileModel = fileModelOp->GetFileModel();
    ui->columnView->setModel(fileModel);

    // tree view daha küçük olmalı
    ui->splitter->setSizes({100,400});
    ui->splitter_2->setSizes({100,400});

    // spliterlar ekranda sadece tree view olacak şekilde sürüklenmemeli
    ui->splitter->setCollapsible(1,false);
    ui->splitter_2->setCollapsible(1,false);

    setWindowTitle("File Manager");
    show();
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

auto MainWindow::GetCurrentTabIndex() -> int
{
    return ui->tabWidget->currentIndex();
}

auto MainWindow::GetPreviousTabIndex() -> int
{
    return tabManager->getPersistentPreviousLeftTabIndex();
}

void MainWindow::OnTabMoved(int toIndex, int fromIndex)
{
    treeManager->swapExpandedPathsMap(toIndex, fromIndex);
    fileModelOp->swapTabModelIndexMap(toIndex, fromIndex);
    fileModelOp->swapTabHistoryModelIndex(toIndex, fromIndex);
}

void MainWindow::UpdateLabel_(const QString& path)
{
    ui->label->setText(path);

    // label default size (in the ui editor) should be bigger than needed
    ui->label->setMinimumSize(ui->label->sizeHint());
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
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

    UpdateLabel_(path);
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

void MainWindow::on_actionList_View_triggered()
{
    if (treeViewActive)
    {
        treeViewActive = false;
        ui->splitter->setSizes({0,400});

        // sürüklemeyi devre dışı bırakma:
        for (int i = 0; i < 2; ++i)
        {
            QSplitterHandle* handle = ui->splitter->handle(i);
            if (handle != nullptr)
            {
                handle->setEnabled(false);
            }
        }
        ui->splitter->setHandleWidth(0);
    }
    else
    {
        treeViewActive = true;
        ui->splitter->setSizes({100,400});
        //  ilk view'ın sağ kenarı ve ikinci view'ın sol kenarı olmak üzere iki handle olur:
        for (int i = 0; i < 2; ++i)
        {
            QSplitterHandle* handle = ui->splitter->handle(i);
            if (handle != nullptr)
            {
                handle->setEnabled(true);
            }
        }

        ui->splitter->setHandleWidth(5);
    }
}

void MainWindow::updateHistoryButtons(int const tabIndex)
{
    toolBarManager->SetBackButtonEnabled(!fileModelOp->IsBackHistoryEmpty(tabIndex));
    toolBarManager->SetForwardButtonEnabled(!fileModelOp->IsForwardHistoryEmpty(tabIndex));
}

void MainWindow::on_toolBackButton_clicked()
{
    const auto tabIndex = ui->tabWidget->currentIndex();

    fileModelOp->OnBackButtonClicked(tabIndex);
    //tree back onClick missing

    tableManager->SetTableContent(tabIndex);
    treeManager->SetTreeContent(tabIndex);

    // buton kontrolü:
    updateHistoryButtons(tabIndex);
}

void MainWindow::on_toolForwardButton_clicked()
{
    const auto tabIndex = ui->tabWidget->currentIndex();

    fileModelOp->OnForwardButtonClicked(tabIndex);
    //tree back onClick missing

    tableManager->SetTableContent(tabIndex);
    treeManager->SetTreeContent(tabIndex);

    // buton kontrolü:
    updateHistoryButtons(tabIndex);
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


void MainWindow::on_actionDual_Pane_View_triggered()
{
    if(ui->stackedWidget->currentIndex() == 0){
        if (dualPaneActive)
        {
            dualPaneActive = false;
            ui->splitter_dualPane->setChildrenCollapsible(true);
            ui->splitter_dualPane->setSizes({1,0});
            ui->splitter_dualPane->setChildrenCollapsible(false);
            // sürüklemeyi devre dışı bırakma:
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
        }
        else
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
        }
    }
    else{
        // column view açıksa onu kapatıp bu fonksiyonu tekrar çağırarak dual pane'i açar
        ColumnViewActive = false;
        ui->stackedWidget->setCurrentIndex(0);

        // column dan çıktıktan sonra her halukarda dual pane açılsın diye:
        dualPaneActive = false;

        on_actionDual_Pane_View_triggered();
    }
}


void MainWindow::on_actionColumn_View_triggered()
{
    if(ui->stackedWidget->currentIndex() == 1){
        ColumnViewActive = false;
        ui->stackedWidget->setCurrentIndex(0);
    }
    else{
        ColumnViewActive = true;
        ui->stackedWidget->setCurrentIndex(1);
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
        updateHistoryButtons(tabIndex);
    }
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
        updateHistoryButtons(tabIndex);
    }
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
    }
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
    }
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
    updateHistoryButtons(tabIndex);
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
    updateHistoryButtons(tabIndex);
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &modelIndex)
{
    const auto firstColumnIndex = modelIndex.siblingAtColumn(0); // her zaman ilk sütunu al

    tableManager->navigateToFolder(ui->tabWidget->currentIndex(), firstColumnIndex);

    treeManager->ExpandTreeView(firstColumnIndex);

    const auto tabIndex = ui->tabWidget->currentIndex();

    // buton kontrolü:
    updateHistoryButtons(tabIndex);

    const auto& path = static_cast<QFileSystemModel*>(ui->FileTreeView->model())->filePath(firstColumnIndex);
    UpdateLabel_(path);
}

void MainWindow::on_tableView_2_doubleClicked(const QModelIndex &modelIndex)
{
    const auto firstColumnIndex = modelIndex.siblingAtColumn(0); // her zaman ilk sütunu al

    tableManager2->navigateToFolder(ui->tabWidget_2->currentIndex(), firstColumnIndex);

    treeManager2->ExpandTreeView(firstColumnIndex);

    const auto tabIndex = ui->tabWidget_2->currentIndex();

    // buton kontrolü:
    updateHistoryButtons(tabIndex);

    const auto& path = static_cast<QFileSystemModel*>(ui->FileTreeView_2->model())->filePath(firstColumnIndex);
    UpdateLabel_(path);
}
