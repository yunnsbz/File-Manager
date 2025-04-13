#include "mainwindow.hpp"
#include "./ui_mainwindow.h"
#include <QFileSystemModel>
#include <QAbstractButton>
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

    ui->splitter->setSizes({100,400});
    SetTabUI();
    setWindowTitle("File-Manager");
    show();
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetTabUI(){
    // QTabWidget içindeki QTabBar'a erişim:
    QTabBar *tabBar = ui->tabWidget->tabBar();

    // Her sekme için close butonunu değiştir:
    for (int i = 0; i < tabBar->count(); ++i) {
        QWidget *tabButton = tabBar->tabButton(i, QTabBar::RightSide);
        if (QAbstractButton *closeButton = qobject_cast<QAbstractButton*>(tabButton)) {
            // Yeni ikon set et
            QIcon myCloseIcon(":/icons/custom_close.png");
            QIcon closeIcon = QApplication::style()->standardIcon(QStyle::SP_DialogCloseButton);
            closeButton->setIcon(closeIcon);
            closeButton->setIconSize(QSize(12, 12)); // isteğe bağlı boyut
        }
    }
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

