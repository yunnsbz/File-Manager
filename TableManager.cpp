#include "TableManager.h"
#include "FileOperations.h"

#include <QUrl>
#include <QDesktopServices>


TableManager::TableManager(QTableView *tableView, QObject *parent)
    :
    QObject(parent),
    tableView(tableView)
{
    auto* fileModel = FileOperations::GetFileModel();
    tableView->setModel(fileModel);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void TableManager::SetTableToDefault()
{
    auto* fileModel = FileOperations::GetFileModel();
    fileModel->setRootPath("");
    const QModelIndex index = fileModel->index(fileModel->rootPath());
    tableView->setRootIndex(index);
}

void TableManager::SetTableContent(int tabIndex)
{
    // set table view content:
    auto index = FileOperations::GetTabModelIndex(tabIndex);

    if (!index.isValid())
    {
        SetTableToDefault();
        return;
    }

    tableView->setRootIndex(index);
}

void TableManager::navigateToFolder(int tabIndex, QModelIndex firstColumnIndex)
{
    auto* fileModel = FileOperations::GetFileModel();
    if (!fileModel->hasChildren(firstColumnIndex))
    {
        const QString filePath = fileModel->filePath(firstColumnIndex);
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    }
    else
    {
        tableView->setRootIndex(firstColumnIndex);
        FileOperations::SetTabModelIndex(tabIndex,firstColumnIndex);
    }
}



