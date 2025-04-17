#ifndef TREEMANAGER_H
#define TREEMANAGER_H

#include <QObject>
#include <QTreeView>
#include <QMap>
#include <QList>

class MainWindow;

class TreeManager : public QObject
{
    Q_OBJECT

public:
    explicit TreeManager(QTreeView* treeView, QObject* parent);

    void setTreeToDefault();
    void SetTreeContent(int tabIndex);

    void navigateToFolder(const QModelIndex &modelIndex, int tabIndex);
    void removeTabExpandedPaths(int tabIndex);
    void swapExpandedPathsMap(int toIndex, int fromIndex);

    void ExpandTreeView(const QModelIndex &index);

    auto IsBackHistoryEmpty(int tabIndex) -> bool;
    auto IsForwardHistoryEmpty(int tabIndex) -> bool;


private:
    // sinyal oluşturmadan ağacın tüm dallarını kapat
    void CollapseAll_noSig();


private:
    MainWindow* mainWindow;

    QTreeView* treeView;

    // treeView için her sekmede açılmış yolları tutar:
    QMap<int, QList<QString>> ExpandedPathsMap;

    // her sekmeye özel history:
    using Path = QList<QString>;
    using PathHistory = QList<Path>;
    QMap<int, PathHistory> BackHistoryExpandedPaths;
    QMap<int, PathHistory> ForwardHistoryExpandedPaths;


};

#endif // TREEMANAGER_H
