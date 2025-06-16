#ifndef FILEMODELOPERATIONS_H
#define FILEMODELOPERATIONS_H

#include <QFileSystemModel>
#include <QModelIndex>
#include <QFileInfo>
#include <QObject>
#include <QMap>
#include <QList>
#include <QDir>

class FileModelOperations : public QObject
{
    Q_OBJECT

public:
    FileModelOperations();


public:
    auto GetFileModel() -> QFileSystemModel*;
    auto GetTabModelIndex(int tabIndex) -> QModelIndex;
    void SetTabModelIndex(int tabIndex, QModelIndex modelIndex);
    auto GetFilePath(QModelIndex modelIndex) -> QString;
    auto GetCurrentPath(int tabIndex) -> QString;
    auto GetFileIndex(const QString& path) -> QModelIndex;

    auto IsBackHistoryEmpty(int tabIndex) -> bool;
    auto IsForwardHistoryEmpty(int tabIndex) -> bool;

    void RemoveTabModelIndex(int tabIndex);
    void swapTabModelIndexMap(int toIndex, int fromIndex);
    void swapTabHistoryModelIndex(int toIndex, int fromIndex);

    void OnBackButtonClicked(int tabIndex);
    void OnForwardButtonClicked(int tabIndex);


protected:


private:
    QFileSystemModel* fileModel;

    QMap<int, QModelIndex> TabModelIndexMap;

    // her sekmeye özel olarak ileri ve geri butonları için file modelin index değerlerini turar
    QMap<int, QList<QModelIndex>> TabBackHistoryModelIndex;
    QMap<int, QList<QModelIndex>> TabForwardHistoryModelIndex;

};
#endif // FILEMODELOPERATIONS_H
