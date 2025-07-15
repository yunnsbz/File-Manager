
#include "TreeManager.h"
#include "FileModelOperations.h"
#include "mainwindow.hpp"

#include <QTableView>
#include <algorithm>

FM_BEGIN_NAMESPACE

TreeManager::TreeManager(QTreeView* treeView, FileModelOperations* fileModelOp, QTabWidget* tabWidget, QObject* parent)
    :
    QObject(parent),
    fileModelOp_(fileModelOp),
    tabWidget_(tabWidget),
    treeView_(treeView)
{
    auto* fileModel = fileModelOp->GetFileModel();
    treeView->setModel(fileModel);
    treeView->setRootIndex(fileModel->index(fileModel->rootPath()));

    // hiding unnecessary columns
    treeView->hideColumn(1);
    treeView->hideColumn(2);
    treeView->hideColumn(3);

    connect(treeView, &QTreeView::clicked, this, &TreeManager::onTreeViewClicked);

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
    auto* fileModel = fileModelOp_->GetFileModel();
    fileModel->setRootPath("");
    const QModelIndex index = fileModel->index(fileModel->rootPath());
    treeView_->setRootIndex(index);
    treeView_->collapseAll();
}

void TreeManager::CollapseAll_noSig()
{
    // collapse all dediğimde collapse sinyallerinin çalışmasını engellemek için
    treeView_->blockSignals(true);   // Tüm sinyalleri kapat
    treeView_->collapseAll();
    treeView_->blockSignals(false); // Tüm sinyalleri aç
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
        const QModelIndex index = fileModelOp_->GetFileIndex(path);
        if (index.isValid() && index.model() != nullptr)
        {
            treeView_->expand(index);
        }
    }
}

void TreeManager::navigateToFolder(const QModelIndex &modelIndex, int tabIndex)
{
    auto *fileModel = fileModelOp_->GetFileModel();
    if (fileModel->hasChildren(modelIndex))
    {
        fileModelOp_->SetTabModelIndex(tabIndex, modelIndex);
    }

    // single click tree expanding and collapsing:
    if (treeView_->isExpanded(modelIndex))
    {
        treeView_->collapse(modelIndex);
    }
    else
    {
        treeView_->expand(modelIndex);
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
    auto* fileModel = fileModelOp_->GetFileModel();
    if(fileModel->hasChildren(modelIndex)){
        treeView_->expand(modelIndex);
    }
}

void TreeManager::onTreeViewClicked(const QModelIndex &modelIndex)
{
    int const tabIndex = tabWidget_->currentIndex();
    navigateToFolder(modelIndex, tabIndex);

    emit treeViewClicked(modelIndex);
}

FM_END_NAMESPACE
