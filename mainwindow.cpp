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
    toolBarManager(new ToolBarManager(ui->toolBar ,this)),
    tabManager(new TabManager(ui->tabWidget, this)),
    tableManager(new TableManager(ui->tableView, this)),
    treeManager(new TreeManager(ui->FileTreeView, this))
{
    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);

    // get FileTreeView changes for command label
    connect(
        ui->FileTreeView->selectionModel(),
        &QItemSelectionModel::currentChanged,
        this,
        &MainWindow::onTreeSelectionChanged
    );

    auto* fileModel = FileModelOperations::GetFileModel();
    ui->columnView->setModel(fileModel);

    // tree view daha küçük olmalı
    ui->splitter->setSizes({100,400});
    ui->splitter_2->setSizes({100,400});

    // spliterlar ekranda sadece tree view olacak şekilde sürüklenmemeli
    ui->splitter->setCollapsible(1,false);
    ui->splitter_2->setCollapsible(1,false);

    // close dual pane by default:
    on_actionDual_Pane_View_triggered();

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
void MainWindow::SetTabContent(int tabIndex)
{
    tableManager->SetTableContent(tabIndex);
    treeManager->SetTreeContent(tabIndex);
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

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    const QModelIndex firstColumnIndex = index.siblingAtColumn(0); // her zaman ilk sütunu al

    tableManager->navigateToFolder(ui->tabWidget->currentIndex(), firstColumnIndex);

    treeManager->ExpandTreeView(firstColumnIndex);

    const auto tabIndex = ui->tabWidget->currentIndex();

    // buton kontrolü:
    updateHistoryButtons(tabIndex);

    const auto& path = static_cast<QFileSystemModel*>(ui->FileTreeView->model())->filePath(firstColumnIndex);
    UpdateLabel_(path);
}

void MainWindow::on_splitter_splitterMoved(int pos, int )
{
    if (pos < 10 && treeActive )
    {
        treeActive = false;
    }
    else if (pos > 10 && !treeActive)
    {
        treeActive = true;
    }
}

void MainWindow::on_actionList_View_triggered()
{
    if (treeActive)
    {
        treeActive = false;
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
        treeActive = true;
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
        FileModelOperations::RemoveTabModelIndex(index);

        tabManager->setPreviousLeftTabIndex(ui->tabWidget->currentIndex());
        SetTabContent(ui->tabWidget->currentIndex());
    }
    else
    {
        treeManager->setTreeToDefault();
        tableManager->SetTableToDefault();
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
        SetTabContent(tabIndex);
        // buton kontrolü:
        updateHistoryButtons(tabIndex);
    }
}

void MainWindow::updateHistoryButtons(int const tabIndex)
{
    toolBarManager->SetBackButtonEnabled(!FileModelOperations::IsBackHistoryEmpty(tabIndex));
    toolBarManager->SetForwardButtonEnabled(!FileModelOperations::IsForwardHistoryEmpty(tabIndex));
}

void MainWindow::on_FileTreeView_clicked(const QModelIndex &modelIndex)
{
    int const tabIndex = ui->tabWidget->currentIndex();
    treeManager->navigateToFolder(modelIndex, tabIndex);

    auto* fileModel = FileModelOperations::GetFileModel();
    // girilen yer klasör ise table view set edilir:
    if (fileModel->hasChildren(modelIndex))
    {
        ui->tableView->setRootIndex(modelIndex);
    }

    // buton kontrolü:
    updateHistoryButtons(tabIndex);
}

void MainWindow::on_toolBackButton_clicked()
{
    const auto tabIndex = ui->tabWidget->currentIndex();

    FileModelOperations::OnBackButtonClicked(tabIndex);
    //tree back onClick missing

    tableManager->SetTableContent(tabIndex);
    treeManager->SetTreeContent(tabIndex);

    // buton kontrolü:
    updateHistoryButtons(tabIndex);
}

void MainWindow::on_toolForwardButton_clicked()
{
    const auto tabIndex = ui->tabWidget->currentIndex();

    FileModelOperations::OnForwardButtonClicked(tabIndex);
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
    auto path = FileModelOperations::GetFilePath(FileModelOperations::GetTabModelIndex(ui->tabWidget->currentIndex()));

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
    }
}


void MainWindow::on_actionColumn_View_triggered()
{
    if(ColumnViewActive){
        ColumnViewActive = false;
        ui->stackedWidget->setCurrentIndex(0);
    }
    else{
        ColumnViewActive = true;
        ui->stackedWidget->setCurrentIndex(1);
    }
}

