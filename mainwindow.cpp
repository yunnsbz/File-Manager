#include "mainwindow.hpp"
#include "TabManager.h"
#include "./ui_mainwindow.h"
#include "ToolBarManager.h"

#include <QFileSystemModel>
#include <QAbstractButton>
#include <QDesktopServices>
#include <QToolButton>
#include <QMouseEvent>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    FileModel(new QFileSystemModel(this))
{
    ui->setupUi(this);

    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);

    // tool bar setup:
    toolBarManager = new ToolBarManager(ui->toolBar,this);

    // tab widget setup:
    tabManager = new TabManager(ui->tabWidget, FileModel, toolBarManager, this);

    // tree view setup:
    // disk directory
    FileModel->setRootPath("");
    ui->FileTreeView->setModel(FileModel);
    ui->FileTreeView->setRootIndex(FileModel->index(FileModel->rootPath()));

    // hiding unnecessary columns
    ui->FileTreeView->hideColumn(1);
    ui->FileTreeView->hideColumn(2);
    ui->FileTreeView->hideColumn(3);

    // table view setup:
    ui->tableView->setModel(FileModel);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    // get FileTreeView changes for command label
    connect(
        ui->FileTreeView->selectionModel(),
        &QItemSelectionModel::currentChanged,
        this,
        &MainWindow::onTreeSelectionChanged
    );

    ui->splitter->setSizes({100,400});

    setWindowTitle("File-Manager");
    show();
}
MainWindow::~MainWindow()
{
    delete ui;
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
    // set table view content:
    auto index = tabManager->getTabModelIndex(tabIndex);

    if (!index.isValid())
    {
        setDefaultContent();
        return;
    }

    ui->tableView->setRootIndex(index);

    auto expandedPaths = tabManager->getTreeExpandedPaths(tabIndex);


    // set tree view content:
    if(expandedPaths.isEmpty())
    {
        setDefaultContent();
        return;
    }

    ui->FileTreeView->collapseAll();

    for (const QString& path : expandedPaths) {
        const QModelIndex index = FileModel->index(path);
        if (index.isValid()) {
            ui->FileTreeView->expand(index);
        }
    }
}

// sekme içerisindeki view'ların default index'e getirilmesi:
void MainWindow::setDefaultContent()
{
    FileModel->setRootPath("");
    const QModelIndex index = FileModel->index(FileModel->rootPath());
    ui->FileTreeView->setRootIndex(index);
    ui->FileTreeView->collapseAll();
    ui->tableView->setRootIndex(index);
}

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::onTreeSelectionChanged(const QModelIndex& current, const QModelIndex&)
{
    const auto& path = static_cast<QFileSystemModel*>(ui->FileTreeView->model())->filePath(current);
    ui->label->setText(path);
    // label default size (in the ui editor) should be bigger than needed
    ui->label->setMinimumSize(ui->label->sizeHint());
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    const QModelIndex firstColumnIndex = index.siblingAtColumn(0); // her zaman ilk sütunu al

    if (!FileModel->hasChildren(firstColumnIndex))
    {
        const QString filePath = FileModel->filePath(index);
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }
    else
    {
        ui->tableView->setRootIndex(firstColumnIndex);
        ui->FileTreeView->expand(firstColumnIndex);
        tabManager->setFileIndexMap(ui->tableView);
    }
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
        tabManager->RemoveTabContent(index);
        tabManager->setLastLeftTabIndex(ui->tabWidget->currentIndex());
        SetTabContent(ui->tabWidget->currentIndex());
    }
    else
    {
        setDefaultContent();
    }

}

void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    // Aynı sekmeye tıklanmadıysa
    if (index != tabManager->getLastLeftTabIndex()
        &&
        index != -1)
    {
        tabManager->moveTabWidget(index);
        SetTabContent(index);
    }
}

void MainWindow::on_FileTreeView_clicked(const QModelIndex &index)
{
    if (FileModel->hasChildren(index))
    {
        ui->tableView->setRootIndex(index);
        tabManager->setFileIndexMap(ui->tableView);
    }

    // single click tree expanding and collapsing:
    if (ui->FileTreeView->isExpanded(index))
    {
        ui->FileTreeView->collapse(index);
    }
    else
    {
        ui->FileTreeView->expand(index);
        //tabsExpandedIndexes.value(ui->tabWidget->currentIndex()).append(index);
    }
}

void MainWindow::on_toolBackButton_clicked()
{
    tabManager->onBackButtonClicked();
}


void MainWindow::on_toolForwardButton_clicked()
{
    tabManager->onForwardButtonClicked();
}

