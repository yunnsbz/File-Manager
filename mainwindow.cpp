#include "mainwindow.hpp"
#include "./ui_mainwindow.h"
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

    // tab widget setup:
    setupTabs();

    // get FileTreeView changes for command label
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

// sekme içeriğini kaydetmek içindir.
void MainWindow::SetFileIndexMap()
{
    auto* currentTab = ui->tabWidget->currentWidget();
    const QModelIndex index = ui->tableView->currentIndex();
    QString path = FileModel->filePath(index);
    tabFilePathMap[currentTab] = path;
}

// yeni sekme ekleme butonunun eklenmesi, işlevi ve ilk açılıştaki sekmeleri ayarlama
void MainWindow::setupTabs() {
    // Başlangıçta bir sekme ve bir '+' sekmesi ekle
    ui->tabWidget->removeTab(1);

    auto* addTabButton = new QToolButton();
    addTabButton->setText("+");
    ui->tabWidget->setCornerWidget(addTabButton, Qt::TopLeftCorner);

    // add button onClick:
    connect(addTabButton, &QToolButton::clicked, this, [this]() {
        // Mevcut sekmedeki içerikleri al
        QWidget* currentTabContent = ui->tabWidget->currentWidget();

        // Eğer current tab'da bir splitter varsa
        auto* currentSplitter = currentTabContent->findChild<QSplitter*>();

        if (currentSplitter != nullptr) {
            // Yeni bir widget oluştur (sekme için container)
            auto* newTabWidget = new QWidget();
            auto* layout = new QVBoxLayout(newTabWidget);
            layout->addWidget(currentSplitter);  // Mevcut splitter'ı ekle
            layout->setContentsMargins(0,0,0,0);

            // Yeni sekmeye ekle
            ui->tabWidget->addTab(newTabWidget, "Yeni Sekme");
            ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);  // Yeni sekmeye geç
            lastLeftTabIndex = ui->tabWidget->count() - 1;
            SetTabContent(ui->tabWidget->currentWidget());
        }
    });
}

// sekme içerisindeki view'ların en son hangi dosya açıksa onu tekrar açması
void MainWindow::SetTabContent(QWidget* tabWidget){
    if (!tabFilePathMap.contains(tabWidget)){
        SetTabContentToDefault();
        return;
    }
    const QString path = tabFilePathMap[tabWidget];
    const QModelIndex index = FileModel->index(path);

    if (!index.isValid()){
        SetTabContentToDefault();
        return;
    }
    ui->FileTreeView->setRootIndex(index);
    ui->FileTreeView->collapseAll();
    ui->FileTreeView->expand(index);

    ui->tableView->setRootIndex(index);
}

// sekme içerisindeki view'ların en son hangi dosya açıksa onu tekrar açması
void MainWindow::SetTabContentToDefault(){
    FileModel->setRootPath("");
    const QModelIndex index = FileModel->index(FileModel->rootPath());
    ui->FileTreeView->setRootIndex(index);
    ui->FileTreeView->collapseAll();
    ui->tableView->setRootIndex(index);
}

// sekme içerisini kopyalamak yerine taşıyarak sekmeler arasında geçiş yaparız
void MainWindow::MoveTabWidget(int index)
{
    QWidget* currentTabContent = ui->tabWidget->widget(lastLeftTabIndex);
    QSplitter* currentSplitter = currentTabContent->findChild<QSplitter*>();

    if (currentSplitter) {

        // Splitter'ı mevcut yerinden kopar
        currentSplitter->setParent(nullptr);

        // Yeni container oluştur
        auto* newTabWidget = new QWidget();
        auto* layout = new QVBoxLayout(newTabWidget);
        layout->addWidget(currentSplitter);
        layout->setContentsMargins(0, 0, 0, 0);

        // Yeni sekmeyi oluştur
        ui->tabWidget->insertTab(index, newTabWidget, ui->tabWidget->tabText(index));
        ui->tabWidget->removeTab(index + 1); // eski widget'ı kaldır
        ui->tabWidget->setCurrentIndex(index);

        lastLeftTabIndex = index;
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

void MainWindow::on_FileTreeView_doubleClicked(const QModelIndex &index)
{
    if(FileModel->hasChildren(index)) {
        ui->tableView->setRootIndex(index);
        SetFileIndexMap();
    }
}

void MainWindow::on_tableView_doubleClicked(const QModelIndex &index)
{
    if(!FileModel->hasChildren(index)){
        const QString filePath = FileModel->filePath(index);
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }
    else{
        ui->tableView->setRootIndex(index);
        ui->FileTreeView->expand(index);
        SetFileIndexMap();
    }
}

void MainWindow::on_splitter_splitterMoved(int pos, int )
{
    if(pos < 10 && treeActive ){
        treeActive = false;
    }
    else if(pos > 10 && !treeActive){
        treeActive = true;
    }
}

void MainWindow::on_actionList_View_triggered()
{
    if(treeActive){
        treeActive = false;
        ui->splitter->setSizes({0,400});
    }
    else{
        treeActive = true;
        ui->splitter->setSizes({100,400});
    }

}

void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    if(ui->tabWidget->count() > 1){
        // move widget before closing current tab:
        if(ui->tabWidget->currentIndex() == index){
            if(index >=1){
                MoveTabWidget(index-1);
            }
            else{
                MoveTabWidget(index+1);
            }
        }
        ui->tabWidget->removeTab(index);
        lastLeftTabIndex = ui->tabWidget->currentIndex();
    }
    else{
        // ilk tab içeriğini sıfırla
        SetTabContent(ui->tabWidget->currentWidget());
    }
}

void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    // Aynı sekmeye tıklanmadıysa
    if (index != lastLeftTabIndex && index != -1) {
        MoveTabWidget(index);
        SetTabContent(ui->tabWidget->currentWidget());
    }
}

