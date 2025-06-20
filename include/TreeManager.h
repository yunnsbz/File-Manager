#ifndef TREEMANAGER_H
#define TREEMANAGER_H

#include <QObject>
#include <QTreeView>
#include <QMap>
#include <QList>
#include <QString>
#include <QTabWidget>
#include <QModelIndex>

class FileModelOperations;

class TreeManager : public QObject
{
    Q_OBJECT

public:
    explicit TreeManager(QTreeView* treeView, FileModelOperations* fileModelOp, QTabWidget* tabWidget, QObject* parent);


public:
    void setTreeToDefault();
    void SetTreeContent(int tabIndex);

    void navigateToFolder(const QModelIndex &modelIndex, int tabIndex);
    void removeTabExpandedPaths(int tabIndex);
    void swapExpandedPathsMap(int toIndex, int fromIndex);
    void ExpandTreeView(const QModelIndex &index);


private:
    // sinyal oluşturmadan ağacın tüm dallarını kapat
    void CollapseAll_noSig();


private:
    FileModelOperations* fileModelOp_;
    QTabWidget* tabWidget_;
    QTreeView* treeView_;


    // treeView için her sekmede açılmış yolları tutar:
    QMap<int, QList<QString>> ExpandedPathsMap;
};

#endif // TREEMANAGER_H
