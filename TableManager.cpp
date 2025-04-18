#include "TableManager.h"
#include "FileModelOperations.h"

#include <QUrl>
#include <QDesktopServices>
#include <QHeaderView>
#include <QTimer>


TableManager::TableManager(QTableView *tableView, QObject *parent)
    :
    QObject(parent),
    tableView(tableView)
{
    auto* fileModel = FileModelOperations::GetFileModel();
    tableView->setModel(fileModel);
    SetColumnResize();
}

void TableManager::SetTableToDefault()
{
    auto* fileModel = FileModelOperations::GetFileModel();
    fileModel->setRootPath("");
    const QModelIndex index = fileModel->index(fileModel->rootPath());
    tableView->setRootIndex(index);
}

void TableManager::SetTableContent(int tabIndex)
{
    // set table view content:
    auto index = FileModelOperations::GetTabModelIndex(tabIndex);

    if (!index.isValid())
    {
        SetTableToDefault();
        return;
    }

    tableView->setRootIndex(index);
}

void TableManager::navigateToFolder(int tabIndex, QModelIndex firstColumnIndex)
{
    auto* fileModel = FileModelOperations::GetFileModel();
    if (!fileModel->hasChildren(firstColumnIndex))
    {
        const QString filePath = fileModel->filePath(firstColumnIndex);
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }
    else
    {
        tableView->setRootIndex(firstColumnIndex);
        FileModelOperations::SetTabModelIndex(tabIndex,firstColumnIndex);
    }
}

void TableManager::SetColumnResize()
{
QHeaderView* header = tableView->horizontalHeader();

    // Stretch ile başlangıç yerleşimi (bu işlem interactive modu kapatır. aşağıda tekrar açarız)
    header->setSectionResizeMode(QHeaderView::Stretch);
    tableView->resizeColumnsToContents();

    // Tablonun güncellenmesini bekle
    QTimer::singleShot(0, this, [this]() {
        QHeaderView* header = tableView->horizontalHeader();
        const int columnCount = tableView->model()->columnCount();

        // stretch değerlerini kaydet:
        QVector<int> widths;
        for (int i = 0; i < columnCount; ++i)
        {
            widths.append(tableView->columnWidth(i));
        }
        // Kullanıcının sütun genişliklerini değiştirebilmesi için Interactive moda geç (stretch değerlerini değiştirir)
        for (int i = 0; i < columnCount; ++i)
        {
            header->setSectionResizeMode(i, QHeaderView::Interactive);
        }
        // Stretch'te belirlenmiş genişlikleri geri uygula:
        for (int i = 0; i < columnCount; ++i)
        {
            tableView->setColumnWidth(i, widths[i]);
        }
    });
}



