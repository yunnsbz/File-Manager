#include "TableManager.h"
#include "FileModelOperations.h"

#include <QUrl>
#include <QDesktopServices>
#include <QHeaderView>


TableManager::TableManager(QTableView *tableView, QObject *parent)
    :
    QObject(parent),
    tableView(tableView)
{
    auto* fileModel = FileModelOperations::GetFileModel();
    tableView->setModel(fileModel);
    tableView->resizeColumnsToContents(); // Başlangıçta içeriklere göre yerleştir
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
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



