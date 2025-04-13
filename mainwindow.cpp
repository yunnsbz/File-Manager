#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileSystemModel>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Model oluştur
    FileModel = new QFileSystemModel(this);
    FileModel->setRootPath(QDir::rootPath());

    // UI'dan gelen QTreeView'e model bağla
    ui->FileTreeView->setModel(FileModel);
    ui->FileTreeView->setRootIndex(FileModel->index(QDir::homePath()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

Ui::MainWindow* MainWindow::getUI(){
    return ui;
}
