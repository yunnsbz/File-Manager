#ifndef FILEOPERATIONS_H
#define FILEOPERATIONS_H

#include <qfilesystemmodel.h>
#include <qobject.h>


class FileOperations : public QObject
{
    Q_OBJECT

public:
    FileOperations();

public:
    static auto GetFileModel() -> QFileSystemModel*;
    static auto GetTabModelIndex(int tabIndex) -> QModelIndex;
    static void SetTabModelIndex(int tabIndex, QModelIndex modelIndex);
    static auto GetFilePath(QModelIndex modelIndex) -> QString;
    static auto GetFileIndex(QString path) -> QModelIndex;

    static auto IsBackHistoryEmpty(int tabIndex) -> bool;
    static auto IsForwardHistoryEmpty(int tabIndex) -> bool;

    static void RemoveTabModelIndex(int tabIndex);
    static void swapTabModelIndexMap(int toIndex, int fromIndex);

    static void OnBackButtonClicked(int tabIndex);
    static void OnForwardButtonClicked(int tabIndex);

private:
    static inline QFileSystemModel* fileModel;
    static inline QMap<int, QModelIndex> TabModelIndexMap;

    // her sekmeye özel olarak ileri ve geri butonları için file modelin index değerlerini turar
    static inline QMap<int, QList<QModelIndex>> TabBackHistoryModelIndex;
    static inline QMap<int, QList<QModelIndex>> TabForwardHistoryModelIndex;

};
#endif // FILEOPERATIONS_H
