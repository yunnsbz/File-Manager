#ifndef TREEMANAGER_H
#define TREEMANAGER_H

#include <QObject>
#include <QTreeView>
#include <QMap>
#include <QList>
#include <QString>
#include <QTabWidget>
#include <QModelIndex>

#include <FM_Macros.hpp>
FM_BEGIN_NAMESPACE

class FileModelOperations;

class TreeManager : public QObject
{
    Q_OBJECT

public:
    explicit TreeManager(QTreeView* treeView, FileModelOperations* fileModelOp, QTabWidget* tabWidget, QObject* parent);


public:
    void setTreeToDefault();
    void setTreeContent(int tabIndex);

    void navigateToFolder(const QModelIndex &modelIndex, int tabIndex);
    void removeTabExpandedPaths(int tabIndex);
    void swapExpandedPathsMap(int toIndex, int fromIndex);
    void expandTreeView(const QModelIndex &index);

signals:
    void treeViewClicked(const QModelIndex &index);

private slots:
    void onTreeViewClicked(const QModelIndex &index);

private:
    // sinyal oluşturmadan ağacın tüm dallarını kapat
    void collapseAll_noSig();

private:
    FileModelOperations* m_fileModelOp_;
    QTabWidget* m_tabWidget_;
    QTreeView* m_treeView_;


    // treeView için her sekmede açılmış yolları tutar:
    QMap<int, QList<QString>> m_expandedPathsMap;
};

FM_END_NAMESPACE
#endif // TREEMANAGER_H
