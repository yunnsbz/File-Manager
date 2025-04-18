#include "FileModelOperations.h"

#include <QtLogging>

#include <algorithm>

FileModelOperations::FileModelOperations()
:
fileModel(new QFileSystemModel(this))
{
    // set to disk directory
    fileModel->setRootPath("");
}

auto FileModelOperations::GetFileModel() -> QFileSystemModel*
{
    return fileModel;
}

void FileModelOperations::SetTabModelIndex(int tabIndex, QModelIndex modelIndex)
{
    TabBackHistoryModelIndex[tabIndex].push_back(TabModelIndexMap[tabIndex]);

    // yeni index'e geçildiğinden ileri geçmiş silinir:
    TabForwardHistoryModelIndex[tabIndex].clear();

    TabModelIndexMap[tabIndex] = modelIndex;
}

auto FileModelOperations::GetFilePath(QModelIndex modelIndex) -> QString
{
    return fileModel->filePath(modelIndex);
}

auto FileModelOperations::GetFileIndex(const QString& path) -> QModelIndex
{
    if (!QFileInfo::exists(path)) return QModelIndex(); // path erişilemiyorsa
    return fileModel->index(path);
}

auto FileModelOperations::IsBackHistoryEmpty(int tabIndex) -> bool
{
    return TabBackHistoryModelIndex[tabIndex].isEmpty();
}

auto FileModelOperations::IsForwardHistoryEmpty(int tabIndex) -> bool
{
    return TabForwardHistoryModelIndex[tabIndex].isEmpty();
}

void FileModelOperations::RemoveTabModelIndex(int tabIndex)
{
    TabModelIndexMap.remove(tabIndex);

    // sekmeden sonra gelenlerin map key'leri (tabIndex'leri) bir geriye alınır:
    auto keys = TabModelIndexMap.keys();
    std::ranges::sort(keys); // sıralanmazsa yanlış sekmenin üzerine yazabilir
    for(auto _tabIndex : keys)
    {
        if(_tabIndex > tabIndex)
        {
            auto content = TabModelIndexMap[_tabIndex];
            TabModelIndexMap.remove(_tabIndex);
            TabModelIndexMap[_tabIndex-1] = content;
        }
    }
}

void FileModelOperations::swapTabModelIndexMap(int toIndex, int fromIndex)
{
    const auto temp = TabModelIndexMap.value(fromIndex);
    TabModelIndexMap[fromIndex] = TabModelIndexMap.value(toIndex);
    TabModelIndexMap[toIndex] = temp;
}

void FileModelOperations::swapTabHistoryModelIndex(int toIndex, int fromIndex)
{
    const auto tempBack = TabBackHistoryModelIndex.value(fromIndex);
    TabBackHistoryModelIndex[fromIndex] = TabBackHistoryModelIndex.value(toIndex);
    TabBackHistoryModelIndex[toIndex] = tempBack;
    const auto tempForward = TabForwardHistoryModelIndex.value(fromIndex);
    TabForwardHistoryModelIndex[fromIndex] = TabForwardHistoryModelIndex.value(toIndex);
    TabForwardHistoryModelIndex[toIndex] = tempBack;
}

void FileModelOperations::OnBackButtonClicked(int tabIndex)
{
    if (!TabBackHistoryModelIndex[tabIndex].isEmpty()) {
        TabForwardHistoryModelIndex[tabIndex].append(TabModelIndexMap[tabIndex]);
        auto backContent = TabBackHistoryModelIndex[tabIndex].takeLast();
        TabModelIndexMap[tabIndex] = backContent;
    }
}

void FileModelOperations::OnForwardButtonClicked(int tabIndex)
{
    if (!TabForwardHistoryModelIndex[tabIndex].isEmpty()) {
        TabBackHistoryModelIndex[tabIndex].append(TabModelIndexMap[tabIndex]);
        auto backContent = TabForwardHistoryModelIndex[tabIndex].takeLast();
        TabModelIndexMap[tabIndex] = backContent;
    }
}

auto FileModelOperations::GetTabModelIndex(int tabIndex) -> QModelIndex
{
    if(TabModelIndexMap.contains(tabIndex))
    {
        return TabModelIndexMap[tabIndex];
    }
    return {};
}
