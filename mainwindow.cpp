#include "mainwindow.hpp"
#include "./ui_mainwindow.h"
#include <QFileSystemModel>
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

    connect(ui->FileTreeView->selectionModel(), &QItemSelectionModel::currentChanged,
            this, &MainWindow::onTreeSelectionChanged);


    setWindowTitle("File-Manager");
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

void MainWindow::on_actionExit_triggered()
{
    close();
}
void MainWindow::onTreeSelectionChanged(const QModelIndex &current, const QModelIndex &)
{
    QString path = static_cast<QFileSystemModel*>(ui->FileTreeView->model())->filePath(current);
    ui->label->setText(path);
    ui->label->setAlignment(Qt::AlignRight);
    ui->label->setMinimumSize(ui->label->sizeHint());
}

