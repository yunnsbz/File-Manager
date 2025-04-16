
#include "TreeManager.h"
#include "FileOperations.h"
#include "mainwindow.hpp"

#include <QTableView>

#include <algorithm>

TreeManager::TreeManager(QTreeView *treeView, QObject *parent)
    :
    QObject(parent),
    mainWindow(static_cast<MainWindow*>(parent)),
    treeView(treeView)
{
    auto* fileModel = FileOperations::GetFileModel();
    treeView->setModel(fileModel);
    treeView->setRootIndex(fileModel->index(fileModel->rootPath()));

    // hiding unnecessary columns
    treeView->hideColumn(1);
    treeView->hideColumn(2);
    treeView->hideColumn(3);

    // tree view açılma ve kapanma durumlarında değişiklikleri tabContents içine kaydetmeliyiz:
    connect(treeView, &QTreeView::expanded, this, [this](const QModelIndex &index) {
        const QString& path = FileOperations::GetFilePath(index);
        const int currentTab = mainWindow->GetCurrentTabIndex();
        ExpandedPathsMap[currentTab].append(path);
    });

    connect(treeView, &QTreeView::collapsed, this, [this](const QModelIndex &index) {
        const QString& path = FileOperations::GetFilePath(index);
        const int currentTab = mainWindow->GetCurrentTabIndex();
        ExpandedPathsMap[currentTab].removeOne(path);
    });

}

void TreeManager::setTreeToDefault()
{
    auto* fileModel = FileOperations::GetFileModel();
    fileModel->setRootPath("");
    const QModelIndex index = fileModel->index(fileModel->rootPath());
    treeView->setRootIndex(index);
    treeView->collapseAll();
}

void TreeManager::SetTreeContent(int tabIndex)
{
    // set tree view content:
    if(ExpandedPathsMap[tabIndex].isEmpty())
    {
        setTreeToDefault();
        return;
    }

    treeView->collapseAll();

    if(!ExpandedPathsMap[tabIndex].isEmpty())
    {
        for (auto &path : ExpandedPathsMap[tabIndex])
        {
            const QModelIndex index = FileOperations::GetFileIndex(path);
            if (index.isValid())
            {
                treeView->expand(index);
            }
        }
    }
    else
    {
        setTreeToDefault();
    }
}

void TreeManager::navigateToFolder(const QModelIndex &modelIndex, int tabIndex)
{
    auto *fileModel = FileOperations::GetFileModel();
    if (fileModel->hasChildren(modelIndex))
    {
        FileOperations::SetTabModelIndex(tabIndex, modelIndex);
    }

    // single click tree expanding and collapsing:
    if (treeView->isExpanded(modelIndex))
    {
        treeView->collapse(modelIndex);
    }
    else
    {
        treeView->expand(modelIndex);
    }
}

void TreeManager::removeTabExpandedPaths(int tabIndex)
{
    ExpandedPathsMap.remove(tabIndex);

    // sekmeden sonra gelenlerin map key'leri (tabIndex'leri) bir geriye alınır:
    auto keys = ExpandedPathsMap.keys();
    std::ranges::sort(keys); // sıralanmazsa yanlış sekmenin üzerine yazabilir
    for(auto _tabIndex : keys)
    {
        if(_tabIndex > tabIndex)
        {
            auto content = ExpandedPathsMap[_tabIndex];
            ExpandedPathsMap.remove(_tabIndex);
            ExpandedPathsMap[_tabIndex-1] = content;
        }
    }
}

void TreeManager::swapExpandedPathsMap(int toIndex, int fromIndex)
{
    const auto temp = ExpandedPathsMap.value(fromIndex);
    ExpandedPathsMap[fromIndex] = ExpandedPathsMap.value(toIndex);
    ExpandedPathsMap[toIndex] = temp;
}

void TreeManager::ExpandTreeView(const QModelIndex &modelIndex)
{
    auto* fileModel = FileOperations::GetFileModel();
    if(fileModel->hasChildren(modelIndex)){
        treeView->expand(modelIndex);
    }
}

auto TreeManager::IsBackHistoryEmpty(int tabIndex) -> bool
{
    return BackHistoryExpandedPaths[tabIndex].isEmpty();
}

auto TreeManager::IsForwardHistoryEmpty(int tabIndex) -> bool
{
    return ForwardHistoryExpandedPaths[tabIndex].isEmpty();
}
