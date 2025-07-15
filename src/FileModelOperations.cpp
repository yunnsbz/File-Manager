#include "FileModelOperations.h"

#include <QtLogging>

#include <algorithm>

FM_BEGIN_NAMESPACE

FileModelOperations::FileModelOperations()
    :
    m_fileModel(new QFileSystemModel(this))
{
    // set to disk directory
    m_fileModel->setRootPath("");
    m_fileModel->setReadOnly(false);
}

auto FileModelOperations::getFileModel() -> QFileSystemModel*
{
    return m_fileModel;
}

void FileModelOperations::setTabModelIndex(int tabIndex, QModelIndex modelIndex)
{
    m_tabBackHistoryModelIndex[tabIndex].push_back(m_tabModelIndexMap[tabIndex]);

    // yeni index'e geçildiğinden ileri geçmiş silinir:
    m_tabForwardHistoryModelIndex[tabIndex].clear();

    m_tabModelIndexMap[tabIndex] = modelIndex;
}

auto FileModelOperations::getFilePath(QModelIndex modelIndex) -> QString
{
    return m_fileModel->filePath(modelIndex);
}

auto FileModelOperations::getCurrentPath(int tabIndex) -> QString
{
    return getFilePath(getTabModelIndex(tabIndex));
}

auto FileModelOperations::getFileIndex(const QString& path) -> QModelIndex
{
    if (!QFileInfo::exists(path))
    {
        return {}; // path erişilemiyorsa
    }
    return m_fileModel->index(path);
}

auto FileModelOperations::isBackHistoryEmpty(int tabIndex) -> bool
{
    return m_tabBackHistoryModelIndex[tabIndex].isEmpty();
}

auto FileModelOperations::isForwardHistoryEmpty(int tabIndex) -> bool
{
    return m_tabForwardHistoryModelIndex[tabIndex].isEmpty();
}

void FileModelOperations::removeTabModelIndex(int tabIndex)
{
    m_tabModelIndexMap.remove(tabIndex);

    // sekmeden sonra gelenlerin map key'leri (tabIndex'leri) bir geriye alınır:
    auto keys = m_tabModelIndexMap.keys();
    std::ranges::sort(keys); // sıralanmazsa yanlış sekmenin üzerine yazabilir
    for(auto _tabIndex : keys)
    {
        if(_tabIndex > tabIndex)
        {
            auto content = m_tabModelIndexMap[_tabIndex];
            m_tabModelIndexMap.remove(_tabIndex);
            m_tabModelIndexMap[_tabIndex-1] = content;
        }
    }

    m_tabBackHistoryModelIndex.remove(tabIndex);
    m_tabForwardHistoryModelIndex.remove(tabIndex);
}

void FileModelOperations::swapTabModelIndexMap(int toIndex, int fromIndex)
{
    const auto temp = m_tabModelIndexMap.value(fromIndex);
    m_tabModelIndexMap[fromIndex] = m_tabModelIndexMap.value(toIndex);
    m_tabModelIndexMap[toIndex] = temp;
}

void FileModelOperations::swapTabHistoryModelIndex(int toIndex, int fromIndex)
{
    const auto tempBack = m_tabBackHistoryModelIndex.value(fromIndex);
    m_tabBackHistoryModelIndex[fromIndex] = m_tabBackHistoryModelIndex.value(toIndex);
    m_tabBackHistoryModelIndex[toIndex] = tempBack;
    const auto tempForward = m_tabForwardHistoryModelIndex.value(fromIndex);
    m_tabForwardHistoryModelIndex[fromIndex] = m_tabForwardHistoryModelIndex.value(toIndex);
    m_tabForwardHistoryModelIndex[toIndex] = tempForward;
}

void FileModelOperations::onBackButtonClicked(int tabIndex)
{
    if (!m_tabBackHistoryModelIndex[tabIndex].isEmpty()) {
        m_tabForwardHistoryModelIndex[tabIndex].append(m_tabModelIndexMap[tabIndex]);
        auto backContent = m_tabBackHistoryModelIndex[tabIndex].takeLast();
        m_tabModelIndexMap[tabIndex] = backContent;
    }
}

void FileModelOperations::onForwardButtonClicked(int tabIndex)
{
    if (!m_tabForwardHistoryModelIndex[tabIndex].isEmpty()) {
        m_tabBackHistoryModelIndex[tabIndex].append(m_tabModelIndexMap[tabIndex]);
        auto backContent = m_tabForwardHistoryModelIndex[tabIndex].takeLast();
        m_tabModelIndexMap[tabIndex] = backContent;
    }
}

auto FileModelOperations::getTabModelIndex(int tabIndex) -> QModelIndex
{
    if(m_tabModelIndexMap.contains(tabIndex))
    {
        return m_tabModelIndexMap[tabIndex];
    }
    return {};
}

FM_END_NAMESPACE
