#include "FileOperations.h"


FileOperations::FileOperations()
{
    // set model:
    fileModel = new QFileSystemModel(this);

    // set to disk directory
    fileModel->setRootPath("");
}

QFileSystemModel* FileOperations::GetFileModel()
{
    return fileModel;
}

void FileOperations::SetTabModelIndex(int tabIndex, QModelIndex modelIndex)
{
    TabBackHistoryModelIndex[tabIndex].push_back(TabModelIndexMap[tabIndex]);
    qDebug()<< "history added";

    // yeni index'e geçildiğinden ileri geçmiş silinir:
    TabForwardHistoryModelIndex[tabIndex].clear();

    TabModelIndexMap[tabIndex] = modelIndex;
}

QString FileOperations::GetFilePath(QModelIndex modelIndex)
{
    return fileModel->filePath(modelIndex);
}

QModelIndex FileOperations::GetFileIndex(QString path)
{
    return fileModel->index(path);
}

bool FileOperations::IsBackHistoryEmpty(int tabIndex)
{
    return TabBackHistoryModelIndex[tabIndex].isEmpty();
}

bool FileOperations::IsForwardHistoryEmpty(int tabIndex)
{
    return TabForwardHistoryModelIndex[tabIndex].isEmpty();
}

void FileOperations::RemoveTabModelIndex(int tabIndex)
{
    TabModelIndexMap.remove(tabIndex);

    // sekmeden sonra gelenlerin map key'leri (tabIndex'leri) bir geriye alınır:
    auto keys = TabModelIndexMap.keys();
    std::sort(keys.begin(), keys.end(), std::less<int>()); // sıralanmazsa yanlış sekmenin üzerine yazabilir
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

void FileOperations::swapTabModelIndexMap(int toIndex, int fromIndex)
{
    const auto temp = TabModelIndexMap.value(fromIndex);
    TabModelIndexMap[fromIndex] = TabModelIndexMap.value(toIndex);
    TabModelIndexMap[toIndex] = temp;
}

void FileOperations::OnBackButtonClicked(int tabIndex)
{
    if (!TabBackHistoryModelIndex[tabIndex].isEmpty()) {
        TabForwardHistoryModelIndex[tabIndex].append(TabModelIndexMap[tabIndex]);
        auto backContent = TabBackHistoryModelIndex[tabIndex].takeLast();
        TabModelIndexMap[tabIndex] = backContent;
    }
}

void FileOperations::OnForwardButtonClicked(int tabIndex)
{
    if (!TabForwardHistoryModelIndex[tabIndex].isEmpty()) {
        TabBackHistoryModelIndex[tabIndex].append(TabModelIndexMap[tabIndex]);
        auto backContent = TabForwardHistoryModelIndex[tabIndex].takeLast();
        TabModelIndexMap[tabIndex] = backContent;
    }
}

auto FileOperations::GetTabModelIndex(int tabIndex) -> QModelIndex
{
    if(TabModelIndexMap.contains(tabIndex))
    {
        return TabModelIndexMap[tabIndex];
    }
    return {};
}
