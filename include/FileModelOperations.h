#ifndef FILEMODELOPERATIONS_H
#define FILEMODELOPERATIONS_H

#include <QFileSystemModel>
#include <QModelIndex>
#include <QFileInfo>
#include <QObject>
#include <QMap>
#include <QList>
#include <QDir>
#include <FM_Macros.hpp>
FM_BEGIN_NAMESPACE

/**
 * file model ile alakalı tüm fonksiyonları barındırır
 */
class FileModelOperations : public QObject
{
    Q_OBJECT

public:
    FileModelOperations();

public:
    auto getFileModel()                         -> QFileSystemModel*;
    auto getTabModelIndex(int tabIndex)         -> QModelIndex;
    auto getFilePath(QModelIndex modelIndex)    -> QString;
    auto getCurrentPath(int tabIndex)           -> QString;
    auto getFileIndex(const QString& path)      -> QModelIndex;

    void setTabModelIndex(int tabIndex, QModelIndex modelIndex);

    auto isBackHistoryEmpty(int tabIndex)       -> bool;
    auto isForwardHistoryEmpty(int tabIndex)    -> bool;

    void removeTabModelIndex(int tabIndex);
    void swapTabModelIndexMap(int toIndex, int fromIndex);
    void swapTabHistoryModelIndex(int toIndex, int fromIndex);

    void onBackButtonClicked(int tabIndex);
    void onForwardButtonClicked(int tabIndex);


protected:


private:
    QFileSystemModel* m_fileModel;

    QMap<int, QModelIndex> m_tabModelIndexMap;

    // her sekmeye özel olarak ileri ve geri butonları için file modelin index değerlerini turar
    QMap<int, QList<QModelIndex>> m_tabBackHistoryModelIndex;
    QMap<int, QList<QModelIndex>> m_tabForwardHistoryModelIndex;

};

FM_END_NAMESPACE
#endif // FILEMODELOPERATIONS_H
