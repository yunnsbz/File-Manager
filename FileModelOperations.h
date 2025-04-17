#ifndef FILEMODELOPERATIONS_H
#define FILEMODELOPERATIONS_H

#include <QFileSystemModel>
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
    static auto GetFileModel() -> QFileSystemModel*;
    static auto GetTabModelIndex(int tabIndex) -> QModelIndex;
    static void SetTabModelIndex(int tabIndex, QModelIndex modelIndex);
    static auto GetFilePath(QModelIndex modelIndex) -> QString;
    static auto GetFileIndex(const QString& path) -> QModelIndex;

    static auto IsBackHistoryEmpty(int tabIndex) -> bool;
    static auto IsForwardHistoryEmpty(int tabIndex) -> bool;

    static void RemoveTabModelIndex(int tabIndex);
    static void swapTabModelIndexMap(int toIndex, int fromIndex);
    static void swapTabHistoryModelIndex(int toIndex, int fromIndex);

    static void OnBackButtonClicked(int tabIndex);
    static void OnForwardButtonClicked(int tabIndex);


protected:


private:
    static inline QFileSystemModel* fileModel;
    // NOLINTBEGIN(fuchsia-statically-constructed-objects)
    static inline QMap<int, QModelIndex> TabModelIndexMap;

    // her sekmeye özel olarak ileri ve geri butonları için file modelin index değerlerini turar
    static inline QMap<int, QList<QModelIndex>> TabBackHistoryModelIndex;
    static inline QMap<int, QList<QModelIndex>> TabForwardHistoryModelIndex;
    // NOLINTEND(fuchsia-statically-constructed-objects)
};
#endif // FILEMODELOPERATIONS_H
