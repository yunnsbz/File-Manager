#include "TableManager.h"
#include "FileModelOperations.h"

#include <QFileSystemModel>
#include <QUrl>
#include <QDesktopServices>
#include <QHeaderView>
#include <QTimer>
#include <QAbstractItemView>
#include <QModelIndex>
#include <QVector>

TableManager::TableManager(QTableView* tableView, FileModelOperations* fileModelOp, QObject *parent)
    :
    QObject(parent),
    fileModelOp_(fileModelOp),
    tableView_(tableView)
{
    auto* fileModel = fileModelOp_->GetFileModel();
    tableView->setModel(fileModel);
    SetColumnResize();
    tableView->verticalHeader()->setDefaultSectionSize(10);
    tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void TableManager::SetTableToDefault()
{
    auto* fileModel = fileModelOp_->GetFileModel();
    fileModel->setRootPath("");
    const QModelIndex index = fileModel->index(fileModel->rootPath());
    tableView_->setRootIndex(index);
}

void TableManager::SetTableContent(int tabIndex)
{
    // set table view content:
    auto index = fileModelOp_->GetTabModelIndex(tabIndex);

    if (!index.isValid())
    {
        SetTableToDefault();
        return;
    }

    tableView_->setRootIndex(index);
}

void TableManager::navigateToFolder(int tabIndex, QModelIndex firstColumnIndex)
{
    auto* fileModel = fileModelOp_->GetFileModel();
    if (!fileModel->hasChildren(firstColumnIndex))
    {
        const QString filePath = fileModel->filePath(firstColumnIndex);
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }
    else
    {
        tableView_->setRootIndex(firstColumnIndex);
        fileModelOp_->SetTabModelIndex(tabIndex,firstColumnIndex);
    }
}

void TableManager::SetColumnResize()
{
    QHeaderView* header = tableView_->horizontalHeader();

    // Stretch ile başlangıç yerleşimi (bu işlem interactive modu kapatır. aşağıda tekrar açarız)
    header->setSectionResizeMode(QHeaderView::Stretch);
    tableView_->resizeColumnsToContents();

    // Tablonun güncellenmesini bekle
    QTimer::singleShot(0, this, [this]() {
        auto* header1 = tableView_->horizontalHeader();
        const int columnCount = tableView_->model()->columnCount();

        // stretch değerlerini kaydet:
        QVector<int> widths;
        for (int i = 0; i < columnCount; ++i)
        {
            widths.append(tableView_->columnWidth(i));
        }
        // Kullanıcının sütun genişliklerini değiştirebilmesi için Interactive moda geç (stretch değerlerini değiştirir)
        for (int i = 0; i < columnCount; ++i)
        {
            header1->setSectionResizeMode(i, QHeaderView::Interactive);
        }
        // Stretch'te belirlenmiş genişlikleri geri uygula:
        for (int i = 0; i < columnCount; ++i)
        {
            tableView_->setColumnWidth(i, widths[i]);
        }
    });
}



