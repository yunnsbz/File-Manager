
#include "TreeManager.h"
#include "FileModelOperations.h"
#include "mainwindow.hpp"

#include <QTableView>
#include <algorithm>

FM_BEGIN_NAMESPACE

TreeManager::TreeManager(QTreeView* treeView, FileModelOperations* fileModelOp, QTabWidget* tabWidget, QObject* parent)
    :
    QObject(parent),
    m_fileModelOp_(fileModelOp),
    m_tabWidget_(tabWidget),
    m_treeView_(treeView)
{
    auto* fileModel = fileModelOp->getFileModel();
    treeView->setModel(fileModel);
    treeView->setRootIndex(fileModel->index(fileModel->rootPath()));

    // hiding unnecessary columns
    treeView->hideColumn(1);
    treeView->hideColumn(2);
    treeView->hideColumn(3);

    connect(treeView, &QTreeView::clicked, this, &TreeManager::onTreeViewClicked);

    // tree view açılma ve kapanma durumlarında değişiklikleri tabContents içine kaydetmeliyiz:
    connect(treeView, &QTreeView::expanded, fileModelOp, [=, this](const QModelIndex &index) {
        const QString& path = fileModelOp->getFilePath(index);
        const int currentTab = m_tabWidget_->currentIndex();
        if(!m_expandedPathsMap[currentTab].contains(path))
        {
            m_expandedPathsMap[currentTab].append(path);
        }
    });

    connect(treeView, &QTreeView::collapsed, fileModelOp, [=, this](const QModelIndex &index) {
        const QString& path = fileModelOp->getFilePath(index);
        const int currentTab = m_tabWidget_->currentIndex();
        m_expandedPathsMap[currentTab].removeOne(path);
    });
}

void TreeManager::setTreeToDefault()
{
    auto* fileModel = m_fileModelOp_->getFileModel();
    fileModel->setRootPath("");
    const QModelIndex index = fileModel->index(fileModel->rootPath());
    m_treeView_->setRootIndex(index);
    m_treeView_->collapseAll();
}

void TreeManager::collapseAll_noSig()
{
    // collapse all dediğimde collapse sinyallerinin çalışmasını engellemek için
    m_treeView_->blockSignals(true);   // Tüm sinyalleri kapat
    m_treeView_->collapseAll();
    m_treeView_->blockSignals(false); // Tüm sinyalleri aç
}

void TreeManager::setTreeContent(int tabIndex)
{
    // set tree view content:
    if(m_expandedPathsMap[tabIndex].isEmpty())
    {
        setTreeToDefault();
        return;
    }

    collapseAll_noSig();

    for (const auto &path : m_expandedPathsMap[tabIndex])
    {
        const QModelIndex index = m_fileModelOp_->getFileIndex(path);
        if (index.isValid() && index.model() != nullptr)
        {
            m_treeView_->expand(index);
        }
    }
}

void TreeManager::navigateToFolder(const QModelIndex &modelIndex, int tabIndex)
{
    auto *fileModel = m_fileModelOp_->getFileModel();
    if (fileModel->hasChildren(modelIndex))
    {
        m_fileModelOp_->setTabModelIndex(tabIndex, modelIndex);
    }

    // single click tree expanding and collapsing:
    if (m_treeView_->isExpanded(modelIndex))
    {
        m_treeView_->collapse(modelIndex);
    }
    else
    {
        m_treeView_->expand(modelIndex);
    }
}

void TreeManager::removeTabExpandedPaths(int tabIndex)
{
    m_expandedPathsMap.remove(tabIndex);

    // sekmeden sonra gelenlerin map key'leri (tabIndex'leri) bir geriye alınır:
    auto keys = m_expandedPathsMap.keys();
    std::ranges::sort(keys); // sıralanmazsa yanlış sekmenin üzerine yazabilir
    for(auto _tabIndex : keys)
    {
        if(_tabIndex > tabIndex)
        {
            auto content = m_expandedPathsMap[_tabIndex];
            m_expandedPathsMap.remove(_tabIndex);
            m_expandedPathsMap[_tabIndex-1] = content;
        }
    }
}

void TreeManager::swapExpandedPathsMap(int toIndex, int fromIndex)
{
    const auto temp = m_expandedPathsMap.value(fromIndex);
    m_expandedPathsMap[fromIndex] = m_expandedPathsMap.value(toIndex);
    m_expandedPathsMap[toIndex] = temp;
}

void TreeManager::expandTreeView(const QModelIndex &modelIndex)
{
    auto* fileModel = m_fileModelOp_->getFileModel();
    if(fileModel->hasChildren(modelIndex)){
        m_treeView_->expand(modelIndex);
    }
}

void TreeManager::onTreeViewClicked(const QModelIndex &modelIndex)
{
    int const tabIndex = m_tabWidget_->currentIndex();
    navigateToFolder(modelIndex, tabIndex);

    emit treeViewClicked(modelIndex);
}

FM_END_NAMESPACE
