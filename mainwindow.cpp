#include "mainwindow.hpp"
#include "./ui_mainwindow.h"
#include <QFileSystemModel>
#include <QAbstractButton>
#include <QDesktopServices>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    FileModel(new QFileSystemModel(this))
{
    ui->setupUi(this);

    // tree view setup:
    FileModel->setRootPath("");

    ui->FileTreeView->setModel(FileModel);
    ui->FileTreeView->setRootIndex(FileModel->index(FileModel->rootPath()));
    ui->FileTreeView->hideColumn(1);
    ui->FileTreeView->hideColumn(2);
    ui->FileTreeView->hideColumn(3);

    ui->tableView->setModel(FileModel);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(ui->FileTreeView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &MainWindow::onTreeSelectionChanged);

    ui->splitter->setSizes({100,400});

    setWindowTitle("File-Manager");
    show();
}
MainWindow::~MainWindow()
{
    delete ui;
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


void MainWindow::on_FileTreeView_doubleClicked(const QModelIndex &index)
{
    if(FileModel->hasChildren(index)) {
        ui->tableView->setRootIndex(index);
    }
}


void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    if(!FileModel->hasChildren(index)){
        const QString filePath = FileModel->filePath(index);
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }
}

