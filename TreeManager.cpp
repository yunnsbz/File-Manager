
#include "TreeManager.h"
#include "FileModelOperations.h"
#include "mainwindow.hpp"

#include <QTableView>

#include <algorithm>

TreeManager::TreeManager(QTreeView *treeView, FileModelOperations *fileModelOp, QTabWidget* tabWidget, QObject *parent)
    :
    QObject(parent),
    fileModelOp(fileModelOp),
    tabWidget_(tabWidget),
    treeView(treeView)
{
    auto* fileModel = fileModelOp->GetFileModel();
    treeView->setModel(fileModel);
    treeView->setRootIndex(fileModel->index(fileModel->rootPath()));

    // hiding unnecessary columns
    treeView->hideColumn(1);
    treeView->hideColumn(2);
    treeView->hideColumn(3);

    // tree view açılma ve kapanma durumlarında değişiklikleri tabContents içine kaydetmeliyiz:
    connect(treeView, &QTreeView::expanded, fileModelOp, [=, this](const QModelIndex &index) {
        const QString& path = fileModelOp->GetFilePath(index);
        const int currentTab = tabWidget_->currentIndex();
        if(!ExpandedPathsMap[currentTab].contains(path))
        {
            ExpandedPathsMap[currentTab].append(path);
        }
    });

    connect(treeView, &QTreeView::collapsed, fileModelOp, [=, this](const QModelIndex &index) {
        const QString& path = fileModelOp->GetFilePath(index);
        const int currentTab = tabWidget_->currentIndex();
        ExpandedPathsMap[currentTab].removeOne(path);
    });
}

void TreeManager::setTreeToDefault()
{
    auto* fileModel = fileModelOp->GetFileModel();
    fileModel->setRootPath("");
    const QModelIndex index = fileModel->index(fileModel->rootPath());
    treeView->setRootIndex(index);
    treeView->collapseAll();
}

void TreeManager::CollapseAll_noSig()
{
    // collapse all dediğimde collapse sinyallerinin çalışmasını engellemek için
    treeView->blockSignals(true);   // Tüm sinyalleri kapat
    treeView->collapseAll();
    treeView->blockSignals(false); // Tüm sinyalleri aç
}

void TreeManager::SetTreeContent(int tabIndex)
{
    // set tree view content:
    if(ExpandedPathsMap[tabIndex].isEmpty())
    {
        setTreeToDefault();
        return;
    }

    CollapseAll_noSig();

    for (const auto &path : ExpandedPathsMap[tabIndex])
    {
        const QModelIndex index = fileModelOp->GetFileIndex(path);
        if (index.isValid() && index.model() != nullptr)
        {
            treeView->expand(index);
        }
    }
}

void TreeManager::navigateToFolder(const QModelIndex &modelIndex, int tabIndex)
{
    auto *fileModel = fileModelOp->GetFileModel();
    if (fileModel->hasChildren(modelIndex))
    {
        fileModelOp->SetTabModelIndex(tabIndex, modelIndex);
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
    auto* fileModel = fileModelOp->GetFileModel();
    if(fileModel->hasChildren(modelIndex)){
        treeView->expand(modelIndex);
    }
}
